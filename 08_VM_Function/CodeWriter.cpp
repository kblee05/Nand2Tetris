#include "CodeWriter.h"

CodeWriter::CodeWriter(std::string filename) : current_function_name(""), label_count(0), ret_count(0) {
    output.open(filename + ".asm");

    segment_asm = {
        {"local", "LCL"},
        {"argument", "ARG"},
        {"this", "THIS"},
        {"that", "THAT"}
    };

    jump_asm = {
        {"eq", "JEQ"},
        {"gt", "JGT"},
        {"lt", "JLT"}  
    };
}

void CodeWriter::close(){
    output.close();
}

void CodeWriter::set_file_name(std::string vm_name){
    size_t dot_pos = vm_name.find(".");
    raw_name = vm_name.substr(0, dot_pos);
}

std::string CodeWriter::get_full_label(std::string label){
    if(current_function_name.empty())
        return raw_name + "." + label;
    else
        return current_function_name + "$" + label;
}

void CodeWriter::push_D(){
    output << "@SP\n";
    output << "A=M\n";
    output << "M=D\n";
    output << "@SP\n";
    output << "M=M+1\n";
}

//      ==================================public==================================

void CodeWriter::write_pushpop(CommandType command, std::string segment, int index){
    // add comment
    output << "// " << (command == C_PUSH ? "push " : "pop ") << segment << " " << index << "\n";

    switch(command){
    case C_PUSH:
        if(segment == "constant"){ // constant 예외 처리
            output << "@" << index << "\n";
            output << "D=A\n";
        }
        else if(segment == "pointer"){
            switch (index){
            case 0: // pointer 0 : THIS
                output << "@THIS\n";
                output << "D=M\n";
                break;
            case 1: // pointer 0 : THAT
                output << "@THAT\n";
                output << "D=M\n";
                break;
            }
        }
        else if(segment == "static"){
            output << "@" << raw_name << "." << index << "\n";
            output << "D=M\n";
        }
        else if(segment == "temp"){
            output << "@" << 5 + index << "\n";
            output << "D=M\n";
        }
        else{
            output << "@" << segment_asm[segment] << "\n";
            output << "D=M\n";
            output << "@" << index << "\n";
            output << "A=D+A\n";
            output << "D=M\n";
        }

        // Push in Stack
        output << "@SP\n";
        output << "A=M\n";
        output << "M=D\n";
        output << "@SP\n";
        output << "M=M+1\n";

        break;
    case C_POP:
        // Store memory segment address
        if(segment == "pointer"){
            output << (index == 0 ? "@THIS\n" : "@THAT\n");
            output << "D=A\n";
        }
        else if(segment == "static"){
            output << "@" << raw_name << "." << index << "\n";
            output << "D=A\n";
        }
        else if(segment == "temp"){
            output << "@" << 5 + index << "\n";
            output << "D=A\n";
        }
        else{
            output << "@" << segment_asm[segment] << "\n";
            output << "D=M\n";
            output << "@" << index << "\n";
            output << "D=D+A\n";
        }
        output << "@R13\n";
        output << "M=D\n";

        // Access Stack
        output << "@SP\n";
        output << "AM=M-1\n";
        output << "D=M\n";

        // Pop to sement address
        output << "@R13\n";
        output << "A=M\n";
        output << "M=D\n";

        break;
    }
}

void CodeWriter::write_arithmetic(std::string command){
    output << "// " << command << "\n"; // comment

    if(command == "neg" || command == "not"){ // Need one item from stack
        output << "@SP\n";
        output << "A=M-1\n";
        output << (command == "neg" ? "M=-M\n" : "M=!M\n");
    }
    else{ // Need two items from Stack
        // Item A
        output << "@SP\n";
        output << "AM=M-1\n";
        output << "D=M\n";
        output << "@R13\n";
        output << "M=D\n";

        // Item B
        output << "@SP\n";
        output << "AM=M-1\n";
        output << "D=M\n";

        // Restore Item A
        output << "@R13\n";

        if(command == "add")
            output << "D=D+M\n";
        else if(command == "sub")
            output << "D=D-M\n"; // Item B - Item A (B was pushed earlier than A)
        else if(command == "and")
            output << "D=D&M\n";
        else if(command == "or")
            output << "D=D|M\n";
        else{ // eq, gt, lt

            // if condition
            output << "D=D-M\n"; // Item B - Item A (B was pushed earlier than A)
            output << "@TRUE_" << label_count << "\n";
            output << "D;" << jump_asm[command] << "\n";
            
            // false
            output << "D=0\n";
            output << "@END_" << label_count << "\n";
            output << "0;JMP\n";

            // true
            output << "(TRUE_" << label_count << ")\n";
            output << "D=-1\n";
            
            // end
            output << "(END_" << label_count << ")\n";

            label_count++;
        }

        // push final result
        output << "@SP\n";
        output << "A=M\n";
        output << "M=D\n";
        output << "@SP\n";
        output << "M=M+1\n";
    }
}

