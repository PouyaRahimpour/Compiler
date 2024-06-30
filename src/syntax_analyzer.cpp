#include "syntax_analyzer.h"

std::vector<std::string> split(std::string s, char ch = ' ') {
    int n = s.size();
    std::vector<std::string> sp;

    std::string tmp = "";
    for (int i = 0; i < n; i++) {
        if (s[i] != ch) {
            tmp += s[i];
        }
        else if (tmp != "") {
            sp.push_back(tmp);
            tmp = "";
        }
    }
    if (tmp != "") {
        sp.push_back(tmp);
    }

    return sp;
}

std::string strip(std::string s, char ch = ' ') {
    while (!s.empty() && s.back() == ch) {
        s.pop_back();
    }

    int n = s.size();
    std::string res = "";
    for (int i = 0; i < n; i++) {
        if (s[i] == ch) {
            continue;
        }
        while (i < n) {
            res += s[i];
            i++;
        }
    }
    return res;
}


class Rule {
    private:
        Symbol head;
        std::vector<Symbol> body;
        rule_type type;

    public:
        Rule() {
            type = EMPTY;
        }
        Rule(rule_type _type) {
            type = _type;
        }

        void set_head(Symbol _head) {
            head = _head;
        }
        Symbol& get_head() {
            return head;
        }
        void set_type(rule_type _type) {
            type = _type;
        }
        rule_type get_type() {
            return type;
        }
        void add_to_body(Symbol var) {
            body.push_back(var);
        }
        std::vector<Symbol>& get_body() {
            return body;
        }

        std::string toString() {
            std::string res = "";
            res += head.toString() + " -> ";
            for (auto &part : body) {
                res += part.toString() + " ";
            }
            if (type == SYNCH) {
                res = "SYNCH";
            }
            if (type == EMPTY) {
                res = "EMPTY";
            }
            return res;
        }
        friend std::ostream& operator << (std::ostream &out, Rule &rule) {
            return out << rule.toString();
        }
};

class SyntaxAnalyzer {
    private:
        std::string out_address;
        std::ofstream out;
        std::vector<Token> tokens;
        std::vector<Rule> rules;
        std::map<Symbol, std::vector<Rule>> self_rules;
        std::set<Symbol> variables, terminals;
        std::map<Symbol, std::set<Symbol>> firsts, follows;
        std::map<Symbol, bool> first_done;
        std::map<Symbol, std::set<Symbol>> graph;
        std::map<std::pair<Symbol, Symbol>, Rule> table;
        std::map<token_type, std::string> match;
        Tree<Symbol> tree;
        bool has_par[200];
        bool has_error;
        Symbol eps;

        void extract(std::string line) {
            Symbol head, tmp;

            line = strip(line);
            std::string head_str = "", body_str = "";
            int len = (int)line.size();
            for (int i = 0; i < len; i++) {
                if (line[i] != ' ') {
                    head_str += line[i];
                }
                else {
                    while (line[i] == ' ' || line[i] == '-' || line[i] == '>') {
                        i++;
                    }
                    while (i < len) {
                        body_str += line[i];
                        i++;
                    }
                }
            }
            head.set_name(head_str);
            head.set_type(VARIABLE);
            variables.insert(head);

            std::vector<std::string> rules_str = split(body_str, '#');
            int number_rules = (int)rules_str.size();
            if (number_rules < 1) {
                std::cerr << RED << "Grammar Error: Invalid grammar in file" << WHITE << std::endl;
                exit(GRAMMAR_ERROR);
            }

            for (int i = 0; i < number_rules; i++) {
                Rule rule;
                rule.set_head(head);
                rule.set_type(VALID);
                std::vector<std::string> rule_parts = split(rules_str[i]);

                for (int j = 0; j < (int)rule_parts.size(); j++) {
                    if (rule_parts[j][0] == '<') {
                        tmp.set_name(rule_parts[j].substr(1, rule_parts[j].size() - 2));
                        tmp.set_type(TERMINAL);
                        terminals.insert(tmp);
                    } 
                    else {
                        tmp.set_name(rule_parts[j]);
                        tmp.set_type(VARIABLE);
                        variables.insert(tmp);
                    }
                    rule.add_to_body(tmp);
                }
                rules.push_back(rule);
                self_rules[rule.get_head()].push_back(rule);
            }
        }

        void calc_firsts() {
            for (auto term : terminals) {
                calc_first(term);
            }
            for (auto var : variables) {
                calc_first(var);
            }
        }

