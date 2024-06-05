#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#pragma once

#define SUCCESS 0
#define FAILIURE 1
#define FILE_ERROR 2
#define GRAMMAR_ERROR 3

enum token_type {
    T_Bool,
    T_Break,
    T_Char, 
    T_Continue,
    T_Else,
    T_False,
    T_For,
    T_If,
    T_Int,
    T_Print,
    T_Return,
    T_True,

    T_AOp_PL,
    T_AOp_MN,
    T_AOp_ML,
    T_AOp_DV,
    T_AOp_RM,

    T_ROp_L,
    T_ROp_G,
    T_ROp_LE,
    T_ROp_GE,
    T_ROp_NE,
    T_ROp_E,

    T_LOp_AND,
    T_LOp_OR,
    T_LOp_NOT,

    T_Assign,
    T_LP,
    T_RP,
    T_LC,
    T_RC,
    T_LB,
    T_RB,
    T_Semicolon,
    T_Comma,
    T_Id,
    T_Decimal,
    T_Hexadecimal,
    T_String,
    T_Character,
    T_Comment,
    T_Whitespace,

    Invalid
};

std::string type_to_string[] = {
    "T_Bool",
    "T_Break",
    "T_Char", 
    "T_Continue",
    "T_Else",
    "T_False",
    "T_For",
    "T_If",
    "T_Int",
    "T_Print",
    "T_Return",
    "T_True",

    "T_AOp_PL",
    "T_AOp_MN",
    "T_AOp_ML",
    "T_AOp_DV",
    "T_AOp_RM",

    "T_ROp_L",
    "T_ROp_G",
    "T_ROp_LE",
    "T_ROp_GE",
    "T_ROp_NE",
    "T_ROp_E",

    "T_LOp_AND",
    "T_LOp_OR",
    "T_LOp_NOT",

    "T_Assign",
    "T_LP",
    "T_RP",
    "T_LC",
    "T_RC",
    "T_LB",
    "T_RB",
    "T_Semicolon",
    "T_Comma",
    "T_Id",
    "T_Decimal",
    "T_Hexadecimal",
    "T_String",
    "T_Character",
    "T_Comment",
    "T_Whitespace",

    "Invalid"
};
class Token {
    private:
        std::string content;
        token_type type;
        int line_number;

    public:
        Token (token_type _type, int _line_number, std::string _content = "") {
            content = _content;
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
            if (content == "") {
                return "< type: " + type_to_string[type] + ", line#: " + std::to_string(line_number) + " >";
            }
            return "< type: " + type_to_string[type] + ", line#: " + std::to_string(line_number) + ", content: " + content + " >";
        }
        friend std::ostream& operator<<(std::ostream &out, const Token &token) {
            return out << token.toString();
        }
};

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
