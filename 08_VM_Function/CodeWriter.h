#ifndef CODEWRITER_H
#define CODEWRITER_H

#include <string>
#include <fstream>
#include "CommandType.h"
#include <map>

class CodeWriter{
private:
    std::string raw_name;
    std::ofstream output;
    std::string current_function_name;
    std::map<std::string, std::string> segment_asm;
    std::map<std::string, std::string> jump_asm;
    int label_count;
    int ret_count;
    std::string get_full_label(std::string label);
    void push_D();
public:
    CodeWriter(std::string filename);
    void set_file_name(std::string vm_name);
    void write_pushpop(CommandType command, std::string segment, int index);
    void write_arithmetic(std::string command);
    void write_label(std::string label);
    void write_goto(std::string label);
    void write_if(std::string label);
    void write_function(std::string function_name, int num_of_vars);
    void write_call(std::string function_name, int num_of_args);
    void write_return();
    void close();
    void bootstrap();
};

#endif