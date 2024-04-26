#include<fstream>
#include<iostream>
#include<vector>
#include<string>

#define SUCCESS 0
#define FAILIURE 1
#define FILE_ERROR 2

#define NUM_KEYWORDS 12

char spaces[] = {9, 10, 32};

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

std::string type_to_string[] {
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


