#include <iostream>
#include <string>
#include <fstream>
#include <map>

std::map<std::string, std::string> memory_segment = {
    {"local", "LCL"},
    {"arguement", "ARG"},
    {"this", "THIS"},
    {"that", "THAT"}
};

std::map<std::string, std::string> jump = {
    {"eq", "JEQ"},
    {"gt", "JGT"},
    {"lt", "JLT"}  
};

void write_push_pop(std::ofstream& output_file, std::string command, std::string segment_type, int index);
void write_stack_commands(std::ofstream& output_file, std::string command);

int main(){

    std::string filename = "StackTest"; 
    //std::cin >> filename; // "BasicTest.vm"

    std::ifstream input_file(filename + ".vm");
    std::ofstream output_file(filename + ".asm");

    std::string input;
    while(input_file >> input){
        if(input == "push" || input == "pop"){
            std::string segment_type;
            input_file >> segment_type;
            int index;
            input_file >> index;

            write_push_pop(output_file, input, segment_type, index);
        }
        else if(input == "add" || input == "sub" || input == "neg" ||
                input == "eq"  || input == "gt"  || input == "lt"  ||
                input == "and" || input == "or"  || input == "not")
        {
            write_stack_commands(output_file, input);
        }
    }

    input_file.close();
    output_file.close();

    return 0;
}

void write_push_pop(std::ofstream& output_file, std::string command, std::string segment_type, int index){

    // commment
    output_file << "// " << command << " " << segment_type << " " << index << "\n";

    if(command == "push"){

        if(segment_type == "constant"){
            output_file << "@" << index << "\n";
            output_file << "D=A\n";
        }
        else if(segment_type == "temp"){
            output_file << "@5\n";
            output_file << "D=A\n";
            output_file << "@" << index << "\n";
            output_file << "A=D+A\n";
            output_file << "D=M\n";
        }
        else{
            // Find Register
            output_file << "@" << memory_segment[segment_type] << "\n";
            output_file << "D=A\n";
            output_file << "@" << index << "\n";
            output_file << "A=D+A\n";
            output_file << "D=M\n";
        }

        // Push in Stack
        output_file << "@SP\n";
        output_file << "A=M\n";
        output_file << "M=D\n";
        output_file << "@SP\n";
        output_file << "M=M+1\n";
    }
    else if(command == "pop"){
        // Store memory segment address
        output_file << "@" << memory_segment[segment_type] << "\n";
        output_file << "D=M\n";
        output_file << "@" << index << "\n";
        output_file << "@R13\n";
        output_file << "M=D\n";

        // Access Stack
        output_file << "@SP\n";
        output_file << "AM=M-1\n";
        output_file << "D=M\n";

        // Pop to sement address
        output_file << "@R13\n";
        output_file << "A=M\n";
        output_file << "M=D\n";
    }
}

void write_stack_commands(std::ofstream& output_file, std::string command){
    // comment
    output_file << "// " << command << "\n";

    if(command == "neg" || command == "not"){ // Need one item from stack
        output_file << "@SP\n";
        output_file << "AM=M-1\n";
        
        output_file << (command == "neg" ? "M=-M\n" : "M=!M\n");

        output_file << "@SP\n";
        output_file << "M=M+1\n";
    }
    else{ // Need two items from Stack
        // Item A
        output_file << "@SP\n";
        output_file << "AM=M-1\n";
        output_file << "D=M\n";
        output_file << "@R13\n";
        output_file << "M=D\n";

        // Item B
        output_file << "@SP\n";
        output_file << "AM=M-1\n";
        output_file << "D=M\n";

        // Restore Item A
        output_file << "@R13\n";

        if(command == "add")
            output_file << "D=D+M\n";
        else if(command == "sub")
            output_file << "D=D-M\n"; // Item B - Item A (B was pushed earlier than A)
        else if(command == "and")
            output_file << "D=D&M\n";
        else if(command == "or")
            output_file << "D=D|M\n";
        else{ // eq, gt, lt
            static int label_count = 0;

            // if statement
            output_file << "D=D-M\n"; // Item B - Item A (B was pushed earlier than A)
            output_file << "@TRUE_" << label_count << "\n";
            output_file << "D;" << jump[command] << "\n";
            
            // false
            output_file << "D=0\n";
            output_file << "@END_" << label_count << "\n";
            output_file << "0;JMP\n";

            // true
            output_file << "(TRUE_" << label_count << ")\n";
            output_file << "D=-1\n";
            
            // end
            output_file << "(END_" << label_count << ")\n";

            label_count++;
        }

        // push final result
        output_file << "@SP\n";
        output_file << "A=M\n";
        output_file << "M=D\n";
        output_file << "@SP\n";
        output_file << "M=M+1\n";
    }
}