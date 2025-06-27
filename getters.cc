/**
 * @file getters.cc
 * @brief Getter functions for private attribute that run into circular dependencies in the main AST.h file 
 */
#include "AST.h"


AST_node_* A_recordExp_::get_fields() const {
    return _fields;
}


AST_node_* A_opExp_::get_left() const {
    return _left;
}

AST_node_* A_varExp_::get_var() const {
    return _var;
}

AST_node_* A_opExp_::get_right() const {
    return _right;
}

AST_node_* A_assignExp_::get_var() const {
    return _var;
}
AST_node_* A_assignExp_::get_exp() const {
    return _exp;
}

AST_node_* A_letExp_::get_decs() const {
    return _decs;
}
AST_node_* A_letExp_::get_body() const {
    return _body;
}

AST_node_* A_callExp_::get_args() const {
    return _args;
}

AST_node_* A_ifExp_::get_test() const {
    return _test;
}
AST_node_* A_ifExp_::get_then() const {
    return _then;
}
AST_node_* A_ifExp_::get_else_or_null() const {
    return _else_or_null;
}

AST_node_* A_whileExp_::get_test() const {
    return _test;
}
AST_node_* A_whileExp_::get_body() const {
    return _body;
}

AST_node_* A_forExp_::get_lo() const {
    return _lo;
}
AST_node_* A_forExp_::get_hi() const {
    return _hi;
}
AST_node_* A_forExp_::get_body() const {
    return _body;
}

AST_node_* A_seqExp_::get_seq() const {
    return _seq;
}

AST_node_* A_fieldVar_::get_var() const {
    return _var;
}

AST_node_* A_subscriptVar_::get_var() const {
    return _var;
}
AST_node_* A_subscriptVar_::get_exp() const {
    return _exp;
}

AST_node_* A_expList_::get_head() const {
    return _head;
}
AST_node_* A_expList_::get_tail() const {
    return _tail;
}

AST_node_* A_efield_::get_exp() const {
    return _exp;
}

AST_node_* A_efieldList_::get_head() const {
    return _head;
}
AST_node_* A_efieldList_::get_tail() const {
    return _tail;
}

AST_node_* A_decList_::get_head() const {
    return _head;
}
AST_node_* A_decList_::get_tail() const {
    return _tail;
}

// A_varDec_
AST_node_* A_varDec_::get_init() const {
    return _init;
}

AST_node_* A_typeDec_::get_theTypes() const {
    return theTypes;
}

// A_functionDec_
AST_node_* A_functionDec_::get_theFunctions() const {
    return theFunctions;
}

AST_node_* A_fundecList_::get_head() const {
    return _head;
}
AST_node_* A_fundecList_::get_tail() const {
    return _tail;
}

AST_node_* A_fundec_::get_params() const {
    return _params;
}
AST_node_* A_fundec_::get_body() const {
    return _body;
}

AST_node_* A_namety_::get_ty() const {
    return _ty;
}

AST_node_* A_nametyList_::get_head() const {
    return _head;
}
AST_node_* A_nametyList_::get_tail() const {
    return _tail;
}

AST_node_* A_fieldList_::get_head() const {
    return _head;
}
AST_node_* A_fieldList_::get_tail() const {
    return _tail;
}

AST_node_* A_recordty_::get_record() const {
    return _record;
}
