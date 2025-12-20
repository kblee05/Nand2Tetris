#ifndef COMPILATIONENGINE_H
#define COMPILATIONENGINE_H

#include <fstream>
#include "JackTokenizer.h"

class CompilationEngine{
private:
    JackTokenizer& tokenizer;
    std::ofstream& output;
    int identation_level;
    void eat(std::string expected_token);
    void eat_identifier();
    void print_xml(std::string tag, std::string body);
    void print_structure(std::string tag, bool is_start);
    void compile_type();
public:
    CompilationEngine(std::ofstream& ofstream, JackTokenizer& tokenizer);
    void compile_class();
    void compile_class_var_dec();
    void compile_subroutine();
    void compile_parameter_list();
    void compile_subroutine_body();
    void compile_var_dec();
    void compile_statements();
    void compile_let();
    void compile_if();
    void compile_while();
    void compile_do();
    void compile_return();
    void compile_expression();
    void compile_term();
    void compile_expression_list();
};

#endif