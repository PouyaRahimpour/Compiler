#include "lexical_analyzer.h"
#include "utils.h"

char spaces[] = {9, 10, 13, 32};

std::string key_words[NUM_KEYWORDS] = {
    "bool",
    "break",
    "char",
    "continue",
    "else",
    "false",
    "for",
    "if",
    "int",
    "print",
    "return",
    "true"
};


bool isspace(const char &ch) {
    for (auto &sp: spaces) {
        if (ch == sp) {
            return true;
        }
    }
    return false;
}

class LexicalAnalyzer {
    private:
        std::string in_address, out_address;
        std::ifstream in;
        std::ofstream out;
        std::vector<Token> tokens;
        int num_erros = 0;

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
                    if (line[index] == '\n') {
                        state = 2;
                    }
                    else if (!isspace(line[index])) {
                        state = 2;
                        continue;
                    }
                }
                else if (state == 2) {
                    return Token(T_Whitespace, line_number);
                }
                else if (state == 3) {
                    index = perv_index;
                    return Token(Invalid, line_number);
                }
                index++;
            }
            
            return Token(T_Whitespace, line_number);
        }

        Token is_comment(int &index, const std::string &line, const int &line_number) {
            int len = (int)line.size();
            int state = 0, perv_index = index;

            std::string content = "";

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
                    if (line[index] == '\n' || line[index] == 13) {
                        state = 3;
                        continue;
                    }
                    content += line[index];
                }
                else if (state == 3) {
                    return Token(T_Comment, line_number, content);
                }
                else if (state == 4) {
                    index = perv_index;
                    return Token(Invalid, line_number);
                }
                index++;
            }
            
            index = perv_index;
            return Token(Invalid, line_number);
        }

        Token is_operator(int &index, const std::string &line, const int &line_number) {
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
                    return Token(T_LP, line_number, "(");
                }
                else if (state == 2) {
                    return Token(T_RP, line_number, ")");
                }
                else if (state == 3) {
                    return Token(T_LB, line_number, "[");
                }
                else if (state == 4) {
                    return Token(T_RB, line_number, "]");
                }
                else if (state == 5) {
                    return Token(T_LC, line_number, "{");
                }
                else if (state == 6) {
                    return Token(T_RC, line_number, "}");
                }
                else if (state == 7) {
                    return Token(T_Comma, line_number, ",");
                }
                else if (state == 8) {
                    return Token(T_Semicolon, line_number, ";");
                }
                else if (state == 9) {
                    return Token(T_AOp_PL, line_number, "+");
                }
                else if (state == 10) {
                    return Token(T_AOp_MN, line_number, "-");
                }
                else if (state == 11) {
                    return Token(T_AOp_ML, line_number, "*");
                }
                else if (state == 12) {
                    return Token(T_AOp_DV, line_number, "/");
                }
                else if (state == 13) {
                    return Token(T_AOp_RM, line_number, "%");
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
                    return Token(T_LOp_AND, line_number, "&&");
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
                    return Token(T_LOp_OR, line_number, "||");
                }
                else if (state == 18) {
                    if (line[index] == '=') {
                        state = 19;
                    }
                    else {
                        state = 20;
                        continue;
                    }
                }
                else if (state == 19) {
                    return Token(T_ROp_E, line_number, "==");
                }
                else if (state == 20) {
                    return Token(T_Assign, line_number, "=");
                }
                else if (state == 21) {
                    if (line[index] == '=') {
                        state = 22;
                    }
                    else {
                        state = 23;
                        continue;
                    }
                }
                else if (state == 22) {
                    return Token(T_ROp_NE, line_number, "!=");
                }
                else if (state == 23) {
                    return Token(T_LOp_NOT, line_number, "!");
                }
                else if (state == 24) {
                    if (line[index] == '=') {
                        state = 25;
                    }
                    else {
                        state = 26;
                        continue;
                    }
                }
                else if (state == 25) {
                    return Token(T_ROp_LE, line_number, "<=");
                }
                else if (state == 26) {
                    return Token(T_ROp_L, line_number, "<");
                }
                else if (state == 27) {
                    if (line[index] == '=') {
                        state = 28;
                    }
                    else {
                        state = 29;
                        continue;
                    }
                }
                else if (state == 28) {
                    return Token(T_ROp_GE, line_number, ">=");
                }
                else if (state == 29) {
                    return Token(T_ROp_G, line_number, ">");
                }
                else if (state == 30) {
                    index = perv_index;
                    return Token(Invalid, line_number);
                }
                index++;
            }
            
            index = perv_index;
            return Token(Invalid, line_number);
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
                            if ((index + sz) < (int)line.size() && (std::isalnum(line[index + sz]) || line[index + sz] == '_')) {
                                continue;
                            }
                            index += sz;
                            state = i + 1;
                            break;
                        }
                    }
                }
                else if (state == 1) {
                    return Token(T_Bool, line_number, "bool");
                }
                else if (state == 2) {
                    return Token(T_Break, line_number, "break");
                }
                else if (state == 3) {
                    return Token(T_Char, line_number, "char");
                }
                else if (state == 4) {
                    return Token(T_Continue, line_number, "continue");
                }
                else if (state == 5) {
                    return Token(T_Else, line_number, "else");
                }
                else if (state == 6) {
                    return Token(T_False, line_number, "false");
                }
                else if (state == 7) {
                    return Token(T_For, line_number, "for");
                }
                else if (state == 8) {
                    return Token(T_If, line_number, "if");
                }
                else if (state == 9) {
                    return Token(T_Int, line_number, "int");
                }
                else if (state == 10) {
                    return Token(T_Print, line_number, "print");
                }
                else if (state == 11) {
                    return Token(T_Return, line_number, "return");
                }
                else if (state == 12) {
                    return Token(T_True, line_number, "true");
                }
                else if (state == 13) {
                    index = perv_index;
                    return Token(Invalid, line_number);
                }
            }

            index = perv_index;
            return Token(Invalid, line_number);
        }

        Token is_decimal(int &index, const std::string &line, const int &line_number) {
            int len = (int)line.size();
            int state = 0, perv_index = index;
            std::string content = "";

            while (index < len) {
                if (state == 0) {
                    if (std::isdigit(line[index])) {
                        state = 1;
                        content += line[index];
                    }
                    else {
                        state = 3;
                    }
                }
                else if (state == 1) {
                    if (std::isdigit(line[index])) {
                        state = 1;
                        content += line[index];
                    }
                    else {
                        state = 2;
                        continue;
                    }
                }
                else if (state == 2) {
                    return Token(T_Decimal, line_number, content);
                }
                else if (state == 3) {
                    index = perv_index;
                    return Token(Invalid, line_number);
                }
                index++;
            }

            index = perv_index;
            return Token(Invalid, line_number);
        }

        Token is_hexadecimal(int &index, const std::string &line, const int &line_number) {
            int len = (int)line.size();
            int state = 0, perv_index = index;

            std::string content = "";

            while (index < len) {
                if (state == 0) {
                    if (line[index] == '0') {
                        state = 1;
                    }
                    else {
                        state = 5;
                    }
                }
                else if (state == 1) {
                    if (std::tolower(line[index]) == 'x') {
                        state = 2;
                    }
                    else {
                        state = 5;
                    }
                }
                else if (state == 2) {
                    if (std::isxdigit(line[index])) {
                        state = 3;
                        content += line[index];
                    }
                    else {
                        state = 5;
                    }
                }
                else if (state == 3) {
                    if (std::isxdigit(line[index])) {
                        state = 3;
                        content += line[index];
                    }
                    else {
                        state = 4;
                        continue;
                    }
                }
                else if (state == 4) {
                    return Token(T_Hexadecimal, line_number, content);
                }
                else if (state == 5) {
                    index = perv_index;
                    return Token(Invalid, line_number);
                }
                index++;
            }

            index = perv_index;
            return Token(Invalid, line_number);
        }

        Token is_id(int &index, const std::string &line, const int &line_number) {
            int len = (int)line.size();
            int state = 0, perv_index = index;
            std::string content = "";

            while (index < len) {
                if (state == 0) {
                    if (std::isalpha(line[index]) || line[index] == '_') {
                        content += line[index];
                        state = 1;
                    }
                    else {
                        state = 3;
                    }
                }
                else if (state == 1) {
                    if (std::isalnum(line[index]) || line[index] == '_') {
                        content += line[index];
                        state = 1;
                    }
                    else {
                        state = 2;
                        continue;
                    }
                }
                else if (state == 2) {
                    return Token(T_Id, line_number, content);
                }
                else if (state == 3) {
                    index = perv_index;
                    return Token(Invalid, line_number);
                }
                index++;
            }

            index = perv_index;
            return Token(Invalid, line_number);

        }

        Token is_string(int &index, const std::string &line, const int &line_number) {
            int len = (int)line.size();
            int state = 0, perv_index = index;
            std::string content = "";

            while (index < len) {
                if (state == 0) {
                    if (line[index] == '"') {
                        state = 1;
                    }
                    else {
                        state = 4;
                    }
                }
                else if (state == 1) {
                    if (line[index] == '\\') {
                        state = 2;
                    }
                    else if (line[index] == '"') {
                        state = 3;
                    } 
                    else {
                        state = 1;
                        content += line[index];
                    }
                }
                else if (state == 2) {
                    state = 1;
                    content += line[index];
                }
                else if (state == 3) {
                    return Token(T_String, line_number, content);
                }
                else if (state == 4) {
                    index = perv_index;
                    return Token(Invalid, line_number);
                }
                index++;
            }

            index = perv_index;
            return Token(Invalid, line_number);
        }

        Token is_character(int &index, const std::string &line, const int &line_number) {
            int len = (int)line.size();
            int state = 0, perv_index = index;
            std::string content = "";

            while (index < len) {
                if (state == 0) {
                    if (line[index] == '\'') {
                        state = 1;
                    }
                    else {
                        state = 6;
                    }
                }
                else if (state == 1) {
                    if (line[index] == '\\') {
                        state = 2;
                    }
                    else {
                        state = 5;
                    }
                }
                else if (state == 2) {
                    state = 3;
                    content += line[index];
                }
                else if (state == 3) {
                    if (line[index] == '\'') {
                        state = 4;
                    }
                    else {
                        state = 6;
                    }
                }
                else if (state == 4) {
                    return Token(T_Character, line_number, content);
                }
                else if (state == 5) {
                    if (line[index] == '\'') {
                        state = 4;
                    }
                    else {
                        state = 6;
                    }
                }
                else if (state == 6) {
                    index = perv_index;
                    return Token(Invalid, line_number);
                }
                index++;
            }

            index = perv_index;
            return Token(Invalid, line_number);
        }

        void extract(std::string &line) {
            static int line_number = 0;
            line_number++;

            int index = 0, len = (int)line.size();

            while (index < len) {
                Token token = is_space(index, line, line_number);
                if (token.get_type() != Invalid) {
                    if (TOKENIZE_WHITESPACE) {
                        tokens.push_back(token);
                    }
                    continue;
                }

                token = is_comment(index, line, line_number);
                if (token.get_type() != Invalid) {
                    if (TOKENIZE_COMMENT) {
                        tokens.push_back(token);
                    }
                    continue;
                }

                token = is_operator(index, line, line_number);
                if (token.get_type() != Invalid) {
                    tokens.push_back(token);
                    continue;
                }

                token = is_keyword(index, line, line_number);
                if (token.get_type() != Invalid) {
                    tokens.push_back(token);
                    continue;
                }
                
                token = is_hexadecimal(index, line, line_number);
                if (token.get_type() != Invalid) {
                    tokens.push_back(token);
                    continue;
                }

                token = is_decimal(index, line, line_number);
                if (token.get_type() != Invalid) {
                    tokens.push_back(token);
                    continue;
                }

                token = is_id(index, line, line_number);
                if (token.get_type() != Invalid) {
                    tokens.push_back(token);
                    continue;
                }

                token = is_string(index, line, line_number);
                if (token.get_type() != Invalid) {
                    tokens.push_back(token);
                    continue;
                }

                token = is_character(index, line, line_number);
                if (token.get_type() != Invalid) {
                    tokens.push_back(token);
                    continue;
                }
                
                std::string error_message = "Lexical Error: character '" + std::to_string(line[index]) + "' is invalid, " + "line:" + std::to_string(line_number); 
                std::cerr << RED << error_message << WHITE << std::endl; 
                std::cerr << "----------------------------------------------------------------" << std::endl;
                num_erros++;
                index++;
            }
        }

        void read_tokens() {
            in.open(in_address);
            if (!in.is_open()) {
                std::cerr << RED << "File error: couldn't open input file" << WHITE << std::endl;
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
                std::cerr << RED << "File error: couldn't open output file" << WHITE << std::endl;
                exit(FILE_ERROR);
            }
            for (Token token: tokens) {
                out << token << '\n';
            }
            out.close();
        }

    public:
        LexicalAnalyzer(std::string input_file, std::string output_file) {
            in_address = input_file;
            out_address = output_file;
        }

        void tokenize() {
            read_tokens();
            if (num_erros == 0) {
                std::cout << GREEN << "Tokenize Complete" << WHITE << std::endl;
            }
            else {
                std::cout << YELLOW << "Tokenize Complete" << WHITE << std::endl;
            }
        }
        
        void write() {
            write_tokens();
        }
        
        std::vector<Token> get_tokens() {
            return tokens;
        }
};
