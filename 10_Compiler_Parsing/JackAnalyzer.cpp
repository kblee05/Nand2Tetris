#include "JackAnalyzer.h"
#include "CompilationEngine.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

void JackAnalyzer::analyze(std::string filename){
    fs::path input_path(filename);
    fs::path output_path = input_path;
    output_path.replace_extension(".xml");

    std::ifstream input(input_path);
    std::ofstream output(output_path);

    if(!input.is_open() || !output.is_open()){
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }

    JackTokenizer tokenizer(input);
    CompilationEngine compilation_engine(output, tokenizer);

    //output << "<tokens>\n";

    if(tokenizer.advance())
        compilation_engine.compile_class();

    //output << "</tokens>\n";
}