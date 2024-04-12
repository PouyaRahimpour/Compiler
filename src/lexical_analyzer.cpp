#include"lexical_analyzer.h"

class Token {
    private:
        int id;
        token_type type;
        int line_number;
    public:
        std::string toString() const {
            // TODO to_string for type
            return "< id:" + std::to_string(id) + ", type: " + std::to_string(type) + ", line#: " + std::to_string(line_number) + " >";
        }
        friend std::ostream& operator<<(std::ostream &out, const Token &token) {
            return out << token.toString();
        }
};

class LexicalAnalyzer {
    private:
        std::string in_address, out_address;
        std::ifstream in;
        std::ofstream out;
        std::vector<Token> tokens;

        LexicalAnalyzer(std::string input_file, std::string output_file) {
            in_address = input_file;
            out_address = output_file;
        }
        void extract(std::string line) {
            //TODO
        }
        void read_tokens() {
            in.open(in_address);
            if (!in.is_open()) {
                std::cerr << "error: couldn't open input file" << std::endl;
                exit(FILE_ERROR);
            }
            std::string line;
            while (getline(in, line)) {
                extract(line);
            }
            in.close();
        }

        void write_tokens() {
            out.open(out_address);
            if (!out.is_open()) {
                std::cerr << "error: couldn't open output file" << std::endl;
                exit(FILE_ERROR);
            }
            for (Token token: tokens) {
                out << token;
            }
            out.close();
        }

    public:
        void start() {
            read_tokens();
            write_tokens();
            std::cout << "mischief managed!" << std::endl;
        }
};