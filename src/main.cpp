#include "main.h"
 
int main(int argc, char* argv[]) {
    // TODO get options for lexical or syntax or semantic analyzer
    std::string input_file_name = argv[1];
    std::string output_file_name = argv[2];
    std::string option1 = "", option2 = "";
    if (argc > 3) {
        option1 = argv[3];
    }
    if (argc > 4) {
        option1 = argv[4];
    }

    bool update_grammar = true;
    if (option1 == "-n" || option2 == "-n") {
        update_grammar = false;
    }

    LexicalAnalyzer lex_analyzer(input_file_name, output_file_name);
    lex_analyzer.tokenize();
    // lex_analyzer.write();

    SyntaxAnalyzer syn_analyzer(lex_analyzer.get_tokens(), output_file_name);
    syn_analyzer.make_tree(update_grammar);
    syn_analyzer.write();
    
    // SemanticAnalyzer
    SemanticAnalyzer sem_analyzer(syn_analyzer.get_tree(), output_file_name);

    return 0;
}
