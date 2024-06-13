#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <deque>
#include <set>
#include <map>
#include <algorithm>

#pragma once

#define SUCCESS 0
#define FAILIURE 1
#define FILE_ERROR 2
#define GRAMMAR_ERROR 3

#define TAB 4

#define COLORED_ERRORS true

const std::string WHITE = COLORED_ERRORS ? "\033[0;m" : "";
const std::string RED = COLORED_ERRORS ? "\033[0;31m" : "";

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

    Invalid,
    Eof
};

const std::string type_to_string[] = {
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

    "Invalid",
    "Eof"
};

class Token {
    private:
        token_type type;
        int line_number;
        std::string content;

    public:
        Token(token_type _type, int _line_number = -1, std::string _content = "") {
            type = _type;
            line_number = _line_number;
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
        std::string get_content() {
            return content;
        }
        void set_content(std::string _content) {
            content = _content;
        }

        std::string toString() const {
            if (content == "") {
                return "< type: " + type_to_string[type] + ", line: " + std::to_string(line_number) + " >";
            }
            return "< type: " + type_to_string[type] + ", line: " + std::to_string(line_number) + ", content: " + content + " >";
        }
        friend std::ostream& operator << (std::ostream &out, const Token &token) {
            return out << token.toString();
        }
};

template <typename T>
class Node {
    private:
        T data; 
        Node<T>* parent;
        std::deque<Node<T>*> children;
        std::string content;

    public:
        Node() {
            data = T();
            parent = nullptr;
            content = "";
        }
        Node(T _data, Node<T>* _parent = nullptr) {
            data = _data;
            parent = _parent;
            content = "";
        }

        void set_date(T _data) {
            data = _data;
        }
        T get_data() {
            return data;
        }
        void set_parent(Node<T>* _parent) {
            parent = _parent; 
        }
        Node<T>* get_parent() {
            return parent;
        }
        void push_back_children(Node<T>* node) {
            children.push_back(node);
        }
        void push_front_children(Node<T>* node) {
            children.push_front(node);
        }
        std::deque<Node<T>*> get_children() {
            return children;
        }
        void set_content(std::string _content) {
            content = _content;
        }
        std::string get_content() {
            return content;
        }
};

template <typename T>
class Tree {
    private:
        Node<T>* root;
        bool has_par[200];

    public:
        Tree() {
            root = nullptr;
            std::fill(has_par, has_par + 200, false);
        }
        Tree(Node<T>* _root) {
            root = _root;
            std::fill(has_par, has_par + 200, false);
        }

        void set_root(Node<T>* _root) {
            root = _root;
        }
        Node<T>* get_root() {
            return root;
        }

        void print_tree(Node<T>* node, int num = 0, bool last = false) {
            T var = node->get_data();

            for (int i = 0; i < num * TAB - TAB; i++) {
                if (has_par[i]) {
                    std::cout << "│";
                }
                else {
                    std::cout << " ";
                }
            }
            if (num) {
                if (last) {
                    std::cout << "└── ";
                }
                else {
                    std::cout << "├── ";
                }
            }
            std::cout << var << std::endl;
            if (node->get_content() != "") {
                for (int i = 0; i < num * TAB; i++) {
                    if (has_par[i]) {
                        std::cout << "│";
                    }
                    else {
                        std::cout << " ";
                    }
                }
                std::cout << "└── '" << node->get_content() << "'" << std::endl;
            }

            has_par[num * TAB] = true;
            std::deque<Node<T>*> children = node->get_children();
            for (auto child : children) {
                bool end = false;
                if (child == children.back()) {
                    has_par[num * TAB] = false;
                    end = true;
                }
                print_tree(child, num + 1, end);
            }
        }
};
