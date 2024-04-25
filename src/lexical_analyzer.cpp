#include"lexical_analyzer.h"

// TODO namespace
// TODO reutrn Token()
bool isspace(const char &ch) {
    char spaces[] = {' ', '\t'};
    for (auto &sp: spaces) {
        if (ch == sp) {
            return true;
        }
    }

    return false;
}

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

        Token is_space(int &index, const std::string &line, const int &line_number) {
            int len = (int)line.size();
            int state = 0, perv_index = index;

            while (index < len) {
                if (state == 0) {
                    if (isspace(line[index])) {
                        state = 1;
                    }
                    else {
                        state = 3;
                    }
                }
                else if (state == 1) {
                    if (!isspace(line[index])) {
                        state = 2;
                    }
                }
                else if (state == 2) {
                    Token token(T_Whitespace, line_number);
                    return token;
                }
                else if (state == 3) {
                    index = perv_index;
                    Token token(Not_Valid, line_number);
                    return token;
                }
                index++;
            }
            
            index = perv_index;
            return Token(Not_Valid, line_number);
        }

        Token is_comment(int index, const std::string &line, const int &line_number) {
            int len = (int)line.size();
            int state = 0, perv_index = index;

            while (index < len) {
                if (state == 0) {
                    if (line[index] == '/') {
                        state = 1;
                    }
                    else {
                        state = 4;
                    }
                }
                else if (state == 1) {
                    if (line[index] == '/') {
                        state = 2;
                    }
                    else {
                        state = 4;
                    }
                }
                else if (state == 2) {
                    if (line[index] == '\n') {
                        state = 3;
                    }
                }
                else if (state == 3) {
                    Token token(T_Comment, line_number);
                    return token;
                }
                else if (state == 4) {
                    index = perv_index;
                    Token token(Not_Valid, line_number);
                    return token;
                }
                index++;
            }
            
            index = perv_index;
            return Token(Not_Valid, line_number);
        }

        Token is_operator(int index, const std::string &line, const int &line_number) {
            int len = (int)line.size();
            int state = 0, perv_index = index;

            while (index < len) {
                if (state == 0) {
                    if (line[index] == '(') {
                        state = 1;
                    }
                    else if (line[index] == ')') {
                        state = 2;
                    }
                    else if (line[index] == '[') {
                        state = 3;
                    }
                    else if (line[index] == ']') {
                        state = 4;
                    }
                    else if (line[index] == '{') {
                        state = 5;
                    }
                    else if (line[index] == '}') {
                        state = 6;
                    }
                    else if (line[index] == ',') {
                        state = 7;
                    }
                    else if (line[index] == ';') {
                        state = 8;
                    }
                    else if (line[index] == '+') {
                        state = 9;
                    }
                    else if (line[index] == '-') {
                        state = 10;
                    }
                    else if (line[index] == '*') {
                        state = 11;
                    }
                    else if (line[index] == '/') {
                        state = 12;
                    }
                    else if (line[index] == '%') {
                        state = 13;
                    }
                    else if (line[index] == '&') {
                        state = 14;
                    }
                    else if (line[index] == '|') {
                        state = 16;
                    }
                    else if (line[index] == '=') {
                        state = 18;
                    }
                    else if (line[index] == '!') {
                        state = 21;
                    }
                    else if (line[index] == '<') {
                        state = 24;
                    }
                    else if (line[index] == '>') {
                        state = 27;
                    }
                    else {
                        state = 30;
                    }
                }
                else if (state == 1) {
                    Token token(T_LP, line_number);
                    return token;
                }
                else if (state == 2) {
                    Token token(T_RP, line_number);
                    return token;
                }
                else if (state == 3) {
                    Token token(T_LB, line_number);
                    return token;
                }
                else if (state == 4) {
                    Token token(T_RB, line_number);
                    return token;
                }
                else if (state == 5) {
                    Token token(T_LC, line_number);
                    return token;
                }
                else if (state == 6) {
                    Token token(T_RC, line_number);
                    return token;
                }
                else if (state == 7) {
                    Token token(T_Comma, line_number);
                    return token;
                }
                else if (state == 8) {
                    Token token(T_Semicolon, line_number);
                    return token;
                }
                else if (state == 9) {
                    Token token(T_AOp_PL, line_number);
                    return token;
                }
                else if (state == 10) {
                    Token token(T_AOp_MN, line_number);
                    return token;
                }
                else if (state == 11) {
                    Token token(T_AOp_ML, line_number);
                    return token;
                }
                else if (state == 12) {
                    Token token(T_AOp_DV, line_number);
                    return token;
                }
                else if (state == 13) {
                    Token token(T_AOp_RM, line_number);
                    return token;
                }
                else if (state == 14) {
                    if (line[index] == '&') {
                        state = 15;
                    }
                    else {
                        state = 30;
                    }
                }
                else if (state == 15) {
                    Token token(T_LOp_AND, line_number);
                    return token;
                }
                else if (state == 16) {
                    if (line[index] == '|') {
                        state = 17;
                    }
                    else {
                        state = 30;
                    }
                }
                else if (state == 17) {
                    Token token(T_LOp_OR, line_number);
                    return token;
                }
                else if (state == 18) {
                    if (line[index] == '=') {
                        state = 19;
                    }
                    else {
                        state = 20;
                    }
                }
                else if (state == 19) {
                    Token token(T_ROp_E, line_number);
                    return token;
                }
                else if (state == 20) {
                    Token token(T_Assign, line_number);
                    return token;
                }
                else if (state == 21) {
                    if (line[index] == '=') {
                        state = 22;
                    }
                    else {
                        state = 23;
                    }
                }
                else if (state == 22) {
                    Token token(T_ROp_NE, line_number);
                    return token;
                }
                else if (state == 23) {
                    Token token(T_LOp_NOT, line_number);
                    return token;
                }
                else if (state == 24) {
                    if (line[index] == '=') {
                        state = 25;
                    }
                    else {
                        state = 26;
                    }
                }
                else if (state == 25) {
                    Token token(T_ROp_LE, line_number);
                    return token;
                }
                else if (state == 26) {
                    Token token(T_ROp_L, line_number);
                    return token;
                }
                else if (state == 27) {
                    if (line[index] == '=') {
                        state = 28;
                    }
                    else {
                        state = 29;
                    }
                }
                else if (state == 28) {
                    return Token(T_ROp_GE, line_number);
                }
                else if (state == 29) {
                    return Token(T_ROp_G, line_number);
                }
                else if (state == 30) {
                    index = perv_index;
                    return Token(Not_Valid, line_number);
                }
                index++;
            }
            
            index = perv_index;
            return Token(Not_Valid, line_number);
        }

        Token is_keyword(int &index, const std::string &line, const int &line_number) {
            int len = (int)line.size();
            int state = 0, perv_index = index;

            while (index < len) {
                if (state == 0) {
                    state = 13;
                    for (int i = 0; i < NUM_KEYWORDS; i++) {
                        int sz = (int)key_words[i].size();
                        if (line.std::string::substr(index, sz) == key_words[i]) {
                            index += sz;
                            state = i + 1;
                            break;
                        }
                    }
                }
                else if (state == 1) {
                    return Token(T_Bool, line_number);
                }
                else if (state == 2) {
                    return Token(T_Break, line_number);
                }
                else if (state == 3) {
                    return Token(T_Char, line_number);
                }
                else if (state == 4) {
                    return Token(T_Continue, line_number);
                }
                else if (state == 5) {
                    return Token(T_Else, line_number);
                }
                else if (state == 6) {
                    return Token(T_False, line_number);
                }
                else if (state == 7) {
                    return Token(T_For, line_number);
                }
                else if (state == 8) {
                    return Token(T_If, line_number);
                }
                else if (state == 9) {
                    return Token(T_Int, line_number);
                }
                else if (state == 10) {
                    return Token(T_Print, line_number);
                }
                else if (state == 11) {
                    return Token(T_Return, line_number);
                }
                else if (state == 12) {
                    return Token(T_True, line_number);
                }
                else if (state == 13) {
                    index = perv_index;
                    return Token(Not_Valid, line_number);
                }
            }

            index = perv_index;
            return Token(Not_Valid, line_number);
        }
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
