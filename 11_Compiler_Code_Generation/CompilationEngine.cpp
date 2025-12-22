#include "CompilationEngine.h"
#include <iostream>

// ===============================private==============================

void CompilationEngine::eat(std::string expected_token){
    if(tokenizer.current_token == expected_token){
        tokenizer.advance();
        return;
    }
    throw std::runtime_error("Current token: " + tokenizer.current_token + " does not match " + expected_token + "\n");
}

std::string CompilationEngine::eat_identifier(){
    if(tokenizer.token_type == IDENTIFIER){
        std::string token = tokenizer.current_token;
        tokenizer.advance();
        return token;
    }
    throw std::runtime_error("Current token: " + tokenizer.current_token + " is not an IDENTIFIER\n");
}

std::string CompilationEngine::eat_type(){
    if(tokenizer.current_token == "int" ||
       tokenizer.current_token == "char" ||
       tokenizer.current_token == "boolean")
    {
        std::string token = tokenizer.current_token;
        tokenizer.advance();
        return token;
    }
    else{
        return eat_identifier();
    }
}

VMWriter::Segment CompilationEngine::kind_to_segment(SymbolTable::Kind kind){
    switch (kind){
    case SymbolTable::Kind::VAR:
        return VMWriter::Segment::LOCAL;
    case SymbolTable::Kind::FIELD:
        return VMWriter::Segment::THIS;
    case SymbolTable::Kind::ARG:
        return VMWriter::Segment::ARG;
    case SymbolTable::Kind::STATIC:
        return VMWriter::Segment::STATIC;
    default:
        throw std::runtime_error("Unkown kind of mapping ( SymbolTable kind to VMWriter)");
    }
}

// ===============================public===============================

CompilationEngine::CompilationEngine(std::ofstream& ofstream, JackTokenizer& tokenizer) : 
    identation_level(0),
    tokenizer(tokenizer),
    symbol_table(),
    vmwriter(ofstream),
    label_count(0) {
}

void CompilationEngine::compile_class(){
    eat("class");

    current_class_name = eat_identifier();

    eat("{");

    while(tokenizer.current_token == "field" || tokenizer.current_token == "static")
        compile_class_var_dec();
    
    while(tokenizer.current_token == "constructor" ||
          tokenizer.current_token == "function" ||
          tokenizer.current_token == "method")
        compile_subroutine();

    eat("}");
}

void CompilationEngine::compile_class_var_dec(){
    SymbolTable::Kind kind = tokenizer.current_token == "field" ? SymbolTable::Kind::FIELD : SymbolTable::Kind::STATIC;
    tokenizer.advance(); // eat field or static
    std::string type = eat_type();
    std::string name = eat_identifier();
    symbol_table.define(name, type, kind);

    while(tokenizer.current_token == ","){
        eat(",");
        name = eat_identifier();
        symbol_table.define(name, type, kind);
    }

    eat(";");
}

void CompilationEngine::compile_subroutine(){
    symbol_table.reset();

    current_subroutine_type = tokenizer.current_token;
    tokenizer.advance(); // eat constructor function method

    if(tokenizer.current_token == "void"){
        tokenizer.advance();
    }
    else    
        eat_type();

    current_subroutine_name = current_class_name + "." + eat_identifier();
    
    eat("(");

    compile_parameter_list();

    eat(")");
    
    compile_subroutine_body();
}

void CompilationEngine::compile_parameter_list(){
    std::string name, type;

    if(tokenizer.current_token != ")"){
        type = eat_type();
        name = eat_identifier();
        symbol_table.define(name, type, SymbolTable::Kind::ARG);
    }

    while(tokenizer.current_token == ","){
        eat(",");

        type = eat_type();
        name = eat_identifier();
        symbol_table.define(name, type, SymbolTable::Kind::ARG);
    }
}

void CompilationEngine::compile_subroutine_body(){
    eat("{");

    while(tokenizer.current_token == "var")
        compile_var_dec();
    
    int num_of_locals = symbol_table.get_var_count(SymbolTable::Kind::VAR);
    vmwriter.write_function(current_subroutine_name, num_of_locals);

    if(current_subroutine_type == "constructor"){
        int num_of_field = symbol_table.get_var_count(SymbolTable::Kind::FIELD);
        vmwriter.write_push(VMWriter::Segment::CONST, num_of_field);
        vmwriter.write_call("Memory.alloc", 1);
        vmwriter.write_pop(VMWriter::Segment::POINTER, 0);
    }
    else if(current_subroutine_type == "method"){
        vmwriter.write_push(VMWriter::Segment::ARG, 0);
        vmwriter.write_pop(VMWriter::Segment::POINTER, 0);
    }

    compile_statements();

    eat("}");
}

