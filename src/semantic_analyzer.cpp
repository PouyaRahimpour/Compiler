#include "semantic_analyzer.h"
#include "utils.h"

class SymbolTableEntry {
    private:
        id_type type;
        semantic_type stype;
        std::vector<std::pair<std::string, semantic_type>> parameters;
        int def_area;
        
    public:
        SymbolTableEntry() {
            type = NONE;
        }
        SymbolTableEntry(id_type _type, semantic_type _stype, int _def_area) {
            type = _type;
            stype = _stype;
            def_area = _def_area;
        }

        void set_type(id_type _type) {
            type = _type;
        }
        id_type get_type() {
            return type;
        }
        void set_stype(semantic_type _stype) {
            stype = _stype;
        }
        semantic_type get_stype() {
            return stype;
        }
        void add_to_parameters(std::pair<std::string, semantic_type> parameter) {
            parameters.push_back(parameter);
        }
        std::vector<std::pair<std::string, semantic_type>>& get_parameters() {
            return parameters;
        }
        void set_def_area(int _def_area) {
            def_area = _def_area;
        }
        int get_def_area() {
            return def_area;
        }

        void clear_parameters() {
            parameters.clear();
        }
};

// TODO show line number for errors
class SemanticAnalyzer {
    private:
        std::string out_address;
        Tree<Symbol> parse_tree;
        std::map<std::string, SymbolTableEntry> symbol_table;
        int def_area;
        std::string current_func;

