#include "main.h"
 
int main(int argc, char* argv[]) {
    std::string input_file_name = argv[1];
    std::string output_file_name = argv[2];
    // TODO get options for lexical or syntax or semantic analyzer

    LexicalAnalyzer lex_analyzer(input_file_name, output_file_name);
    lex_analyzer.tokenize();
    // lex_analyzer.write();

    // TODO add option for update grammar
    SyntaxAnalyzer syn_analyzer(lex_analyzer.get_tokens(), output_file_name);
    if (1) {
        syn_analyzer.update_grammar();
    }
    // syn_analyzer.make_tree();
    // syn_naalyzer.write();
    
    // SemanticAnalyzer

    return 0;
}