void CompilationEngine::compile_var_dec(){
    eat("var");

    std::string type = eat_type();
    std::string name = eat_identifier();
    symbol_table.define(name, type, SymbolTable::Kind::VAR);

    while(tokenizer.current_token == ","){
        eat(",");

        name = eat_identifier();
        symbol_table.define(name, type, SymbolTable::Kind::VAR);
    }

    eat(";");
}

void CompilationEngine::compile_statements(){
    while(tokenizer.token_type == KEYWORD){
        if(tokenizer.current_token == "let")
            compile_let();
        else if(tokenizer.current_token == "if")
            compile_if();
        else if(tokenizer.current_token == "while")
            compile_while();
        else if(tokenizer.current_token == "do")
            compile_do();
        else if(tokenizer.current_token == "return")
            compile_return();
        else
            return;
    }
}

void CompilationEngine::compile_let(){
    eat("let");

    std::string name = eat_identifier();
    SymbolTable::Kind kind = symbol_table.get_kind(name);
    int index = symbol_table.get_index(name);

    if(tokenizer.current_token == "["){ // Array
        eat("[");
        compile_expression(); // push 'i' ( a[i] )
        eat("]");

        vmwriter.write_push(kind_to_segment(kind), index);
        
        vmwriter.write_arithmetic(VMWriter::Command::ADD); // push a + i

        eat("=");

        compile_expression(); // push b[i]

        vmwriter.write_pop(VMWriter::Segment::TEMP, 0); // save result of expression at temp 0
        vmwriter.write_pop(VMWriter::Segment::POINTER, 1); // pop a + i to that
        vmwriter.write_push(VMWriter::Segment::TEMP, 0);
        vmwriter.write_pop(VMWriter::Segment::THAT, 0);
    }
    else{ // normal variable
        eat("=");

        compile_expression();

        vmwriter.write_pop(kind_to_segment(kind), index);
    }

    eat(";");
}

void CompilationEngine::compile_if(){
    std::string label_end = current_class_name + "_" + std::to_string(label_count++);
    std::string label_false = current_class_name + "_" + std::to_string(label_count++);

    eat("if");

    eat("(");
    compile_expression(); // top of stack is the result of if-condition
    eat(")");
    vmwriter.write_arithmetic(VMWriter::Command::NOT);
    vmwriter.write_if(label_false);

    eat("{");
    compile_statements();
    eat("}");
    vmwriter.write_goto(label_end);

    vmwriter.write_label(label_false);
    if(tokenizer.current_token == "else"){
        eat("else");
        eat("{");
        compile_statements();
        eat("}");
    }

    vmwriter.write_label(label_end);
}

void CompilationEngine::compile_while(){
    std::string label_cond = current_class_name + "_" + std::to_string(label_count++);
    std::string label_end = current_class_name + "_" + std::to_string(label_count++);

    eat("while");

    vmwriter.write_label(label_cond);
    eat("(");
    compile_expression();
    eat(")");
    vmwriter.write_arithmetic(VMWriter::Command::NOT);
    vmwriter.write_if(label_end);

    eat("{");
    compile_statements();
    eat("}");

    vmwriter.write_goto(label_cond);
    vmwriter.write_label(label_end);
}

void CompilationEngine::compile_do(){
    eat("do");

    std::string name = eat_identifier();

    if(tokenizer.current_token == "."){ // var.subroutine()
        eat(".");
        std::string subroutine_name = eat_identifier();

        int num_of_args = 0;
        std::string type;

        if(symbol_table.exists(name)){ // method of a variable
            SymbolTable::Kind kind = symbol_table.get_kind(name);
            int index = symbol_table.get_index(name);
            vmwriter.write_push(kind_to_segment(kind), index);
            num_of_args++;
            type = symbol_table.get_type(name);
        }
        else{ // static method ( function )
            type = name;
        }

        eat("(");
        num_of_args += compile_expression_list(); // push args
        eat(")");

        vmwriter.write_call(type + "." + subroutine_name, num_of_args);
        vmwriter.write_pop(VMWriter::Segment::TEMP, 0);
    }
    else{ // method
        vmwriter.write_push(VMWriter::Segment::POINTER, 0); // push this

        eat("(");
        int num_of_args = compile_expression_list(); // push args
        eat(")");

        vmwriter.write_call(current_class_name + "." + name, num_of_args + 1);
        vmwriter.write_pop(VMWriter::Segment::TEMP, 0);
    }

    eat(";");
}

void CompilationEngine::compile_return(){
    eat("return");

    if(tokenizer.current_token != ";"){
        compile_expression();
    }
    else{
        vmwriter.write_push(VMWriter::Segment::CONST, 0);
    }

    vmwriter.write_return();
    
    eat(";");
}

