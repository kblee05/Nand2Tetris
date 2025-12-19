#include "JackAnalyzer.h"

// ===============================private==============================



// ===============================public===============================

void JackAnalyzer::set_file(std::string filename){
    input.open(filename);
    size_t end_pos = filename.find(".");
    std::string raw_name = filename.substr(0, end_pos - 1);
}

void JackAnalyzer::analyze(){
    JackTokenizer tokenizer(input);
    CompilationEngine compilation_engine(output, tokenizer);

    output << "<tokens>\n";

    while(tokenizer.advance()){
        switch(tokenizer.token_type){
            
        }
    }

    input.close();
    output.close();
}