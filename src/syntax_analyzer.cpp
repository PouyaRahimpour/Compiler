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
        bool operator < (const Variable &other) const {
            return name < other.get_name();
        }

        std::string toString() {
            return name;
        }
        friend std::ostream& operator<<(std::ostream &out, Variable &var) {
            return out << var.toString();
        }
};

enum Rule_type {
    EMPTY,
    SYNCH,
    VALID
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
        void set_type(Rule_type _type) {
            type = _type;
        }
        void set_head(Variable _head) {
            head = _head;
        }
        Variable& get_head() {
            return head;
        }

        void add_to_body(Variable var) {
            body.push_back(var);
        }
        std::vector<Variable>& get_body() {
            return body;
        }
        
        Rule_type get_type() {
            return type;
        }

        std::string toString() {
            std::string res;
            res += head.get_name() + " -> ";
            for (auto &part: body) {
                res += part.get_name() + " ";
            }
            return res;
        }
        friend std::ostream& operator<<(std::ostream &out, Rule &rule) {
            return out << rule.toString();
        }
};

class SyntaxAnalyzer {

    private:
        std::vector<Rule> rules;
        std::vector<Token> tokens;
        std::string out_address;
        std::ofstream out;
        std::set<Variable> variables, terminals;
        std::map<std::pair<Variable, Variable>, Rule> table;
        std::map<Variable, std::set<Variable>> graph;
        std::map<Variable, std::set<Variable>> firsts, follows;
        std::map<Variable, bool> first_done, follow_done;
        std::map<token_type, std::string> match;
        Variable eps;

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
            match[Eof] = "$";
        }

        void print_firsts(Variable var) {
            std::cout << "First[" + var.get_name() + "]:";
            for (auto f : firsts[var]) {
                std::cout << " " << f;
            }
            std::cout << std::endl;
        }

        void print_follows(Variable var) {
            std::cout << "Follows[" + var.get_name() + "]:";
            for (auto f : follows[var]) {
                std::cout << " " << f;
            }
            std::cout << std::endl;
        }

        void extract(std::string line) {
            Variable head, tmp;

            // TODO: strip line
            std::string head_str = "", body_str = "";
            int i = 0;
            for (; i < (int)line.size(); i++) {
                if (line[i] != ' ') {
                    head_str += line[i];
                }
                else {
                    while (line[i] == ' ' || line[i] == '-' || line[i] == '>') {
                        i++;
                    }
                    while (i < (int)line.size()) {
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

        // TODO DSU
        // add rules of every var to itself
        void calc_first (Variable &var) {
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
                        for (auto f : firsts[part_body]) {
                            if (f == eps) {
                                exist_eps = true;
                            }
                            else {
                                firsts[var].insert(f);
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

        void calc_firsts() {
            for (auto var : variables) {
                calc_first(var);
            }
            for (auto term : terminals) {
                calc_first(term);
            }
        }

        void calc_follow(Variable var) {
            for (Rule &r : rules) {
                std::vector<Variable> &body = r.get_body();
                int rule_sz = (int)body.size();

                for (int i = 0; i < rule_sz; i++) {
                    if (body[i] == var) {
                        bool all_eps = true;
                        for (int j = i + 1; j < rule_sz; j++) {
                            bool has_eps = false;
                            for (auto v : firsts[body[j]]) {
                                if (v == eps) {
                                    has_eps = true;
                                }
                                else {
                                    follows[var].insert(v);
                                }
                            }
                            if (!has_eps) {
                                all_eps = false;
                                break;
                            }
                        }
                        if (all_eps && !(r.get_head() == var)) {
                            /*
                            if (!r.get_head().get_follow_done()) {
                                calc_follow(r.get_head());
                            }
                            */
                            for (auto v : follows[r.get_head()]) {
                                follows[var].insert(v);
                            }
                            graph[r.get_head()].insert(var);
                        }
                    }
                }
            }

            //var.set_follow_done(true);
        }

        void relaxation() {
            std::queue<Variable> q;
            for (auto var : variables) {
                q.push(var);
            }

            while (!q.empty()) {
                Variable var = q.front();
                q.pop();

                for (auto v : graph[var]) {
                    int old_sz = (int)follows[v].size();
                    for (auto u : follows[var]) {
                        follows[v].insert(u);
                    }
                    int new_sz = (int)follows[v].size();
                    if (new_sz > old_sz) {
                        q.push(v);
                    }
                }
            }
        }

        void calc_follows() {
            for (auto var : variables) {
                if (var.get_name() == START_VAR) {
                    follows[var].insert(Variable("$", TERMINAL));
                }
                calc_follow(var);
            }
            relaxation();

            /*
            for (auto var : variables) {
                print_follows(var);
            }
            */
        }
        
        std::set<Variable> sf_first(std::vector<Variable>& body) {
            std::set<Variable> res;
    
            bool all_eps = true;
            for (auto part_body : body) {
                bool has_eps = false;
                for (auto f : firsts[part_body]) {
                    if (f == eps) {
                        has_eps = true; 
                    }
                    else {
                        res.insert(f);
                    }
                }
                if (!has_eps) {
                    all_eps = false;
                    break;
                }
            }
            if (all_eps) {
                res.insert(eps);
            }

            return res;
        }

        void union_set(std::set<Variable>& s1, std::set<Variable>& s2) {
            for (Variable v: s2) {
                s1.insert(v);
            }
        }

        // TODO: table from file
        void make_table() {
            for (Rule &r : rules) {
                std::set<Variable> first = sf_first(r.get_body());
                bool has_eps = false;
                //std::cout << first.size() << std::endl;
                for (Variable v : first) {
                    if (v == eps) {
                        has_eps = true;
                    } 
                    else {
                        table[{r.get_head(), v}] = r;
                    }
                }
                if (has_eps) {
                    for (Variable v : follows[r.get_head()]) {
                        table[{r.get_head(), v}] = r;
                    }
                } 
                else {
                    for (Variable v : follows[r.get_head()]) {
                        if (table[{r.get_head(), v}].get_type() != VALID) {
                            table[{r.get_head(), v}] = Rule(SYNCH);
                        }
                    }
                }
            }

            // add this rule: else_stmt -> <else> stmt
            Rule r;
            r.set_type(VALID);
            r.set_head(Variable("else_stmt", VARIABLE));
            r.add_to_body(Variable("else", TERMINAL));
            r.add_to_body(Variable("stmt", VARIABLE));
            table[{Variable("else_stmt", VARIABLE), Variable("else", TERMINAL)}] = r;
        }
    

    public:
        SyntaxAnalyzer(std::vector<Token> _tokens, std::string output_file) {
            out_address = output_file;
            tokens = _tokens;
            eps.set_name("eps");
            eps.set_type(TERMINAL);
        }

        void make_tree() {
            std::stack<Variable> stack;
            int index = 0;
            tokens.push_back(Token(Eof));
            int token_sz = (int) tokens.size();
            stack.push(Variable("$", TERMINAL));
            stack.push(Variable(START_VAR, VARIABLE));

            while (index < token_sz && stack.size()) {
                Variable top = stack.top();
                stack.pop();
                Variable term = Variable(match[tokens[index].get_type()], TERMINAL);

                if (top.get_type() == TERMINAL) {
                    if (term == top) {
                        index++;
                    } else {
                        std::cout << term << ' ' << top << std::endl;
                        std::cout << "ERROR: terminals don't match" << std::endl;
                    }
                } 
                else {
                    bool found = false;
                    Rule rule = table[{top, term}];
                    if (rule.get_type() == VALID) {
                        std::vector body = rule.get_body();
                        std::reverse(body.begin(), body.end());
                        for (Variable var: body) {
                            if (!(var == eps)) {
                                stack.push(var);
                            }
                        }
                        //std::cout << "Valid" << std::endl;
                        //std::cout << rule << std::endl;
                    } 
                    else if (rule.get_type() == SYNCH) {
                        std::cout << "ERROR: synch" << std::endl;
                    } 
                    else if (rule.get_type() == EMPTY) {
                        std::cout << top << ' ' << term << std::endl;
                        std::cout << "ERROR: empty cell" << std::endl;
                        index++;
                        stack.push(top);
                    }
                    else {
                        std::cout << "UNKNOWN ERROR" << std::endl;
                    }
                }
            }
            std::cout << "Parsed tree successfully" << std::endl;
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
            set_matches();
            make_table();

            make_tree();
        }
};
