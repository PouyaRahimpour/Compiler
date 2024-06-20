#include "semantic_analyzer.h"

class SymbolTableEntry {
    private:
        // variable function array
        
    public:

};

class SemanticAnalyzer {
    private:
        Tree<Symbol> parse_tree;
        std::string out_address;
        std::map<std::string, SymbolTableEntry> symbol_table;

        void dfs(Node<Symbol>* node) {
            std::deque<Node<Symbol>*> children = node->get_children();
            Symbol symbol = node->get_data();
            for (auto child : children) {
                dfs(child);
            }
            std::string name = symbol.get_name();
            if (name == "int" || name == "decimal" || name == "hexadecimal") {
                symbol.set_stype(INT);
            }
            if (name == "character" || name == "char") {
                symbol.set_stype(CHAR);
            }
            if (name == "bool" || name == "false" || name == "true") {
                symbol.set_stype(BOOL);
            }
            node->set_date(symbol);
 

        }

    public:
        SemanticAnalyzer(Tree<Symbol> _parse_tree, std::string output_file_name) {
            parse_tree = _parse_tree;
            out_address = output_file_name;
        }
        // exp.type = int , bool
        void set_exp_types() {
        }
};