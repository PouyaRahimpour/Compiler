#include"main.h"
 
int main(int argc, char* argv[]) {
    std::string input_file_name = argv[1];
    std::string output_file_name = argv[2];
    LexicalAnalyzer analyzer(input_file_name, output_file_name);
    analyzer.start();

    return 0;
}