        void calc_first(Symbol var) {
            if (var.get_type() == TERMINAL) {
                firsts[var].insert(var);
                first_done[var] = true;
                return;
            }

            for (auto &rule : self_rules[var]) {
                bool exist_eps = false;
                for (auto &part_body : rule.get_body()) {
                    if (first_done[part_body] == false) {
                        calc_first(part_body);
                    }
                    exist_eps = false;
                    for (auto first : firsts[part_body]) {
                        if (first == eps) {
                            exist_eps = true;
                        }
                        else {
                            firsts[var].insert(first);
                        }
                    }
                    if (!exist_eps) {
                        break;
                    }
                }
                if (exist_eps) {
                    firsts[var].insert(eps);
                }
            }
            first_done[var] = true;
        }

        void print_firsts() {
            for (auto term : terminals) {
                print_first(term);
            }
            for (auto var : variables) {
                print_first(var);
            }
        }

        void print_first(Symbol var) {
            std::cout << "First[" + var.get_name() + "]:";
            for (auto first : firsts[var]) {
                std::cout << " " << first;
            }
            std::cout << std::endl;
        }

        void calc_follows() {
            for (auto var : variables) {
                if (var.get_name() == START_VAR) {
                    follows[var].insert(Symbol("$", TERMINAL));
                }
                calc_follow(var);
            }
            relaxation();
        }

        void calc_follow(Symbol var) {
            for (auto &rule : rules) {
                std::vector<Symbol> &body = rule.get_body();
                int body_len = (int)body.size();

                for (int i = 0; i < body_len; i++) {
                    if (body[i] == var) {
                        bool all_eps = true;
                        for (int j = i + 1; j < body_len; j++) {
                            bool has_eps = false;
                            for (auto first : firsts[body[j]]) {
                                if (first == eps) {
                                    has_eps = true;
                                }
                                else {
                                    follows[var].insert(first);
                                }
                            }
                            if (!has_eps) {
                                all_eps = false;
                                break;
                            }
                        }
                        if (all_eps && rule.get_head() != var) {
                            for (auto follow : follows[rule.get_head()]) {
                                follows[var].insert(follow);
                            }
                            graph[rule.get_head()].insert(var);
                        }
                    }
                }
            }
        }

        void relaxation() {
            std::set<Symbol> set;
            for (auto var : variables) {
                set.insert(var);
            }

            while (!set.empty()) {
                Symbol var = *set.begin();
                set.erase(set.begin());

                for (auto v : graph[var]) {
                    int old_size = (int)follows[v].size();
                    for (auto follow : follows[var]) {
                        follows[v].insert(follow);
                    }
                    int new_size = (int)follows[v].size();
                    if (new_size > old_size) {
                        set.insert(v);
                    }
                }
            }
        }

        void print_follows() {
            for (auto var : variables) {
                print_follow(var);
            }
        }

        void print_follow(Symbol var) {
            std::cout << "Follow[" + var.get_name() + "]:";
            for (auto follow : follows[var]) {
                std::cout << " " << follow;
            }
            std::cout << std::endl;
        }

        void set_matches() {
            match[T_Bool] = "bool";
            match[T_Break] = "break";
            match[T_Char] = "char";
            match[T_Continue] = "continue";
            match[T_Else] = "else";
            match[T_False] = "false";
            match[T_For] = "for";
            match[T_If] = "if";
            match[T_Int] = "int";
            match[T_Print] = "print";
            match[T_Return] = "return";
            match[T_True] = "true";

            match[T_AOp_PL] = "+";
            match[T_AOp_MN] = "-";
            match[T_AOp_ML] = "*";
            match[T_AOp_DV] = "/";
            match[T_AOp_RM] = "%";

            match[T_ROp_L] = "<";
            match[T_ROp_G] = ">";
            match[T_ROp_LE] = "<=";
            match[T_ROp_GE] = ">=";
            match[T_ROp_NE] = "!=";
            match[T_ROp_E] = "==";

            match[T_LOp_AND] = "&&";
            match[T_LOp_OR] = "||";
            match[T_LOp_NOT] = "!";

            match[T_Assign] = "=";
            match[T_LP] = "(";
            match[T_RP] = ")";
            match[T_LC] = "{";
            match[T_RC] = "}";
            match[T_LB] = "[";
            match[T_RB] = "]";
            match[T_Semicolon] = ";";
            match[T_Comma] = ",";

            match[T_Id] = "id";
            match[T_Decimal] = "decimal";
            match[T_Hexadecimal] = "hexadecimal";
            match[T_String] = "string";
            match[T_Character] = "character";

            match[Invalid] = "invalid";
            match[Eof] = "$";
        }