void CodeWriter::write_label(std::string label){
    output << "// label " << label << "\n"; // comment
    output << "(" << get_full_label(label) << ")\n";
}

void CodeWriter::write_goto(std::string label){
    output << "// goto " << label << "\n"; // comment
    output << "@" << get_full_label(label) << "\n";
    output << "0;JMP\n";
}

void CodeWriter::write_if(std::string label){
    output << "// if-goto " << label << "\n"; // comment

    // get top of stack
    output << "@SP\n";
    output << "AM=M-1\n";
    output << "D=M\n";

    // if top of stack != 0, goto
    output << "@" << get_full_label(label) << "\n";
    output << "D;JNE\n";
}

void CodeWriter::write_call(std::string function_name, int num_of_args){
    output << "// call " << function_name << " " << num_of_args << "\n"; // comment

    // save return address
    output << "@" << get_full_label("ret") << "." << ret_count << "\n";
    output << "D=A\n";
    push_D();

    // push LCL
    output << "@LCL\n";
    output << "D=M\n";
    push_D();

    // push ARG
    output << "@ARG\n";
    output << "D=M\n";
    push_D();

    // push THIS
    output << "@THIS\n";
    output << "D=M\n";
    push_D();

    // push THAT
    output << "@THAT\n";
    output << "D=M\n";
    push_D();

    // ARG = *SP - 5 - num_of_args
    output << "@SP\n";
    output << "D=M\n";
    output << "@5\n";
    output << "D=D-A\n";
    output << "@" << num_of_args << "\n";
    output << "D=D-A\n";
    output << "@ARG\n";
    output << "M=D\n";

    // LCL = SP
    output << "@SP\n";
    output << "D=M\n";
    output << "@LCL\n";
    output << "M=D\n";

    // goto function_name
    output << "@" << function_name <<"\n";
    output << "0;JMP\n";

    // set return labela
    output << "(" << get_full_label("ret") << "." << ret_count++ << ")\n"; 
}

void CodeWriter::write_function(std::string function_name, int num_of_vars){
    output << "// function " << function_name << " " << num_of_vars << "\n"; // comment

    // Inject an entry point label
    output << "("  << function_name << ")\n";

    // Initialize the local segment
    output << "D=0\n";
    for(size_t i = 0; i < num_of_vars; i++)
        push_D(); // LCL = SP when function is called
}

void CodeWriter::write_return(){
    output << "// return\n"; // comment

    // save end_frame at R13
    output << "@LCL\n";
    output << "D=M\n";
    output << "@R13\n";
    output << "M=D\n";

    // save return address at R14
    output << "@5\n";
    output << "A=D-A\n";
    output << "D=M\n";
    output << "@R14\n";
    output << "M=D\n";

    // set return value at ARG 0
    output << "@SP\n";
    output << "AM=M-1\n";
    output << "D=M\n";
    output << "@ARG\n";
    output << "A=M\n";
    output << "M=D\n";

    // set SP to ARG + 1
    output << "@ARG\n";
    output << "D=M\n";
    output << "@SP\n";
    output << "M=D+1\n";

    // set THAT to end_frame(R13) - 1
    output << "@R13\n";
    output << "AM=M-1\n";
    output << "D=M\n";
    output << "@THAT\n";
    output << "M=D\n";

    // set THIS to end_frame - 2
    output << "@R13\n";
    output << "AM=M-1\n";
    output << "D=M\n";
    output << "@THIS\n";
    output << "M=D\n";

    // set ARG to end_frame - 3
    output << "@R13\n";
    output << "AM=M-1\n";
    output << "D=M\n";
    output << "@ARG\n";
    output << "M=D\n";

    // set LCL to end_frame - 4
    output << "@R13\n";
    output << "AM=M-1\n";
    output << "D=M\n";
    output << "@LCL\n";
    output << "M=D\n";

    // goto return address
    output << "@R14\n";
    output << "A=M\n";
    output << "0;JMP\n";
}

void CodeWriter::bootstrap(){
    output << "@256\n";
    output << "D=A\n";
    output << "@SP\n";
    output << "M=D\n";
    raw_name = "bootstrap";
    write_call("Sys.init", 0);
}