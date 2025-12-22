#include "JackCompiler.h"
#include "CompilationEngine.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

void JackCompiler::compile(std::string filename){
    fs::path input_path(filename);
    fs::path output_path = input_path;
    output_path.replace_extension(".vm");

    std::ifstream input(input_path);
    std::ofstream output(output_path);

    if(!input.is_open() || !output.is_open()){
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }

    JackTokenizer tokenizer(input);
    CompilationEngine compilation_engine(output, tokenizer);
    
    if(tokenizer.advance()){
        compilation_engine.compile_class();
    }
}