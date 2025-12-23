#include "SymbolTable.h"
#include <stdexcept>

// ===============================private==============================

SymbolTable::SymbolInfo SymbolTable::find_symbol_info(std::string name){
    if(subroutine_table.count(name)){
        return subroutine_table[name];
    }
    else if(class_table.count(name)){
        return class_table[name];
    }
    throw std::runtime_error("Variable does not exist");
}

// ===============================private==============================

SymbolTable::SymbolTable(){
    indices[Kind::STATIC] = 0;
    indices[Kind::FIELD] = 0;
    indices[Kind::ARG] = 0;
    indices[Kind::VAR] = 0;
}

void SymbolTable::reset(){
    subroutine_table.clear();
    indices[Kind::VAR] = 0;
    indices[Kind::ARG] = 0;
}

void SymbolTable::define(std::string name, std::string type, SymbolTable::Kind kind){
    if(kind == Kind::STATIC || kind == Kind::FIELD){
        class_table[name] = {type, kind, indices[kind]++};
    }
    else{ // name == ARG or VAR
        subroutine_table[name] = {type, kind, indices[kind]++};
    }
}

int SymbolTable::get_var_count(Kind kind){
    return indices[kind];
}

SymbolTable::Kind SymbolTable::get_kind(std::string name){
    return find_symbol_info(name).kind;
}

std::string SymbolTable::get_type(std::string name){
    return find_symbol_info(name).type;
}

int SymbolTable::get_index(std::string name){
    return find_symbol_info(name).index;
}

bool SymbolTable::exists(std::string name){
    if(subroutine_table.count(name) || class_table.count(name)){
        return true;
    }
    return false;
}