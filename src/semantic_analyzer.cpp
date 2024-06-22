#include "semantic_analyzer.h"
#include "utils.h"

// TODO shaw line number for errors
class SymbolTableEntry {
    private:
        // variable function array
        
    public:

};

class SemanticAnalyzer {
    private:
        std::string out_address;
        Tree<Symbol> parse_tree;
        std::map<std::string, SymbolTableEntry> symbol_table;

        void dfs(Node<Symbol>* node) {
            std::deque<Node<Symbol>*> children = node->get_children();
            Symbol &symbol = node->get_data();
            std::string head_name = symbol.get_name();

            for (auto child : children) {
                std::string child_name = child->get_data().get_name();
                // Semantic rules for inherited attribute
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
                    }
                    else if (child_name == "func_dec") {
                        // func_dec.type = init_dec_type
                        child->get_data().set_stype(symbol.get_stype());
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


                dfs(child);
            }

            // Semantic rules for synthesizeⅾ attribute
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
            else if (head_name == "param_type") {
                // param.type = type.type
                symbol.set_stype(children[0]->get_data().get_stype());
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
                    // TODO this is wrong
                    // exp9.type = symbol_tabel[id].type
                    symbol.set_stype(INT);
                }
                else if (children[0]->get_data().get_name() == "constant") {
                    // exp9.type = constant.type
                    symbol.set_stype(children[0]->get_data().get_stype());
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
            set_exp_types();
        }
        // exp.type = int , bool
        void set_exp_types() {
            dfs(parse_tree.get_root());
        }
};
