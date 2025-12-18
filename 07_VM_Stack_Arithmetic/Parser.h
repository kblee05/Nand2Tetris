#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <fstream>
#include <sstream>
#include "CommandType.h"

class Parser{
private:
    std::string filename;
    std::ifstream input;
    std::string current_command;

    std::string clean_line(std::string line);
    void parse(std::string cleaned_line);
public:
    std::string arg0;
    std::string arg1;
    int arg2;
    
    Parser(std::string filename);
    bool advance();
    CommandType get_command_type();
};

#endif