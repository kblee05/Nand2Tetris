#include "VMWriter.h"

// ===============================private==============================

std::string VMWriter::get_segment_str(Segment segment){
    switch(segment){
    case Segment::CONST: return "constant";
    case Segment::ARG: return "argument";
    case Segment::LOCAL: return "local";
    case Segment::STATIC: return "static";
    case Segment::THIS: return "this";
    case Segment::THAT: return "that";
    case Segment::POINTER: return "pointer";
    case Segment::TEMP: return "temp";
    }
}

// ===============================private==============================

VMWriter::VMWriter(std::ofstream& ofstream) : output(ofstream) {

}

void VMWriter::write_push(Segment segment, int index){
    output << "\tpush " << get_segment_str(segment) << " " << index << "\n";
}

void VMWriter::write_pop(Segment segment, int index){
    output << "\tpop " << get_segment_str(segment) << " " << index << "\n";
}

void VMWriter::write_arithmetic(Command command){
    switch(command){
    case Command::ADD: output << "\tadd\n"; break;
    case Command::SUB: output << "\tsub\n"; break;
    case Command::NEG: output << "\tneg\n"; break;
    case Command::EQ: output << "\teq\n"; break;
    case Command::GT: output << "\tgt\n"; break;
    case Command::LT: output << "\tlt\n"; break;
    case Command::AND: output << "\tand\n"; break;
    case Command::OR: output << "\tor\n"; break;
    case Command::NOT: output << "\tnot\n"; break;
    }
}

void VMWriter::write_label(std::string label){
    output << "label " << label << "\n";
}

void VMWriter::write_goto(std::string label){
    output << "\tgoto " << label << "\n";
}

void VMWriter::write_if(std::string label){
    output << "\tif-goto " << label << "\n";
}

void VMWriter::write_call(std::string name, int num_of_args){
    output << "\tcall " << name << " " << num_of_args << "\n";
}

void VMWriter::write_function(std::string name, int num_of_vars){
    output << "function " << name << " " << num_of_vars << "\n";
}

void VMWriter::write_return(){
    output << "\treturn\n";
}

void VMWriter::close(){
    output.close();
}