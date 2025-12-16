#ifndef CODEWRITER_H
#define CODEWRITER_H

#include <string>
#include <fstream>
#include "CommandType.h"
#include <map>

class CodeWriter{
private:
    std::string filename;
    std::string raw_name;
    std::ofstream output;
    std::map<std::string, std::string> segment_asm;
    std::map<std::string, std::string> jump_asm;
    int label_count;
public:
    CodeWriter(std::string filename);
    void write_pushpop(CommandType command, std::string segment, int index);
    void write_arithmetic(std::string command);
};

#endif