        void make_table() {
            for (auto &rule : rules) {
                std::vector<Symbol> &body = rule.get_body();
                Symbol head = rule.get_head();
                bool all_eps = true;
                for (auto part_body : body) {
                    bool has_eps = false;
                    for (auto first : firsts[part_body]) {
                        if (first == eps) {
                            has_eps = true; 
                        }
                        else {
                            table[{head, first}] = rule;
                        }
                    }
                    if (!has_eps) {
                        all_eps = false;
                        break;
                    }
                }

                if (all_eps) {
                    for (Symbol var : follows[rule.get_head()]) {
                        table[{rule.get_head(), var}] = rule;
                    }
                } 
                else {
                    for (Symbol var : follows[rule.get_head()]) {
                        if (table[{rule.get_head(), var}].get_type() != VALID) {
                            table[{rule.get_head(), var}] = Rule(SYNCH);
                        }
                    }
                }
            }

            // add rule (else_stmt -> <else> stmt)
            Rule rule;
            rule.set_type(VALID);
            rule.set_head(Symbol("else_stmt", VARIABLE));
            rule.add_to_body(Symbol("else", TERMINAL));
            rule.add_to_body(Symbol("stmt", VARIABLE));
            table[{Symbol("else_stmt", VARIABLE), Symbol("else", TERMINAL)}] = rule;

            // error handeling
            Symbol semicolon = Symbol(";", TERMINAL);
            Symbol closed_curly = Symbol("}", TERMINAL);
            for (auto var : variables) {
                if (table[{var, semicolon}].get_type() != VALID) {
                    table[{var, semicolon}] = Rule(SYNCH);
                }
                if (table[{var, closed_curly}].get_type() != VALID) {
                    table[{var, closed_curly}] = Rule(SYNCH);
                }
            }
        }
        
        void write_table() {
            std::ofstream table_file;
            table_file.open(TABLE_PATH);
            if (!table_file.is_open()) {
                std::cerr << RED << "File Error: Couldn't open table file for write" << WHITE << std::endl;
                exit(FILE_ERROR);
            }
            for (auto &col : table) {
                Symbol head1 = col.first.first;
                Symbol head2 = col.first.second;
                Rule rule = col.second;

                table_file << "# " << head1 << ' ' << head2 << '\n';
                table_file << rule << '\n';
            }

            table_file.close();
        }

        void read_table() {
            std::ifstream table_file;
            table_file.open(TABLE_PATH);
            if (!table_file.is_open()) {
                std::cerr << RED << "File Error: Couldn't open table file for read" << WHITE << std::endl;
                exit(FILE_ERROR);
            }

            std::string line;
            Symbol head1("", VARIABLE), head2("", TERMINAL);
            while (getline(table_file, line)) {
                line = strip(line);
                std::vector<std::string> line_parts = split(line);
                int part_rules = (int)line_parts.size();
                if (line_parts[0] != "SYNCH" && line_parts[0] != "EMPTY" && part_rules < 3) {
                    std::cerr << RED << "Table Error: Invalid table in file" << WHITE << std::endl;
                    exit(GRAMMAR_ERROR);
                }

                if (line_parts[0] == "#") {
                    head1.set_name(line_parts[1]);
                    head2.set_name(line_parts[2].substr(1, (int)line_parts[2].size() - 2));
                }
                else if (line_parts[0] == "SYNCH") {
                    Rule rule(SYNCH);
                    table[{head1, head2}] = rule;
                }
                else if (line_parts[0] == "EMPTY") {
                    Rule rule(EMPTY);
                    table[{head1, head2}] = rule;
                }
                else {
                    Symbol head(line_parts[0], VARIABLE), tmp;
                    Rule rule(VALID);
                    rule.set_head(head);

                    for (int i = 2; i < part_rules; i++) {
                        if (line_parts[i][0] == '<') {
                            tmp.set_name(line_parts[i].substr(1, line_parts[i].size() - 2));
                            tmp.set_type(TERMINAL);
                        } 
                        else {
                            tmp.set_name(line_parts[i]);
                            tmp.set_type(VARIABLE);
                        }
                        rule.add_to_body(tmp);
                    }
                    table[{head1, head2}] = rule;
                }
            }
            table_file.close();
        }

