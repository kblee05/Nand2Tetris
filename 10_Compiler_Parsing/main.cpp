#include <iostream>
#include "JackAnalyzer.h"
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char* argv[]){
    
    std::string input_arg = argv[1];
    std::vector<std::string> jack_files;

    fs::path input_path(input_arg);

    if(fs::is_directory(input_path))
        for(const auto& entry : fs::directory_iterator(input_path))
            if(entry.is_regular_file() && entry.path().extension() == ".jack")
                jack_files.push_back(entry.path().string());
    else if(fs::exists(input_path))
        if(input_path.extension() == ".jack")
            jack_files.push_back(input_path.string());

    JackAnalyzer analyzer;
    
    for(const std::string& source_file : jack_files){
        try{
            std::cout << "Analyzing " << source_file << "\n";
            analyzer.analyze(source_file);
        }catch(const std::exception& e){
            std::cerr << e.what();
            return -1;
        }
    }

    return 0;
}