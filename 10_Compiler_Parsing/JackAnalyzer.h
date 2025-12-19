#ifndef JACKANALYZER_H
#define JACKANALYZER_H

#include "JackTokenizer.h"
#include "CompilationEngine.h"
#include <string>
#include <fstream>

class JackAnalyzer{
private:
    std::ifstream input;
    std::ofstream output;
public:
    void set_file(std::string filename);
    void analyze();
};

#endif