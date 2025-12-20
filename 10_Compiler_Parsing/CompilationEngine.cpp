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

    print_xml("keyword", tokenizer.current_token); // constructor function method
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

        compile_type();

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

    print_xml("symbol", "}");
    eat("}");

    print_xml("subroutineBody", 0);
}

void CompilationEngine::compile_var_dec(){
    print_xml("keyword", "var");

    compile_type();

    print_xml("identifier", tokenizer.current_token);
    eat_identifier();

    while(tokenizer.current_token == ","){
        print_xml("symbol", ",");
        eat(",");

        print_xml("identifier", tokenizer.current_token);
        eat_identifier();
    }

    print_xml("symbol", ";");
    eat(";");
}

void CompilationEngine::compile_statements(){
    print_xml("statements", 1);

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

    print_xml("statements", 0);
}

void CompilationEngine::compile_let(){
    print_xml("letStatement", 1);

    print_xml("keyword", "let");
    eat("let");

    print_xml("identifier", tokenizer.current_token);
    eat_identifier();

    if(tokenizer.current_token == "["){
        print_xml("symbol", "[");
        tokenizer.advance();

        compile_expression();

        print_xml("symbol", "]");
        eat("]");
    }

    print_xml("symbol", "=");
    eat("=");

    compile_expression();

    print_xml("symbol", ";");
    eat("");

    print_xml("letStatement", 0);
}

void CompilationEngine::compile_if(){
    print_xml("ifStatement", 1);

    print_xml("keyword", "if");
    eat("if");

    print_xml("symbol", "(");
    eat("(");

    compile_expression();

    print_xml("symbol", ")");
    eat(")");

    print_xml("symbol", "{");
    eat("{");

    compile_statements();

    print_xml("symbol", "}");
    eat("}");

    if(tokenizer.current_token == "else"){
        print_xml("keyword", "else");
        tokenizer.advance();

        print_xml("symbol", "{");
        eat("{");

        compile_statements();

        print_xml("symbol", "}");
        eat("}");
    }

    print_xml("ifStatement", 0);
}

void CompilationEngine::compile_while(){
    print_xml("whileStatement", 1);

    print_xml("keyword", "while");
    eat("while");

    print_xml("symbol", "(");
    eat("(");

    compile_expression();

    print_xml("symbol", ")");
    eat(")");

    print_xml("symbol", "{");
    eat("{");

    compile_statements();

    print_xml("symbol", "}");
    eat("}");

    print_xml("whileStatement", 0);
}

void CompilationEngine::compile_do(){
    print_xml("doStatement", 1);

    print_xml("keyword", "do");
    eat("do");

    print_xml("identifier", tokenizer.current_token);
    eat_identifier();

    if(tokenizer.current_token == "."){
        print_xml("symbol", ".");
        tokenizer.advance();

        print_xml("identifier", tokenizer.current_token);
        eat_identifier();
    }

    print_xml("symbol", "(");
    eat("(");

    compile_expression_list();

    print_xml("symbol", ")");
    eat(")");

    print_xml("symbol", ";");
    eat(";");

    print_xml("doStatement", 0);
}

void CompilationEngine::compile_return(){
    print_xml("returnStatement", 1);

    print_xml("keyword", "return");
    eat("return");

    if(tokenizer.current_token != ";");
        compile_expression();

    print_xml("symbol", ";");
    eat(";");

    print_xml("returnStatement", 0);
}

void CompilationEngine::compile_expression(){
    print_xml("expression", 1);

    compile_term();

    static const std::string OPERATORS = "+-*/&|<>=";

    while(OPERATORS.find(tokenizer.current_token) != std::string::npos){
        print_xml("symbol", tokenizer.current_token);
        tokenizer.advance();

        compile_term();
    }

    print_xml("expression", 0);
}

void CompilationEngine::compile_term(){
    print_xml("term", 1);

    switch (tokenizer.token_type)
    {
    case INT_CONST:
        print_xml("integerConstant", tokenizer.current_token);
        tokenizer.advance();
        break;
    case STRING_CONST:
        print_xml("stringConstant", tokenizer.current_token);
        tokenizer.advance();
        break;
    case IDENTIFIER:
        print_xml("identifier", tokenizer.current_token);
        eat_identifier();
        break;
    default:
        if(tokenizer.current_token == "-" || tokenizer.current_token == "~"){ // unary operator
            print_xml("symbol", tokenizer.current_token);
            tokenizer.advance();
            compile_term();
        }
        else if(tokenizer.current_token == "("){ // expression
            print_xml("symbol", "(");
            tokenizer.advance();

            compile_expression();

            print_xml("symbol", ")");
            eat(")");
        }
        else if(tokenizer.current_token == "true" || // keyword constant: true false null this
                tokenizer.current_token == "false" ||
                tokenizer.current_token == "null" ||
                tokenizer.current_token == "this")
        {
            print_xml("keyword", tokenizer.current_token);
            tokenizer.advance();
        }
        else{ // varName || varName[expression] || subroutineCall
            print_xml("identifier", tokenizer.current_token);
            eat_identifier();

            if(tokenizer.current_token == "["){
                print_xml("symbol", "[");
                tokenizer.advance();

                compile_expression();

                print_xml("symbol", "]");
                eat("]");
            }
            else if(tokenizer.current_token == "(" || tokenizer.current_token == "."){
                if(tokenizer.current_token == "."){
                    print_xml("symbol", ".");
                    tokenizer.advance();

                    print_xml("identifier", tokenizer.current_token);
                    eat_identifier();
                }

                print_xml("symbol", "(");
                tokenizer.advance();

                compile_expression_list();

                print_xml("symbol", ")");
                eat(")");
            }
        }
        break;
    }

    print_xml("term", 0);
}

void CompilationEngine::compile_expression_list(){
    print_xml("expressionList", 1);

    compile_expression();

    while(tokenizer.current_token == ","){
        print_xml("symbol", ",");
        tokenizer.advance();

        compile_expression();
    }

    print_xml("expressionList", 0);
}