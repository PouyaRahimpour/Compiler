#include "semantic_analyzer.h"
#include "utils.h"

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
            else if (head_name == "optexp") {
                if (children[0]->get_data().get_name() == "exp") {
                    // optexp.type = exp.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                // TODO name == eps ?
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
                if (children[0]->get_data().get_name() == "exp") {
                    // eexp.type = exp.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                else if (children[0]->get_data().get_name() == "{") {
                    // eexp.type = exp_list.type 
                    symbol.set_stype(children[1]->get_data().get_stype());
                }
            }
            else if (head_name == "exp_list") {
                if (children[0]->get_data().get_name() == "exp") {
                    if (children[0]->get_data().get_stype() == children[1]->get_data().get_stype() || children[1]->get_data().get_stype() == VOID) {
                        // exp_list.type = exp.type
                        symbol.set_stype(children[0]->get_data().get_stype());
                    }
                    else {
                        // exp_list.type = void
                        symbol.set_stype(VOID);
                    }
                }
                else if (children[0]->get_data().get_name() == "eps") {
                    // TODO Error
                    // exp_list.type = void 
                    symbol.set_stype(VOID);
                }
            }
            else if (head_name == "exp_list2") {
                if (children[0]->get_data().get_name() == ",") {
                    if (children[1]->get_data().get_stype() == children[2]->get_data().get_stype() || children[2]->get_data().get_stype() == VOID) {
                        // exp_list2.type = exp.type
                        symbol.set_stype(children[0]->get_data().get_stype());
                    }
                    else {
                        // TODO Error
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
                // if_stmt.type = exp.type
                symbol.set_stype(children[2]->get_data().get_stype());
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
                    // TODO Error
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
                        // TODO Error
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
                if (children[1]->get_data().get_stype() == VOID || children[0]->get_data().get_stype() == children[1]->get_data().get_stype()) {
                    // exp2.type = exp3.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                else {
                    // TODO Error
                    // exp2.type = bool 
                    symbol.set_stype(BOOL);
                }
            }
            else if (head_name == "t3") {
                if (children[0]->get_data().get_name() == "||") {
                    if (children[2]->get_data().get_stype() == VOID || children[1]->get_data().get_stype() == children[2]->get_data().get_stype()) {
                        // t3.type = exp3.type
                        symbol.set_stype(children[1]->get_data().get_stype());
                    }
                    else {
                        // TODO Error
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
                if (children[1]->get_data().get_stype() == VOID || children[0]->get_data().get_stype() == children[1]->get_data().get_stype()) {
                    // exp3.type = exp4.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                else {
                    // TODO Error
                    // exp3.type = bool 
                    symbol.set_stype(BOOL);
                }
            }
            else if (head_name == "t4") {
                if (children[0]->get_data().get_name() == "&&") {
                    if (children[2]->get_data().get_stype() == VOID || children[1]->get_data().get_stype() == children[2]->get_data().get_stype()) {
                        // t4.type = exp4.type
                        symbol.set_stype(children[1]->get_data().get_stype());
                    }
                    else {
                        // TODO Error
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
                    // TODO Error
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
                        // TODO Error
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
                    // TODO Error
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
                        // TODO Error
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
                    // TODO Error
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
                        // TODO Error
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
                    // TODO Error
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
                        // TODO Error
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
                if (children[0]->get_data().get_name() == "exp9") {
                    // exp8.type = exp9.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                else {
                    if (children[1]->get_data().get_stype() == INT) {
                        // exp8.type = int 
                        symbol.set_stype(INT);
                    }
                    else {
                        // TODO Error
                        // exp8.type = int 
                        symbol.set_stype(INT);
                    }
                }
            }
            else if (head_name == "exp9") {
                if (children[0]->get_data().get_name() == "(") {
                    // exp9.type = exp.type
                    symbol.set_stype(children[1]->get_data().get_stype());
                }
                else if (children[0]->get_data().get_name() == "id") {
                    // exp9.type = id.type
                    symbol.set_stype(children[0]->get_data().get_stype());
                }
                else if (children[0]->get_data().get_name() == "constant") {
                    // exp9.type = constant.type
                    symbol.set_stype(children[0]->get_data().get_stype());
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
