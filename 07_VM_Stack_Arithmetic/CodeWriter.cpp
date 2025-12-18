#include "CodeWriter.h"

CodeWriter::CodeWriter(std::string filename) {
    size_t dot_pos = filename.find_last_of(".");
    raw_name = filename.substr(0, dot_pos);
    this->filename = raw_name + ".asm";
    output.open(this->filename);

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

    label_count = 0;
}

void CodeWriter::write_pushpop(CommandType command, std::string segment, int index){
    // add comment
    output << "// " << command << " " << segment << " " << index << "\n";

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
    // comment
    output << "// " << command << "\n";

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