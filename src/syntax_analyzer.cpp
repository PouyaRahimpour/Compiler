#include "syntax_analyzer.h"
#include <ostream>



enum Type {
    TERMINAL,
    VARIABLE
};

class Variable {
    private:
        std::string name;
        Type type;
        std::set<Variable> first, follow;
        bool first_done, follow_done;

    public:
        Variable() {
            first_done = 0;
            follow_done = 0;
        }

        void set_first_done(bool _set) {
            first_done = _set;
        }
        bool get_first_done() {
            return first_done;
        }
        void set_name(std::string _name) {
            name = _name;
        }

        void set_type(Type _type) {
            type = _type;
        }

        std::string get_name() const {
            return name;
        }

        Type get_type() const {
            return type;
        }

        std::set<Variable>& get_first() {
            return first;
        }

        void add_to_first(Variable _first) {
            first.insert(_first);
        }

        void add_to_follow(Variable _follow) {
            follow.insert(_follow);

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

        void print_firsts() const {
            std::cout << "First = [";
            for (auto f : first) {
                std::cout << f;
                if (!(f == *first.rbegin())) {
                    std::cout << ", ";
                }
            }
            std::cout << "]";
        }
};

class Rule {

    private:
        Variable head;
        std::vector<Variable> body;

    public:
        void set_head(Variable _head) {
            head = _head;
        }
        Variable get_head() {
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
        std::set<Variable> variables;
        Variable eps;

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
                var.add_to_first(var);
                var.set_first_done(true);
                return;
            }

            for (auto &rule : rules) {
                if (rule.get_head() == var) {
                    bool exist_eps = false;
                    for (auto &part_body : rule.get_body()) {
                        if (part_body.get_first_done() == false) {
                            calc_first(part_body);
                        }
                        exist_eps = false;
                        for (auto f : part_body.get_first()) {
                            if (f == eps) {
                                exist_eps = true;
                            }
                            else {
                                var.add_to_first(f);
                            }
                        }
                        if (!exist_eps) {
                            break;
                        }
                    }
                    if (exist_eps) {
                        var.add_to_first(eps);
                    }
                }
            }
            var.set_first_done(true);
        }

        void calc_firsts() {
            for (auto var : variables) {
                calc_first(var);
            }
        }

        void calc_follows() {

        }

    public:
        SyntaxAnalyzer(std::vector<Token> _tokens, std::string output_file) {
            out_address = output_file;
            tokens = _tokens;
            eps.set_name("eps");
            eps.set_type(TERMINAL);
        }

        void make_tree() {
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
        }
};
