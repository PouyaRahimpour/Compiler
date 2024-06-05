#include"main.h"
 
int main(int argc, char* argv[]) {
    std::string input_file_name = argv[1];
    std::string output_file_name = argv[2];

    LexicalAnalyzer lex_analyzer(input_file_name, output_file_name);
    lex_analyzer.tokenize();

    SyntaxAnalyzer syn_analyzer(lex_analyzer.get_tokens(), output_file_name);
    // TODO
    if (1) {
        syn_analyzer.update_grammar();
    }
    syn_analyzer.make_tree();


    return 0;
}
