#include "CompilationEngine.h"

// ===============================private==============================

void CompilationEngine::eat(std::string expected_token){
    if(tokenizer.current_token == expected_token){
        tokenizer.advance();
        return;
    }
    throw std::runtime_error("Current token: " + tokenizer.current_token + " does not match " + expected_token + "\n");
}

void CompilationEngine::eat_identifier(){
    if(tokenizer.token_type == IDENTIFIER){
        tokenizer.advance();
        return;
    }
    throw std::runtime_error("Current token: " + tokenizer.current_token + " is not an IDENTIFIER\n");
}

void CompilationEngine::print_xml(std::string tag, std::string body){
    for(int i=0; i<identation_level; i++)
        output << "\t";
    output << "<" << tag << "> " << body << " </" << tag << ">\n"; 
}

void CompilationEngine::print_xml(std::string tag, bool is_start){
    for(int i=0; i<identation_level; i++)
        output << "\t";
    
    if(is_start){ // start of identation
        output << "<" << tag << ">\n";
        identation_level++;
    }
    else{ // end of identation
        output << "</" << tag << ">\n";
        identation_level--;
    }
}

void CompilationEngine::compile_type(){
    if(tokenizer.current_token == "int" ||
       tokenizer.current_token == "char" ||
       tokenizer.current_token == "boolean")
    {
        print_xml("keyword", tokenizer.current_token);
        tokenizer.advance();
    }
    else{
        print_xml("identifier", tokenizer.current_token);
        eat_identifier();
    }
}

// ===============================public===============================

CompilationEngine::CompilationEngine(std::ofstream& ofstream, JackTokenizer& tokenizer) : output(ofstream), identation_level(1), tokenizer(tokenizer) {
    
}

void CompilationEngine::compile_class(){
    print_xml("keyword", "class");
    eat("class");

    print_xml("identifier", tokenizer.current_token);
    tokenizer.advance();

    print_xml("symbol", "{");
    eat("{");

    while(tokenizer.current_token == "field" || tokenizer.current_token == "static")
        compile_class_var_dec();
    
    while(tokenizer.current_token == "constructor" ||
          tokenizer.current_token == "function" ||
          tokenizer.current_token == "method")
        compile_subroutine();

    print_xml("symbol", "}");
    eat("}");
}

void CompilationEngine::compile_class_var_dec(){
    print_xml("classVarDec", true);

    print_xml("keyword", tokenizer.current_token);
    tokenizer.advance();

    compile_type();

    print_xml("identifier", tokenizer.current_token);
    eat_identifier();

    while(tokenizer.current_token == ","){
        print_xml("symbol", ",");
        eat(",");

        print_xml("identifier", tokenizer.current_token);
        eat_identifier();
    }

    print_xml("classVarDec", false);
}

void CompilationEngine::compile_subroutine(){
    print_xml("subroutineDec", 1);

    print_xml("keyword", tokenizer.current_token);
    tokenizer.advance();

    if(tokenizer.current_token == "void"){
        print_xml("keyword", "void");
        tokenizer.advance();
    }
    else
        compile_type();

    print_xml("identifier", tokenizer.current_token);
    eat_identifier();

    print_xml("symbol", "(");
    eat("(");

    compile_parameter_list();

    print_xml("symbol", ")");
    eat(")");

    compile_subroutine_body();

    print_xml("subroutineDec", 0);
}

void CompilationEngine::compile_parameter_list(){
    if(tokenizer.current_token == ")")
        return;
    
    print_xml("parameterList", 1);

    compile_type();
    print_xml("identifier", tokenizer.current_token);

    while(tokenizer.current_token == ","){
        print_xml("symbol", ",");
        eat(",");

        print_xml("identifier", tokenizer.current_token);
        eat_identifier();
    }

    print_xml("parameterList", 0);
}

void CompilationEngine::compile_subroutine_body(){
    print_xml("subroutineBody", 1);

    print_xml("symbol", "{");
    eat("{");

    while(tokenizer.current_token == "var")
        compile_var_dec();

    compile_statements();

    print_xml("subroutineBody", 0);
}

void CompilationEngine::compile_var_dec(){

}

void CompilationEngine::compile_statements(){

}

void CompilationEngine::compile_let(){

}

void CompilationEngine::compile_if(){

}

void CompilationEngine::compile_while(){

}

void CompilationEngine::compile_do(){

}

void CompilationEngine::compile_return(){

}

void CompilationEngine::compile_expression(){

}

void CompilationEngine::compile_term(){

}

void CompilationEngine::compile_expression_list(){

}