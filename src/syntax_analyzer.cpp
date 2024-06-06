#include "syntax_analyzer.h"
#include <queue>


enum Type {
    TERMINAL,
    VARIABLE
};

class Variable {

    private:
        std::string name;
        Type type;


    public:
        Variable() {
        }

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
        //std::map<std::pair<std::string, std::string>, Rule> table;
        std::map<std::pair<Variable, Variable>, Rule> table;
        std::map<Variable, std::set<Variable>> graph;
        std::map<Variable, std::set<Variable>> firsts, follows;
        std::map<Variable, bool> first_done, follow_done;
        Variable eps;

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

            for (auto var : variables) {
                print_follows(var);
            }
        }
        
        std::set<Variable>& sf_first(std::vector<Variable>& body) {

        }

        void union_set(std::set<Variable>& s1, std::set<Variable>& s2) {
            for (Variable v: s2) {
                s1.insert(v);
            }
        }

        // TODO: table from file
        void make_table() {
            for (Rule& r: rules) {
                std::set<Variable> first = sf_first(r.get_body());
                bool has_eps = false;
                for (Variable v: first) {
                    if (v == eps) {
                        has_eps = true;
                    } else {
                        table[{r.get_head(), v}] = r;
                    }
                }
                if (has_eps) {
                    for (Variable v: follows[r.get_head()]) {
                        table[{r.get_head(), v}] = r;
                    }
                } else {
                    for (Variable v: follows[r.get_head()]) {
                        table[{r.get_head(), v}] = Rule(SYNCH);
                    }
                }
            }
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
            return;
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
            //make_table();
        }
};
