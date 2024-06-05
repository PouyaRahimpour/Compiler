#include "syntax_analyzer.h"

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

        void set_name(std::string _name) {
            name = _name;
        }

        void set_type(Type _type) {
            type = _type;
        }
        std::string get_name() {
            return name;
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

        void add_to_body(Variable var) {
            body.push_back(var);
        }

        std::string toString() {
            std::string res;
            res += "head: " + head.get_name() + "\nbody: ";
            for (auto &part: body) {
                res += part.get_name() + ", ";
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

        void extract(std::string line) {
            Rule rule;
            Variable head, tmp;
            // TODO: strip line
            std::vector<std::string> line_parts = split(line);
            int line_size = (int)line_parts.size();
            if (line_size < 3) {
                std::cerr << "Grammar Error: Invalid grammar in file" << std::endl;
                exit(GRAMMAR_ERROR);
            }
            head.set_name(line_parts[0]);
            head.set_type(VARIABLE);
            rule.set_head(head);

            for (int i = 2; i < line_size; i++) {
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
            rules.push_back(rule);
            std::cout << rule << std::endl;
        }

    public:
        SyntaxAnalyzer(std::vector<Token> _tokens, std::string output_file) {
            out_address = output_file;
            tokens = _tokens;
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
        }
};