        void write_tree(Node<Symbol>* node, int num = 0, bool last = false) {
            Symbol var = node->get_data();

            for (int i = 0; i < num * TAB - TAB; i++) {
                if (has_par[i]) {
                    out << "│";
                }
                else {
                    out << " ";
                }
            }
            if (num) {
                if (last) {
                    out << "└── ";
                }
                else {
                    out << "├── ";
                }
            }
            out << var << "\n";
            if (node->get_data().get_content() != "") {
                for (int i = 0; i < num * TAB; i++) {
                    if (has_par[i]) {
                        out << "│";
                    }
                    else {
                        out << " ";
                    }
                }
                out << "└── '" << node->get_data().get_content() << "'" << "\n";
            }

            has_par[num * TAB] = true;
            std::deque<Node<Symbol>*> children = node->get_children();
            for (auto child : children) {
                bool end = false;
                if (child == children.back()) {
                    has_par[num * TAB] = false;
                    end = true;
                }
                write_tree(child, num + 1, end);
            }
        }

    public:
        SyntaxAnalyzer(std::vector<Token> _tokens, std::string output_file) {
            tokens = _tokens;
            out_address = output_file;
            eps.set_name("eps");
            eps.set_type(TERMINAL);
            has_error = false;
        }

        void update_grammar() {
            std::ifstream in;
            in.open(GRAMMAR_PATH);
            if (!in.is_open()) {
                std::cerr << RED << "File Error: Couldn't open grammar input file" << WHITE << std::endl;
                exit(FILE_ERROR);
            }

            std::string line;
            while (getline(in, line)) {
                if (line != "") {
                    extract(line);
                }
            }
            in.close();

            calc_firsts();
            calc_follows();
            make_table();
            write_table();
        }

        void make_tree(bool update = true) {
            if (update) {
                update_grammar();
            }
            else {
                read_table();
            }
            set_matches();

            std::stack<Node<Symbol>*> stack;
            int index = 0;
            tokens.push_back(Token(Eof));
            int tokens_len = (int)tokens.size();

            Node<Symbol>* Eof = new Node<Symbol>(Symbol("$", TERMINAL), NULL);
            stack.push(Eof);
            Node<Symbol>* root = new Node<Symbol>(Symbol(START_VAR, VARIABLE), NULL);
            stack.push(root);

            tree.set_root(root);

            while (index < tokens_len && stack.size()) {
                Node<Symbol>* top_node = stack.top();
                Symbol top_var = top_node->get_data(); 
                stack.pop();

                Symbol term = Symbol(match[tokens[index].get_type()], TERMINAL);
                int line_number = tokens[index].get_line_number();

                if (top_var.get_type() == TERMINAL) {
                    if (term == top_var) {
                        std::string content = tokens[index].get_content();
                        top_node->get_data().set_content(content);
                        top_node->get_data().set_line_number(line_number);
                        index++;
                    } 
                    else {
                        std::cerr << RED << "Syntax Error: Terminals don't match, line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "Expected '" + top_var.get_name() +  "' , but found '" + term.get_name() + "' instead." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;
                        has_error = true;
                    }
                } 
                else {
                    Rule rule = table[{top_var, term}];
                    if (rule.get_type() == VALID) {
                        top_node->get_data().set_line_number(line_number);

                        std::vector<Symbol> body = rule.get_body();
                        std::reverse(body.begin(), body.end());
                        for (auto var : body) {
                            Node<Symbol>* node = new Node<Symbol>(var, top_node);
                            top_node->push_front_children(node);
                            if (var != eps) {
                                stack.push(node);
                            }
                        }
                    } 
                    else if (rule.get_type() == SYNCH) {
                        std::cerr << RED << "Syntax Error: Synch, line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "Synced by adding '" + top_var.get_name() + "' before '" + term.get_name() + "'." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;
                        has_error = true;
                    } 
                    else if (rule.get_type() == EMPTY) {
                        std::cerr << RED << "Syntax Error: Empty cell, line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "Ignored '" + term.get_name() + "' due to syntax mismatch." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;
                        index++;
                        stack.push(top_node);
                        has_error = true;
                    }
                    else {
                        std::cerr << "Unknown Error: ?, line: " << line_number << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;
                        has_error = true;
                    }
                }
            }
            if (!has_error) {
                std::cout << GREEN << "Parsed tree successfully" << WHITE << std::endl;
            }
            else {
                std::cout << YELLOW << "Parsed tree unsuccessfully" << WHITE << std::endl;
            }
        }

        void write() {
            if (has_error) {
                return;
            }

            out.open(out_address);
            if (!out.is_open()) {
                std::cerr << RED << "File Error: Couldn't open output file" << WHITE << std::endl;
                exit(FILE_ERROR);
            }
            std::fill(has_par, has_par + 200, false);
            write_tree(tree.get_root());
            out.close();
        }

        Tree<Symbol> get_tree() {
            return tree; 
        }
};
