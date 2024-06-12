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

class Variable {
    private:
        std::string name;
        Type type;

    public:
        Variable() {}
        Variable(std::string _name, Type _type) {
            name = _name;
            type = _type;
        }
 
        void set_name(std::string _name) {
            name = _name;
        }
        std::string get_name() const {
            return name;
        }
        void set_type(Type _type) {
            type = _type;
        }
        Type get_type() const {
            return type;
        }

        bool operator == (const Variable &other) const {
            return name == other.get_name() && type == other.get_type();
        }
        bool operator != (const Variable &other) const {
            return name != other.get_name() || type != other.get_type();
        }
        bool operator < (const Variable &other) const {
            return name < other.get_name();
        }
        bool operator > (const Variable &other) const {
            return name > other.get_name();
        }

        std::string toString() {
            return name;
        }
        friend std::ostream& operator << (std::ostream &out, Variable &var) {
            return out << var.toString();
        }
};

class Rule {
    private:
        Variable head;
        std::vector<Variable> body;
        Rule_type type;

    public:
        Rule() {
            type = EMPTY;
        }
        Rule(Rule_type _type) {
            type = _type;
        }

        void set_head(Variable _head) {
            head = _head;
        }
        Variable& get_head() {
            return head;
        }
        void set_type(Rule_type _type) {
            type = _type;
        }
        Rule_type get_type() {
            return type;
        }
        void add_to_body(Variable var) {
            body.push_back(var);
        }
        std::vector<Variable>& get_body() {
            return body;
        }

        std::string toString() {
            std::string res = "";
            res += head.get_name() + " -> ";
            for (auto &part : body) {
                res += part.get_name() + " ";
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
        std::vector<Token> tokens;
        std::vector<Rule> rules;
        std::set<Variable> variables, terminals;
        std::map<Variable, std::set<Variable>> firsts, follows;
        std::map<Variable, bool> first_done;
        std::map<Variable, std::set<Variable>> graph;
        std::map<std::pair<Variable, Variable>, Rule> table;
        std::map<token_type, std::string> match;
        Variable eps; // TODO delete eps?

        // TODO add rules of every var to itself && change calc_first
        void extract(std::string line) {
            Variable head, tmp;

            // TODO: strip line
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
                std::cerr << "Grammar Error: Invalid grammar in file" << std::endl;
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

        void calc_first(Variable var) {
            if (var.get_type() == TERMINAL) {
                firsts[var].insert(var);
                first_done[var] = true;
                return;
            }

            for (auto &rule : rules) {
                if (rule.get_head() == var) {
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

        void print_first(Variable var) {
            std::cout << "First[" + var.get_name() + "]:";
            for (auto first : firsts[var]) {
                std::cout << " " << first;
            }
            std::cout << std::endl;
        }

        void calc_follows() {
            for (auto var : variables) {
                if (var.get_name() == START_VAR) {
                    follows[var].insert(Variable("$", TERMINAL));
                }
                calc_follow(var);
            }
            relaxation();
        }

        void calc_follow(Variable var) {
            for (auto &rule : rules) {
                std::vector<Variable> &body = rule.get_body();
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

        // TODO use set instead queue
        void relaxation() {
            std::queue<Variable> q;
            for (auto var : variables) {
                q.push(var);
            }

            while (!q.empty()) {
                Variable var = q.front();
                q.pop();

                for (auto v : graph[var]) {
                    int old_size = (int)follows[v].size();
                    for (auto follow : follows[var]) {
                        follows[v].insert(follow);
                    }
                    int new_size = (int)follows[v].size();
                    if (new_size > old_size) {
                        q.push(v);
                    }
                }
            }
        }

        void print_follows() {
            for (auto var : variables) {
                print_follow(var);
            }
        }

        void print_follow(Variable var) {
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

        // TODO write tabel on file && if UPDATE_GRAMMAR is false read table from file table.txt
        void make_table() {
            for (auto &rule : rules) {
                std::vector<Variable> &body = rule.get_body();
                Variable head = rule.get_head();
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
                    for (Variable var : follows[rule.get_head()]) {
                        table[{rule.get_head(), var}] = rule;
                    }
                } 
                else {
                    for (Variable var : follows[rule.get_head()]) {
                        if (table[{rule.get_head(), var}].get_type() != VALID) {
                            table[{rule.get_head(), var}] = Rule(SYNCH);
                        }
                    }
                }
            }

            // add rule (else_stmt -> <else> stmt)
            Rule rule;
            rule.set_type(VALID);
            rule.set_head(Variable("else_stmt", VARIABLE));
            rule.add_to_body(Variable("else", TERMINAL));
            rule.add_to_body(Variable("stmt", VARIABLE));
            table[{Variable("else_stmt", VARIABLE), Variable("else", TERMINAL)}] = rule;
        }

    public:
        SyntaxAnalyzer(std::vector<Token> _tokens, std::string output_file) {
            tokens = _tokens;
            out_address = output_file;
            eps.set_name("eps");
            eps.set_type(TERMINAL);
        }

        void update_grammar() {
            std::ifstream in;
            in.open(GRAMMAR_PATH);
            if (!in.is_open()) {
                std::cerr << "File error: couldn't open grammar input file" << std::endl;
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
        }

        // write tree on file syntax.txt
        // TODO colored error messages
        // TODO better error messages
        void make_tree() {
            set_matches();

            std::stack<Node<Variable>*> stack;
            int index = 0;
            tokens.push_back(Token(Eof));
            int tokens_len = (int)tokens.size();

            Node<Variable> Eof(Variable("$", TERMINAL));
            stack.push(&Eof);
            Node<Variable> root(Variable(START_VAR, VARIABLE));
            stack.push(&root);

            Tree<Variable> tree(&root);

            while (index < tokens_len && stack.size()) {
                Node<Variable>* top_node = stack.top();
                Variable top_var = top_node->get_data(); 
                stack.pop();

                Variable term = Variable(match[tokens[index].get_type()], TERMINAL);
                int line_number = tokens[index].get_line_number();

                if (top_var.get_type() == TERMINAL) {
                    if (term == top_var) {
                        std::string content = tokens[index].get_content();
                        top_node->set_content(content);
                        index++;
                    } 
                    else {
                        std::cout << "Syntax ERROR: terminals don't match, line: " << line_number << std::endl;
                    }
                } 
                else {
                    Rule rule = table[{top_var, term}];
                    if (rule.get_type() == VALID) {
                        std::vector<Variable> body = rule.get_body();
                        std::reverse(body.begin(), body.end());
                        for (auto var : body) {
                            Node<Variable>* node = new Node<Variable>(var, top_node);
                            top_node->push_front_children(node);
                            if (var != eps) {
                                stack.push(node);
                            }
                        }
                    } 
                    else if (rule.get_type() == SYNCH) {
                        std::cout << "Syntax Error: synch, line: " << line_number << std::endl;
                    } 
                    else if (rule.get_type() == EMPTY) {
                        std::cout << "Syntax Error: empty cell, line: " << line_number << std::endl;
                        index++;
                        stack.push(top_node);
                    }
                    else {
                        std::cout << "Unknown Error, line: " << line_number << std::endl;
                    }
                }
            }
            std::cout << "Parsed tree successfully" << std::endl;
            tree.print_tree(tree.get_root());
        }
};
