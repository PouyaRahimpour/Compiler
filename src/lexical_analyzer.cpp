#include"lexical_analyzer.h"

class Token {
    private:
        std::string content;
        token_type type;
        int line_number;

    public:
        Token (token_type _type, int _line_number) {
            content = "";
            type = _type;
            line_number = _line_number;
        }
        std::string get_content() {
            return content;
        }
        void set_content(std::string _content) {
            content = _content;
        }
        token_type get_type() {
            return type;
        }
        void set_type(token_type _type) {
            type = _type;
        }
        int get_line_number() {
            return line_number;
        }
        void set_line_number(int _line_number) {
            line_number = _line_number;
        }

        std::string toString() const {
            // TODO to_string for type
            return "< content:" + content + ", type: " + std::to_string(type) + ", line#: " + std::to_string(line_number) + " >";
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

        Token is_space(int &index, std::string &line, int line_number) {
            int len = (int)line.size();
            int state = 0, perv_index = index;

            while (index < len) {
                if (state == 0) {
                    if (std::isspace(line[index])) {
                        state = 1;
                        index++;
                    }
                    else {
                        state = 3;
                    }
                }
                else if (state == 1) {
                    if (!std::isspace(line[index])) {
                        state = 2;
                    }
                }
                else if (state == 2) {
                    index = perv_index;
                    Token token(T_Whitespace, line_number);
                    return token;
                }
                else if (state == 3) {
                    index = perv_index;
                    Token token(Not_Valid, line_number);
                    break;
                }
            }
            
            index = perv_index;
            return Token(Not_Valid, line_number);
        }

//         Token is_comment(int index, std::string &line) {
//             return Token();
//         }
//         Token is_operator(int index, std::string &line) {
//             return Token();
//         }
//         Token is_keyword(int index, std::string &line) {
//             return Token();
//         }
//         Token is_decimal(int index, std::string &line) {
//             return Token();
//         }
//         Token is_hexadecimal(int index, std::string &line) {
//             return Token();
//         }
//         Token is_id(int index, std::string &line) {
//             return Token();
//         }
//         Token is_string(int index, std::string &line) {
//             return Token();
//         }
//         Token is_character(int index, std::string &line) {
//             return Token();
//         }
        void extract(std::string &line) {
            static int line_number = 0;
            line_number++;

            int index = 0, len = (int)line.size();

            while (index < len) {
                Token token = is_space(index, line, line_number);
                if (token.get_type() != Not_Valid) {
                    tokens.push_back(token);
                    continue;
                }

//                 token = is_comment(index, line);
//                 if (token.get_type() != Not_Valid) {
//                     tokens.push_back(token);
//                     continue;
//                 }
// 
//                 token = is_operator(index, line);
//                 if (token.get_type() != Not_Valid) {
//                     tokens.push_back(token);
//                     continue;
//                 }
// 
//                 token = is_keyword(index, line);
//                 if (token.get_type() != Not_Valid) {
//                     tokens.push_back(token);
//                     continue;
//                 }
//                 
//                 token = is_decimal(index, line);
//                 if (token.get_type() != Not_Valid) {
//                     tokens.push_back(token);
//                     continue;
//                 }
// 
//                 token = is_hexadecimal(index, line);
//                 if (token.get_type() != Not_Valid) {
//                     tokens.push_back(token);
//                     continue;
//                 }
// 
//                 token = is_id(index, line);
//                 if (token.get_type() != Not_Valid) {
//                     tokens.push_back(token);
//                     continue;
//                 }
// 
//                 token = is_string(index, line);
//                 if (token.get_type() != Not_Valid) {
//                     tokens.push_back(token);
//                     continue;
//                 }
// 
//                 token = is_character(index, line);
//                 if (token.get_type() != Not_Valid) {
//                     tokens.push_back(token);
//                     continue;
//                 }
            }

        }
        void read_tokens() {
            in.open(in_address);
            if (!in.is_open()) {
                std::cerr << "error: couldn't open input file" << std::endl;
                exit(FILE_ERROR);
            }
            std::string line;
            while (getline(in, line)) {
                line.push_back('\n');
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
