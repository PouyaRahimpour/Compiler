#include "main.h"
 
int main(int argc, char* argv[]) {
    std::string input_file_name = argv[1];
    std::string output_file_name = argv[2];
    std::string option1 = DEFAULT_OPTION, option2 = "";
    if (argc > 3) {
        option1 = argv[3];
    }
    if (argc > 4) {
        option1 = argv[4];
    }

    bool update_grammar = true;
    if (option2 == "-n") {
        update_grammar = false;
    }

    LexicalAnalyzer lex_analyzer(input_file_name, output_file_name);
    lex_analyzer.tokenize();
    if (option1 == "-lex") {
        lex_analyzer.write();
        return SUCCESS;
    }

    SyntaxAnalyzer syn_analyzer(lex_analyzer.get_tokens(), output_file_name);
    syn_analyzer.make_tree(update_grammar);
    if (option1 == "-syn") {
        syn_analyzer.write();
        return SUCCESS;
    }
    
    SemanticAnalyzer sem_analyzer(syn_analyzer.get_tree(), output_file_name);
    sem_analyzer.analyse();
    if (option1 == "-sem") {
        return SUCCESS;
    }

    if (option1 == "-run") {
        sem_analyzer.run_code();
    }

    return SUCCESS;
}