void CompilationEngine::compile_expression(){
    compile_term();

    static const std::string OPERATORS = "+-*/&|<>=";   

    while(OPERATORS.find(tokenizer.current_token) != std::string::npos){
        char op = tokenizer.current_token[0];
        tokenizer.advance();

        compile_term();

        switch (op){
        case '+': vmwriter.write_arithmetic(VMWriter::Command::ADD); break;
        case '-': vmwriter.write_arithmetic(VMWriter::Command::SUB); break;
        case '*': vmwriter.write_call("Math.multiply", 2); break;
        case '/': vmwriter.write_call("Math.divide", 2); break;
        case '&': vmwriter.write_arithmetic(VMWriter::Command::AND); break;
        case '|': vmwriter.write_arithmetic(VMWriter::Command::OR); break;
        case '<': vmwriter.write_arithmetic(VMWriter::Command::LT); break;
        case '>': vmwriter.write_arithmetic(VMWriter::Command::GT); break;
        case '=': vmwriter.write_arithmetic(VMWriter::Command::EQ); break;
        }
    }
}

void CompilationEngine::compile_term(){
    switch (tokenizer.token_type){
    case INT_CONST:
        vmwriter.write_push(VMWriter::Segment::CONST, tokenizer.get_int_val());
        tokenizer.advance();
        break;
    case STRING_CONST: {
        std::string str = tokenizer.get_string_val();
        int len = str.length();

        vmwriter.write_push(VMWriter::Segment::CONST, len);
        vmwriter.write_call("String.new", 1);

        for(char c : str){
            vmwriter.write_push(VMWriter::Segment::CONST, (int) c);
            vmwriter.write_call("String.appendChar", 2);
        }

        tokenizer.advance();
        break;
    }
    case IDENTIFIER:{
        std::string name = eat_identifier();

        if(tokenizer.current_token == "["){
            eat("[");
            compile_expression();
            eat("]");

            SymbolTable::Kind kind = symbol_table.get_kind(name);
            int index = symbol_table.get_index(name);
            vmwriter.write_push(kind_to_segment(kind), index);
            vmwriter.write_arithmetic(VMWriter::Command::ADD);
            vmwriter.write_pop(VMWriter::Segment::POINTER, 1);
            vmwriter.write_push(VMWriter::Segment::THAT, 0);
        }
        else if(tokenizer.current_token == "(" || tokenizer.current_token == "."){
            if(tokenizer.current_token == "."){ // sub routine of diff variable
                eat(".");
                
                std::string subroutine_name = eat_identifier();

                eat("(");
                int num_of_args = compile_expression_list();
                eat(")");

                if(symbol_table.exists(name)){ // method
                    SymbolTable::Kind kind = symbol_table.get_kind(name);
                    int index = symbol_table.get_index(name);
                    vmwriter.write_push(kind_to_segment(kind), index);
                    num_of_args++;
                }
                
                vmwriter.write_call(name + "." + subroutine_name, num_of_args);
            }
            else{ // method
                eat("(");
                int num_of_args = compile_expression_list();
                eat(")");

                vmwriter.write_push(VMWriter::Segment::POINTER, 0);
                vmwriter.write_call(name, num_of_args + 1);
            }
        }
        else{ // normal variable
            SymbolTable::Kind kind = symbol_table.get_kind(name);
            int index = symbol_table.get_index(name);
            vmwriter.write_push(kind_to_segment(kind), index);
        }
        break;
    }
    case SYMBOL:
        if(tokenizer.current_token == "-"){
            tokenizer.advance();
            compile_term();
            vmwriter.write_arithmetic(VMWriter::Command::NEG);
        }
        else if(tokenizer.current_token == "~"){
            tokenizer.advance();
            compile_term();
            vmwriter.write_arithmetic(VMWriter::Command::NOT);
        }
        else if(tokenizer.current_token == "("){ // expression
            eat("(");
            compile_expression();
            eat(")");
        }
        else{
            throw std::runtime_error("Cannot compile term: " + tokenizer.current_token);
        }
        break;
    case KEYWORD:
        if(tokenizer.current_token == "true"){
            eat("true");
            vmwriter.write_push(VMWriter::Segment::CONST, 1);
            vmwriter.write_arithmetic(VMWriter::Command::NEG);
        }
        else if(tokenizer.current_token == "false"){
            eat("false");
            vmwriter.write_push(VMWriter::Segment::CONST, 0);
        }
        else if(tokenizer.current_token == "null"){
            eat("false");
            vmwriter.write_push(VMWriter::Segment::CONST, 0);
        }
        else if(tokenizer.current_token == "this"){
            eat("this");
            vmwriter.write_push(VMWriter::Segment::POINTER, 0);
        }
        else{
            throw std::runtime_error("Cannot compile term: " + tokenizer.current_token);
        }
        break;
    default:
        throw std::runtime_error("Cannot compile term: " + tokenizer.current_token);
        break;
    }
}

int CompilationEngine::compile_expression_list(){
    int num_of_exp = 0;

    if(tokenizer.current_token != ")"){
        compile_expression();
        num_of_exp++;
    }

    while(tokenizer.current_token == ","){
        eat(",");
        compile_expression();
        num_of_exp++;
    }

    return num_of_exp;
}