        void dfs(Node<Symbol>* node) {
            std::deque<Node<Symbol>*> children = node->get_children();
            Symbol &symbol = node->get_data();
            std::string head_name = symbol.get_name();

            for (auto child : children) {
                std::string child_name = child->get_data().get_name();
                // Semantic rules for inherited attributes
                if (head_name == "dec") {
                    if (child_name == "id") {
                        // id.type = type.type
                        child->get_data().set_stype(children[0]->get_data().get_stype());
                    }
                    else if (child_name == "init_dec") {
                        // init_dec.type = type.type
                        child->get_data().set_stype(children[0]->get_data().get_stype());
                    }
                }
                else if (head_name == "init_dec") {
                    if (child_name == "var_dec_global") {
                        // var_dec_glabal.type = init_dec_type
                        child->get_data().set_stype(symbol.get_stype());

                        std::string id = node->get_parent()->get_children()[1]->get_content();
                        semantic_type stype = node->get_parent()->get_children()[1]->get_data().get_stype();
                        if (symbol_table[id].get_type() == NONE) {
                            symbol_table[id] = SymbolTableEntry(VAR, stype, def_area);
                        }
                        else {
                            int line_number = -1;

                            std::cerr << RED << "Semantic Error: Redefinition of variable '" + id + "', line: " << line_number << WHITE << std::endl;
                            std::cerr << "---------------------------------------------------------------" << std::endl;
                        }
                    }
                    else if (child_name == "func_dec") {
                        // func_dec.type = init_dec_type
                        child->get_data().set_stype(symbol.get_stype());

                        std::string id = node->get_parent()->get_children()[1]->get_content();
                        semantic_type stype = node->get_parent()->get_children()[1]->get_data().get_stype();
                        if (symbol_table[id].get_type() == NONE) {
                            symbol_table[id] = SymbolTableEntry(FUNC, stype, def_area);
                        }
                        else {
                            int line_number = -1;

                            std::cerr << RED << "Semantic Error: Redefinition of function '" + id + "', line: " << line_number << WHITE << std::endl;
                            std::cerr << "---------------------------------------------------------------" << std::endl;
                        }

                        current_func = id;
                    }
                }
                else if (head_name == "var_dec_global") {
                    if (child_name == "more") {
                        if (symbol.get_stype() != children[1]->get_data().get_stype() && children[1]->get_data().get_stype() != VOID) {
                            int line_number = -1;
                            std::string id = node->get_parent()->get_parent()->get_children()[1]->get_content();
                            std::string left_type = semantic_type_to_string[symbol.get_stype()];
                            std::string right_type = semantic_type_to_string[children[1]->get_data().get_stype()];

                            std::cerr << RED << "Semantic Error: Assigning types don't match for id '" + id + "', line: " << line_number << WHITE << std::endl;
                            std::cerr << RED << "id type is '" + left_type +  "', but assign value type is '" + right_type + "'." << WHITE << std::endl;
                            std::cerr << "---------------------------------------------------------------" << std::endl;
                        }

                        // more.type = var_dec_glabal.type
                        child->get_data().set_stype(symbol.get_stype());
                    }
                }
                else if (head_name == "more") {
                    if (child_name == "var_dec_list") {
                        // var_dec_list.type = more.type
                        child->get_data().set_stype(symbol.get_stype());
                    }
                }
                else if (head_name == "var_dec") {
                    if (child_name == "var_dec_list") {
                        // var_dec_list.type = type.type
                        child->get_data().set_stype(children[0]->get_data().get_stype());
                    }
                }
                else if (head_name == "var_dec_list") {
                    if (child_name == "var_dec_init") {
                        // var_dec_init.type = var_dec_list.type
                        child->get_data().set_stype(symbol.get_stype());
                    }
                    else if (child_name == "var_dec_list2") {
                        // var_dec_list2.type = var_dec_list.type
                        child->get_data().set_stype(symbol.get_stype());
                    }
                }
                else if (head_name == "var_dec_list2") {
                    if (child_name == "var_dec_init") {
                        // var_dec_init.type = var_dec_list2.type
                        child->get_data().set_stype(symbol.get_stype());
                    }
                    else if (child_name == "var_dec_list2") {
                        // var_dec_list2@.type = var_dec_list2.type
                        child->get_data().set_stype(symbol.get_stype());
                    }
                }
                else if (head_name == "var_dec_init") {
                    if (child_name == "var_id") {
                        // var_id.type = var_dec_init.type
                        child->get_data().set_stype(symbol.get_stype());
                    }
                }
                else if (head_name == "var_id") {
                    if (child_name == "id") {
                        // id.type = var_id.type
                        child->get_data().set_stype(symbol.get_stype());

                        std::string id = children[0]->get_content();
                        semantic_type stype = children[0]->get_data().get_stype();
                        if (symbol_table[id].get_type() == NONE) {
                            symbol_table[id] = SymbolTableEntry(VAR, stype, def_area);
                        }
                        else {
                            int line_number = -1;

                            std::cerr << RED << "Semantic Error: Redefinition of variable '" + id + "', line: " << line_number << WHITE << std::endl;
                            std::cerr << "---------------------------------------------------------------" << std::endl;
                        }
                    }
                }
                else if (head_name == "param") {
                    if (child_name == "param_id") {
                        // param_id.type = type.type
                        child->get_data().set_stype(children[0]->get_data().get_stype());
                    }
                }
                else if (head_name == "param_id") {
                    if (child_name == "id") {
                        // id.type = param_id.type
                        child->get_data().set_stype(symbol.get_stype());
                    }
                }
                else if (head_name == "func_call_or_id") {
                    if (child_name == "(") {
                        std::string id = node->get_parent()->get_children()[0]->get_content();
                        if (symbol_table[id].get_type() == NONE) {
                            int line_number = -1;

                            std::cerr << RED << "Semantic Error: Use undefined function '" + id + "', line: " << line_number << WHITE << std::endl;
                            std::cerr << "---------------------------------------------------------------" << std::endl;
                            symbol.set_stype(VOID);
                        }
                        else if (symbol_table[id].get_type() == VAR) {
                            int line_number = -1;

                            std::cerr << RED << "Semantic Error: Id '" + id + "' is variable but used instead a function , line: " << line_number << WHITE << std::endl;
                            std::cerr << "---------------------------------------------------------------" << std::endl;
                            symbol.set_stype(VOID);
                        }
                        else {
                            symbol.set_stype(symbol_table[id].get_stype());
                        }
                    }
                    else if (child_name == "ebracket") {
                        std::string id = node->get_parent()->get_children()[0]->get_content();
                        if (symbol_table[id].get_type() == NONE) {
                            int line_number = -1;

                            std::cerr << RED << "Semantic Error: Use undefined variable '" + id + "', line: " << line_number << WHITE << std::endl;
                            std::cerr << "---------------------------------------------------------------" << std::endl;
                            symbol.set_stype(VOID);
                        }
                        else if (symbol_table[id].get_type() == FUNC) {
                            int line_number = -1;

                            std::cerr << RED << "Semantic Error: Id '" + id + "' is function but used instead a variable , line: " << line_number << WHITE << std::endl;
                            std::cerr << "---------------------------------------------------------------" << std::endl;
                            symbol.set_stype(VOID);
                        }
                        else {
                            symbol.set_stype(symbol_table[id].get_stype());
                        }

                    }
                }

                dfs(child);
            }

            if (head_name == "{") {
                def_area++;
            }
            else if (head_name == "}") {
                // TODO copy symbol_table or not?
                for (auto &entry : symbol_table) {
                    if (entry.second.get_def_area() == def_area) {
                        symbol_table[entry.first].set_type(NONE); 
                        symbol_table[entry.first].clear_parameters();
                    }
                }

                def_area--;
                if (def_area == 0) {
                    current_func = "";
                }
            }

            // Semantic rules for synthesized attributes
            if (head_name == "dec") {
                // dec.type = type.type
                symbol.set_stype(children[0]->get_data().get_stype());
            }
            else if (head_name == "var_dec") {
                // dec.type = type.type
                symbol.set_stype(children[0]->get_data().get_stype());
            }
            else if (head_name == "var_dec_init") {
                if (children[0]->get_data().get_stype() != children[1]->get_data().get_stype() && children[1]->get_data().get_stype() != VOID) {
                    int line_number = -1;
                    std::string id = children[0]->get_children()[0]->get_content();
                    std::string left_type = semantic_type_to_string[children[0]->get_data().get_stype()];
                    std::string right_type = semantic_type_to_string[children[1]->get_data().get_stype()];

                    std::cerr << RED << "Semantic Error: Assigning types don't match for id '" + id + "', line: " << line_number << WHITE << std::endl;
                    std::cerr << RED << "id type is '" + left_type +  "', but assign value type is '" + right_type + "'." << WHITE << std::endl;
                    std::cerr << "---------------------------------------------------------------" << std::endl;
                }
            }
            else if (head_name == "sz") {
                if (children[0]->get_data().get_name() == "[") {
                    if (children[1]->get_data().get_stype() != VOID && children[1]->get_data().get_stype() != INT) {
                        int line_number = -1;
                        Node<Symbol>* tmp = node->get_parent();
                        std::string id;
                        while (true) {
                            if (tmp->get_data().get_name() == "dec") {
                                id = tmp->get_children()[1]->get_content();
                                break;
                            }
                            else if (tmp->get_data().get_name() == "var_dec_init") {
                                id = tmp->get_children()[0]->get_children()[0]->get_content();
                                break;
                            }
                            tmp = tmp->get_parent();
                        }
                        std::string size_type = semantic_type_to_string[children[1]->get_data().get_stype()];

                        std::cerr << RED << "Semantic Error: The array size must have an int type for array '" + id + "', line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "The array size type is '" + size_type +  "'." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;
                    }
                }
            }
            else if (head_name == "optexp") {
                if (children[0]->get_data().get_name() == "exp") {
                    // optexp.type = exp.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                else if (children[0]->get_data().get_name() == "eps") {
                    // optexp.type = void 
                    symbol.set_stype(VOID);
                }
            }
            else if (head_name == "initial") {
                if (children[0]->get_data().get_name() == "=") {
                    // initial.type = eexp.type
                    symbol.set_stype(children[1]->get_data().get_stype());
                }
                else if (children[0]->get_data().get_name() == "eps") {
                    // initial.type = void 
                    symbol.set_stype(VOID);
                }
            }
            else if (head_name == "eexp") {
                if (children[0]->get_data().get_name() == "exp1") {
                    // eexp.type = exp1.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                else if (children[0]->get_data().get_name() == "{") {
                    // eexp.type = exp_list.type 
                    symbol.set_stype(children[1]->get_data().get_stype());
                }
            }
            else if (head_name == "exp_list") {
                if (children[0]->get_data().get_name() == "exp1") {
                    if (children[0]->get_data().get_stype() == children[1]->get_data().get_stype() || children[1]->get_data().get_stype() == VOID) {
                        // exp_list.type = exp1.type
                        symbol.set_stype(children[0]->get_data().get_stype());
                    }
                    else {
                        int line_number = -1;
                        Node<Symbol>* tmp = node->get_parent();
                        std::string id;
                        while (true) {
                            if (tmp->get_data().get_name() == "dec") {
                                id = tmp->get_children()[1]->get_content();
                                break;
                            }
                            else if (tmp->get_data().get_name() == "var_dec_init") {
                                id = tmp->get_children()[0]->get_children()[0]->get_content();
                                break;
                            }
                            tmp = tmp->get_parent();
                        }
                        std::string member_type1 = semantic_type_to_string[children[0]->get_data().get_stype()];
                        std::string member_type2 = semantic_type_to_string[children[1]->get_data().get_stype()];

                        std::cerr << RED << "Semantic Error: All array memebers must have the same type for array '" + id + "', line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "'" + member_type1 +  "' and '" + member_type2 + "' types exist in array." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;

                        // exp_list.type = void
                        symbol.set_stype(VOID);
                    }
                }
                else if (children[0]->get_data().get_name() == "eps") {
                    // exp_list.type = void 
                    symbol.set_stype(VOID);
                }
            }
            else if (head_name == "exp_list2") {
                if (children[0]->get_data().get_name() == ",") {
                    if (children[1]->get_data().get_stype() == children[2]->get_data().get_stype() || children[2]->get_data().get_stype() == VOID) {
                        // exp_list2.type = exp1.type
                        symbol.set_stype(children[1]->get_data().get_stype());
                    }
                    else {
                        int line_number = -1;
                        Node<Symbol>* tmp = node->get_parent();
                        std::string id;
                        while (true) {
                            if (tmp->get_data().get_name() == "dec") {
                                id = tmp->get_children()[1]->get_content();
                                break;
                            }
                            else if (tmp->get_data().get_name() == "var_dec_init") {
                                id = tmp->get_children()[0]->get_children()[0]->get_content();
                                break;
                            }
                            tmp = tmp->get_parent();
                        }
                        std::string member_type1 = semantic_type_to_string[children[1]->get_data().get_stype()];
                        std::string member_type2 = semantic_type_to_string[children[2]->get_data().get_stype()];

                        std::cerr << RED << "Semantic Error: All array memebers must have the same type for array '" + id + "', line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "'" + member_type1 +  "' and '" + member_type2 + "' types exist in array." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;

                        // exp_list2.type = void
                        symbol.set_stype(VOID);
                    }
                }
                else if (children[0]->get_data().get_name() == "eps") {
                    // exp_list2.type = void 
                    symbol.set_stype(VOID);
                }
            }
            else if (head_name == "type") {
                if (children[0]->get_data().get_name() == "int") {
                    // type.type = int 
                    symbol.set_stype(INT);
                }
                else if (children[0]->get_data().get_name() == "bool") {
                    // type.type = bool 
                    symbol.set_stype(BOOL);
                }
                else if (children[0]->get_data().get_name() == "char") {
                    // type.type = char 
                    symbol.set_stype(CHAR);
                }
            }
            else if (head_name == "param") {
                // param.type = type.type
                symbol.set_stype(children[0]->get_data().get_stype());

                std::string id = children[1]->get_children()[0]->get_content();
                symbol_table[current_func].add_to_parameters({id, symbol.get_stype()});
            }
            else if (head_name == "stmt") {
                if (children[0]->get_data().get_name() == "return_stmt") {
                    if (children[0]->get_data().get_stype() != symbol_table[current_func].get_stype()) {
                        int line_number = -1;
                        std::string func_type = semantic_type_to_string[symbol_table[current_func].get_stype()];
                        std::string return_type = semantic_type_to_string[children[0]->get_data().get_stype()];

                        std::cerr << RED << "Semantic Error: Return stmt for function '" + current_func + "' have unmatched type, line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "function type is '" + func_type + "' but return type is '" + return_type + "'." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;
                    }
                }
            }
            else if (head_name == "if_stmt") {
                if (children[2]->get_data().get_stype() != VOID && children[2]->get_data().get_stype() != BOOL) {
                    int line_number = -1;
                    std::string condition_type = semantic_type_to_string[children[2]->get_data().get_stype()];

                    std::cerr << RED << "Semantic Error: The condition type of if stmt must have a bool type, line: " << line_number << WHITE << std::endl;
                    std::cerr << RED << "The condition type is '" + condition_type +  "'." << WHITE << std::endl;
                    std::cerr << "---------------------------------------------------------------" << std::endl;
                }
            }
            else if (head_name == "return_stmt") {
                // return_stmt.type = exp.type
                symbol.set_stype(children[1]->get_data().get_stype());
            }
            else if (head_name == "exp") {
                if (children[1]->get_data().get_stype() == VOID) {
                    // exp.type = exp1.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                else {
                    // exp.type = t1.type
                    symbol.set_stype(children[1]->get_data().get_stype());
                }
            }
            else if (head_name == "t1") {
                if (children[0]->get_data().get_name() == ",") {
                    if (children[2]->get_data().get_stype() == VOID) {
                        // t1.type = exp1.type
                        symbol.set_stype(children[1]->get_data().get_stype());
                    }
                    else {
                        // t1.type = t1@.type
                        symbol.set_stype(children[2]->get_data().get_stype());
                    }
                }
                else if (children[0]->get_data().get_name() == "eps") {
                    // t1.type = void
                    symbol.set_stype(VOID);
                }
            }
            else if (head_name == "exp1") {
                if (children[1]->get_data().get_stype() == VOID || children[0]->get_data().get_stype() == children[1]->get_data().get_stype()) {
                    // exp1.type = exp2.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                else {
                    int line_number = -1;
                    Node<Symbol>* tmp = node;
                    std::string id;
                    while (true) {
                        if (tmp->get_data().get_name() == "exp9") {
                            id = tmp->get_children()[0]->get_content();
                            break;
                        }
                        tmp = tmp->get_children()[0];
                    }
                    std::string left_type = semantic_type_to_string[children[0]->get_data().get_stype()];
                    std::string right_type = semantic_type_to_string[children[1]->get_data().get_stype()];

                    std::cerr << RED << "Semantic Error: Assigning types don't match for id '" + id + "', line: " << line_number << WHITE << std::endl;
                    std::cerr << RED << "id type is '" + left_type +  "', but assign value type is '" + right_type + "'." << WHITE << std::endl;
                    std::cerr << "---------------------------------------------------------------" << std::endl;

                    // exp1.type = exp2.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
            }
            else if (head_name == "t2") {
                if (children[0]->get_data().get_name() == "=") {
                    if (children[2]->get_data().get_stype() == VOID || children[1]->get_data().get_stype() == children[2]->get_data().get_stype()) {
                        // t2.type = exp2.type
                        symbol.set_stype(children[1]->get_data().get_stype());
                    }
                    else {
                        int line_number = -1;
                        Node<Symbol>* tmp = node->get_children()[1];
                        std::string id;
                        while (true) {
                            if (tmp->get_data().get_name() == "exp9") {
                                id = tmp->get_children()[0]->get_content();
                                break;
                            }
                            tmp = tmp->get_children()[0];
                        }
                        std::string left_type = semantic_type_to_string[children[1]->get_data().get_stype()];
                        std::string right_type = semantic_type_to_string[children[2]->get_data().get_stype()];

                        std::cerr << RED << "Semantic Error: Assigning types don't match for id '" + id + "', line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "id type is '" + left_type +  "', but assign value type is '" + right_type + "'." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;

                        // t2.type = exp2.type
                        symbol.set_stype(children[1]->get_data().get_stype());
                    }
                }
                else if (children[0]->get_data().get_name() == "eps") {
                    // t2.type = void
                    symbol.set_stype(VOID);
                }
            }
            else if (head_name == "exp2") {
                if (children[1]->get_data().get_stype() == VOID || (children[0]->get_data().get_stype() == BOOL && children[1]->get_data().get_stype() == BOOL)) {
                    // exp2.type = exp3.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                else {
                    int line_number = -1;
                    std::string left_type = semantic_type_to_string[children[0]->get_data().get_stype()];
                    std::string right_type = semantic_type_to_string[children[1]->get_data().get_stype()];

                    std::cerr << RED << "Semantic Error: Logical operator || takes 2 values with the bool type, line: " << line_number << WHITE << std::endl;
                    std::cerr << RED << "first value type is '" + left_type +  "', but second value type is '" + right_type + "'." << WHITE << std::endl;
                    std::cerr << "---------------------------------------------------------------" << std::endl;

                    // exp2.type = bool 
                    symbol.set_stype(BOOL);
                }
            }
            else if (head_name == "t3") {
                if (children[0]->get_data().get_name() == "||") {
                    if (children[2]->get_data().get_stype() == VOID || (children[1]->get_data().get_stype() == BOOL && children[2]->get_data().get_stype() == BOOL)) {
                        // t3.type = exp3.type
                        symbol.set_stype(children[1]->get_data().get_stype());
                    }
                    else {
                        int line_number = -1;
                        std::string left_type = semantic_type_to_string[children[1]->get_data().get_stype()];
                        std::string right_type = semantic_type_to_string[children[2]->get_data().get_stype()];

                        std::cerr << RED << "Semantic Error: Logical operator || takes 2 values with the bool type, line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "first value type is '" + left_type +  "', but second value type is '" + right_type + "'." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;

                        // t3.type = bool 
                        symbol.set_stype(BOOL);
                    }
                }
                else if (children[0]->get_data().get_name() == "eps") {
                    // t3.type = void
                    symbol.set_stype(VOID);
                }
            }
            else if (head_name == "exp3") {
                if (children[1]->get_data().get_stype() == VOID || (children[0]->get_data().get_stype() == BOOL && children[1]->get_data().get_stype() == BOOL)) {
                    // exp3.type = exp4.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                else {
                    int line_number = -1;
                    std::string left_type = semantic_type_to_string[children[0]->get_data().get_stype()];
                    std::string right_type = semantic_type_to_string[children[1]->get_data().get_stype()];

                    std::cerr << RED << "Semantic Error: Logical operator && takes 2 values with the bool type, line: " << line_number << WHITE << std::endl;
                    std::cerr << RED << "first value type is '" + left_type +  "', but second value type is '" + right_type + "'." << WHITE << std::endl;
                    std::cerr << "---------------------------------------------------------------" << std::endl;

                    // exp3.type = bool 
                    symbol.set_stype(BOOL);
                }
            }
            else if (head_name == "t4") {
                if (children[0]->get_data().get_name() == "&&") {
                    if (children[2]->get_data().get_stype() == VOID || (children[1]->get_data().get_stype() == BOOL && children[2]->get_data().get_stype() == BOOL)) {
                        // t4.type = exp4.type
                        symbol.set_stype(children[1]->get_data().get_stype());
                    }
                    else {
                        int line_number = -1;
                        std::string left_type = semantic_type_to_string[children[1]->get_data().get_stype()];
                        std::string right_type = semantic_type_to_string[children[2]->get_data().get_stype()];

                        std::cerr << RED << "Semantic Error: Logical operator && takes 2 values with the bool type, line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "first value type is '" + left_type +  "', but second value type is '" + right_type + "'." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;

                        // t4.type = bool 
                        symbol.set_stype(BOOL);
                    }
                }
                else if (children[0]->get_data().get_name() == "eps") {
                    // t4.type = void
                    symbol.set_stype(VOID);
                }
            }
            else if (head_name == "exp4") {
                if (children[1]->get_data().get_stype() == VOID || children[0]->get_data().get_stype() == children[1]->get_data().get_stype()) {
                    // exp4.type = exp5.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                else {
                    int line_number = -1;
                    std::string opt = children[1]->get_children()[0]->get_data().get_name();
                    std::string left_type = semantic_type_to_string[children[0]->get_data().get_stype()];
                    std::string right_type = semantic_type_to_string[children[1]->get_data().get_stype()];

                    std::cerr << RED << "Semantic Error: Logical operator " + opt + " takes 2 values with the same type, line: " << line_number << WHITE << std::endl;
                    std::cerr << RED << "first value type is '" + left_type +  "', but second value type is '" + right_type + "'." << WHITE << std::endl;
                    std::cerr << "---------------------------------------------------------------" << std::endl;

                    // exp4.type = bool 
                    symbol.set_stype(BOOL);
                }
            }
            else if (head_name == "t5") {
                if (children[0]->get_data().get_name() == "==" || children[0]->get_data().get_name() == "!=") {
                    if (children[2]->get_data().get_stype() == VOID || children[1]->get_data().get_stype() == children[2]->get_data().get_stype()) {
                        // t5.type = exp5.type
                        symbol.set_stype(children[1]->get_data().get_stype());
                    }
                    else {
                        int line_number = -1;
                        std::string opt = children[2]->get_children()[0]->get_data().get_name();
                        std::string left_type = semantic_type_to_string[children[1]->get_data().get_stype()];
                        std::string right_type = semantic_type_to_string[children[2]->get_data().get_stype()];

                        std::cerr << RED << "Semantic Error: Logical operator " + opt + " takes 2 values with the same type, line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "first value type is '" + left_type +  "', but second value type is '" + right_type + "'." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;

                        // t5.type = bool 
                        symbol.set_stype(BOOL);
                    }
                }
                else if (children[0]->get_data().get_name() == "eps") {
                    // t5.type = void
                    symbol.set_stype(VOID);
                }
            }
            else if (head_name == "exp5") {
                if (children[1]->get_data().get_stype() == VOID || children[0]->get_data().get_stype() == children[1]->get_data().get_stype()) {
                    // exp5.type = exp6.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                else {
                    int line_number = -1;
                    std::string opt = children[1]->get_children()[0]->get_data().get_name();
                    std::string left_type = semantic_type_to_string[children[0]->get_data().get_stype()];
                    std::string right_type = semantic_type_to_string[children[1]->get_data().get_stype()];

                    std::cerr << RED << "Semantic Error: Logical operator " + opt + " takes 2 values with the same type, line: " << line_number << WHITE << std::endl;
                    std::cerr << RED << "first value type is '" + left_type +  "', but second value type is '" + right_type + "'." << WHITE << std::endl;
                    std::cerr << "---------------------------------------------------------------" << std::endl;

                    // exp5.type = bool 
                    symbol.set_stype(BOOL);
                }
            }
            else if (head_name == "t6") {
                if (children[0]->get_data().get_name() == "<" || children[0]->get_data().get_name() == ">"
                    || children[0]->get_data().get_name() == "<=" || children[0]->get_data().get_name() == ">=") {
                    if (children[2]->get_data().get_stype() == VOID || children[1]->get_data().get_stype() == children[2]->get_data().get_stype()) {
                        // t6.type = exp6.type
                        symbol.set_stype(children[1]->get_data().get_stype());
                    }
                    else {
                        int line_number = -1;
                        std::string opt = children[2]->get_children()[0]->get_data().get_name();
                        std::string left_type = semantic_type_to_string[children[1]->get_data().get_stype()];
                        std::string right_type = semantic_type_to_string[children[2]->get_data().get_stype()];

                        std::cerr << RED << "Semantic Error: Logical operator " + opt + " takes 2 values with the same type, line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "first value type is '" + left_type +  "', but second value type is '" + right_type + "'." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;

                        // t6.type = bool 
                        symbol.set_stype(BOOL);
                    }
                }
                else if (children[0]->get_data().get_name() == "eps") {
                    // t6.type = void
                    symbol.set_stype(VOID);
                }
            }
            else if (head_name == "exp6") {
                if (children[1]->get_data().get_stype() == VOID || children[0]->get_data().get_stype() == children[1]->get_data().get_stype()) {
                    // exp6.type = exp7.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                else {
                    int line_number = -1;
                    std::string opt = children[1]->get_children()[0]->get_data().get_name();
                    std::string left_type = semantic_type_to_string[children[0]->get_data().get_stype()];
                    std::string right_type = semantic_type_to_string[children[1]->get_data().get_stype()];

                    std::cerr << RED << "Semantic Error: Computational operator " + opt + " takes 2 values with the int type, line: " << line_number << WHITE << std::endl;
                    std::cerr << RED << "first value type is '" + left_type +  "', but second value type is '" + right_type + "'." << WHITE << std::endl;
                    std::cerr << "---------------------------------------------------------------" << std::endl;

                    // exp6.type = int 
                    symbol.set_stype(INT);
                }
            }
            else if (head_name == "t7") {
                if (children[0]->get_data().get_name() == "+" || children[0]->get_data().get_name() == "-") {
                    if (children[2]->get_data().get_stype() == VOID || children[1]->get_data().get_stype() == children[2]->get_data().get_stype()) {
                        // t7.type = exp7.type
                        symbol.set_stype(children[1]->get_data().get_stype());
                    }
                    else {
                        int line_number = -1;
                        std::string opt = children[2]->get_children()[0]->get_data().get_name();
                        std::string left_type = semantic_type_to_string[children[1]->get_data().get_stype()];
                        std::string right_type = semantic_type_to_string[children[2]->get_data().get_stype()];

                        std::cerr << RED << "Semantic Error: Computational operator " + opt + " takes 2 values with the int type, line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "first value type is '" + left_type +  "', but second value type is '" + right_type + "'." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;

                        // t7.type = int 
                        symbol.set_stype(INT);
                    }
                }
                else if (children[0]->get_data().get_name() == "eps") {
                    // t7.type = void
                    symbol.set_stype(VOID);
                }
            }
            else if (head_name == "exp7") {
                if (children[1]->get_data().get_stype() == VOID || children[0]->get_data().get_stype() == children[1]->get_data().get_stype()) {
                    // exp7.type = exp8.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                else {
                    int line_number = -1;
                    std::string opt = children[1]->get_children()[0]->get_data().get_name();
                    std::string left_type = semantic_type_to_string[children[0]->get_data().get_stype()];
                    std::string right_type = semantic_type_to_string[children[1]->get_data().get_stype()];

                    std::cerr << RED << "Semantic Error: Computational operator " + opt + " takes 2 values with the int type, line: " << line_number << WHITE << std::endl;
                    std::cerr << RED << "first value type is '" + left_type +  "', but second value type is '" + right_type + "'." << WHITE << std::endl;
                    std::cerr << "---------------------------------------------------------------" << std::endl;

                    // exp7.type = int 
                    symbol.set_stype(INT);
                }
            }
            else if (head_name == "t8") {
                if (children[0]->get_data().get_name() == "*" || children[0]->get_data().get_name() == "/"
                    || children[0]->get_data().get_name() == "%") {
                    if (children[2]->get_data().get_stype() == VOID || children[1]->get_data().get_stype() == children[2]->get_data().get_stype()) {
                        // t8.type = exp8.type
                        symbol.set_stype(children[1]->get_data().get_stype());
                    }
                    else {
                        int line_number = -1;
                        std::string opt = children[2]->get_children()[0]->get_data().get_name();
                        std::string left_type = semantic_type_to_string[children[1]->get_data().get_stype()];
                        std::string right_type = semantic_type_to_string[children[2]->get_data().get_stype()];

                        std::cerr << RED << "Semantic Error: Computational operator " + opt + " takes 2 values with the int type, line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "first value type is '" + left_type +  "', but second value type is '" + right_type + "'." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;

                        // t8.type = int 
                        symbol.set_stype(INT);
                    }
                }
                else if (children[0]->get_data().get_name() == "eps") {
                    // t8.type = void
                    symbol.set_stype(VOID);
                }
            }
            else if (head_name == "exp8") {
                if (children[0]->get_data().get_name() == "+" || children[0]->get_data().get_name() == "-") {
                    if (children[1]->get_data().get_stype() == INT) {
                        // exp8.type = int 
                        symbol.set_stype(INT);
                    }
                    else {
                        int line_number = -1;
                        std::string opt = children[0]->get_data().get_name();
                        std::string right_type = semantic_type_to_string[children[1]->get_data().get_stype()];

                        std::cerr << RED << "Semantic Error: Computational operator " + opt + " takes 1 value with the int type, line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "value type is '" + right_type + "." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;

                        // exp8.type = int 
                        symbol.set_stype(INT);
                    }
                }
                else if (children[0]->get_data().get_name() == "!") {
                    if (children[1]->get_data().get_stype() == BOOL) {
                        // exp8.type = bool 
                        symbol.set_stype(BOOL);
                    }
                    else {
                        int line_number = -1;
                        std::string opt = children[0]->get_data().get_name();
                        std::string right_type = semantic_type_to_string[children[1]->get_data().get_stype()];

                        std::cerr << RED << "Semantic Error: Computational operator " + opt + " takes 1 value with the bool type, line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "value type is '" + right_type + "." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;

                        // exp8.type = bool 
                        symbol.set_stype(BOOL);
                    }

                }
                else if (children[0]->get_data().get_name() == "exp9") {
                    // exp8.type = exp9.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
            }
            else if (head_name == "exp9") {
                if (children[0]->get_data().get_name() == "(") {
                    // exp9.type = exp.type
                    symbol.set_stype(children[1]->get_data().get_stype());
                }
                else if (children[0]->get_data().get_name() == "id") {
                    // id.type = func_call_or_id.type
                    children[0]->get_data().set_stype(children[1]->get_data().get_stype());

                    // exp9.type = id.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                else if (children[0]->get_data().get_name() == "constant") {
                    // exp9.type = constant.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
            }
            else if (head_name == "func_call_or_id") {
                if (node->get_children()[0]->get_data().get_name() == "(") {
                    std::string id = node->get_parent()->get_children()[0]->get_content();
                    std::vector<std::pair<std::string, semantic_type>> &define_params = symbol_table[id].get_parameters();
                    int len_define_params = define_params.size();
                    std::vector<semantic_type> &used_params = children[1]->get_data().get_params_type();
                    int len_used_params = used_params.size();

                    int line_number = -1;
                    if (len_define_params > len_used_params) {
                        std::cerr << RED << "Semantic Error: Too few arguments for function '" + id + "', line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "Expected " + std::to_string(len_define_params) + " arguments but found " + std::to_string(len_used_params) + " arguments." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;
                    }
                    else if (len_define_params < len_used_params) {
                        std::cerr << RED << "Semantic Error: Too many arguments for function '" + id + "', line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "Expected " + std::to_string(len_define_params) + " arguments but found " + std::to_string(len_used_params) + " arguments." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;
                    }
                    else {
                        for (int i = 0; i < len_define_params; i++) {
                            if (define_params[i].second != used_params[i]) {
                                std::cerr << RED << "Semantic Error: Unmatched type for " + std::to_string(i + 1) + "th argument '" + define_params[i].first +  "' of function '" + id + "', line: " << line_number << WHITE << std::endl;
                                std::cerr << RED << "Expected '" + semantic_type_to_string[define_params[i].second] + "' type but found '" + semantic_type_to_string[used_params[i]] + "' type." << WHITE << std::endl;
                                std::cerr << "---------------------------------------------------------------" << std::endl;
                            }
                        }
                    }
                }
            }
            else if (head_name == "params_call") {
                if (node->get_children()[0]->get_data().get_name() == "exp1") {
                    symbol.add_to_params_type(children[0]->get_data().get_stype()); 
                    symbol.add_to_params_type(children[1]->get_data().get_params_type()); 
                }
            }
            else if (head_name == "params_call2") {
                if (children[0]->get_data().get_name() == ",") {
                    symbol.add_to_params_type(children[1]->get_data().get_stype()); 
                    symbol.add_to_params_type(children[2]->get_data().get_params_type()); 
                }
            }
            else if (head_name == "ebracket") {
                if (children[0]->get_data().get_name() == "[") {
                    if (children[1]->get_data().get_stype() != INT) {
                        int line_number = -1;
                        std::string id = node->get_parent()->get_parent()->get_children()[0]->get_content();
                        std::string index_type = semantic_type_to_string[children[1]->get_data().get_stype()];

                        std::cerr << RED << "Semantic Error: The array index must have an int type for array '" + id + "', line: " << line_number << WHITE << std::endl;
                        std::cerr << RED << "The array index type is '" + index_type +  "'." << WHITE << std::endl;
                        std::cerr << "---------------------------------------------------------------" << std::endl;
                    }
                }
            }
            else if (head_name == "constant") {
                if (children[0]->get_data().get_name() == "decimal") {
                    // constant.type = int
                    symbol.set_stype(INT);
                }
                else if (children[0]->get_data().get_name() == "hexadecimal") {
                    // constant.type = int
                    symbol.set_stype(INT);
                }
                else if (children[0]->get_data().get_name() == "string") {
                    // constant.type = char 
                    symbol.set_stype(CHAR);
                }
                else if (children[0]->get_data().get_name() == "character") {
                    // constant.type = char 
                    symbol.set_stype(CHAR);
                }
                else if (children[0]->get_data().get_name() == "false") {
                    // constant.type = bool 
                    symbol.set_stype(BOOL);
                }
                else if (children[0]->get_data().get_name() == "true") {
                    // constant.type = bool 
                    symbol.set_stype(BOOL);
                }
            }
        }

    public:
        SemanticAnalyzer(Tree<Symbol> _parse_tree, std::string output_file_name) {
            parse_tree = _parse_tree;
            out_address = output_file_name;
            def_area = 0;
            current_func = "";
            set_exp_types();
        }

        void set_exp_types() {
            dfs(parse_tree.get_root());
            if (symbol_table["main"].get_type() != FUNC) {
                std::cerr << RED << "Semantic Error: There is no function called 'main' in code." << WHITE << std::endl;
                std::cerr << "---------------------------------------------------------------" << std::endl;
            }
            else if (symbol_table["main"].get_stype() != INT) {
                std::string main_type = semantic_type_to_string[symbol_table["main"].get_stype()];
                std::cerr << RED << "Semantic Error: The type of main function is invalid." << WHITE << std::endl;
                std::cerr << RED << "Expected 'int' type but found '" + main_type + "' type." << WHITE << std::endl;
                std::cerr << "---------------------------------------------------------------" << std::endl;
            }
        }
};
