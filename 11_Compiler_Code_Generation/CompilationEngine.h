#ifndef COMPILATIONENGINE_H
#define COMPILATIONENGINE_H

#include <fstream>
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"
#include <vector>
#include <iostream>

class CompilationEngine{
private:
    JackTokenizer& tokenizer;
    SymbolTable symbol_table;
    VMWriter vmwriter;
    int identation_level;
    std::string current_class_name;
    std::string current_subroutine_name;
    std::string current_subroutine_type;
    int label_count;
    void eat(std::string expected_token);
    std::string eat_identifier();
    std::string eat_type();
    VMWriter::Segment kind_to_segment(SymbolTable::Kind kind);
    std::vector<std::string> call_stack;
    struct TraceGuard{
        CompilationEngine& engine;
        
        TraceGuard(CompilationEngine& e, std::string function_name) : engine(e) {
            engine.call_stack.push_back(function_name);
        }

        ~TraceGuard(){
            engine.call_stack.pop_back();
        }
    };
    void error(std::string message);
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
    int compile_expression_list();
    void print_stack_trace();
};

#endif