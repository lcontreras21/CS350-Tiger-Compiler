/**
 * @file visitor.h
 * @brief Define a Visitor class
 * Visitor handles the one part of the Double dispatch framework.
 * Each Visitor is responsible for handling how childrens are visited.
 */

#if ! defined VISITOR_H
#define VISITOR_H 
#include <iostream>
#include "../ST.h"

// Forward Declare these classes to avoid circular dependencies
class AST_node_;
class A_root_; 
class A_nilExp_; 
class A_boolExp_; 
class A_intExp_; 
class A_stringExp_; 
class A_recordExp_; 
class A_arrayExp_; 
class A_varExp_; 
class A_opExp_; 
class A_assignExp_; 
class A_letExp_; 
class A_callExp_; 
class A_ifExp_; 
class A_whileExp_; 
class A_forExp_; 
class A_breakExp_; 
class A_seqExp_; 
class A_simpleVar_; 
class A_fieldVar_; 
class A_subscriptVar_; 
class A_expList_; 
class A_efield_; 
class A_efieldList_; 
class A_decList_; 
class A_varDec_; 
class A_typeDec_; 
class A_functionDec_; 
class A_fundecList_; 
class A_fundec_; 
class A_namety_; 
class A_nametyList_; 
class A_fieldList_; 
class A_field_; 
class A_nameTy_; 
class A_recordty_; 
class A_arrayty_; 


struct Context {
};

struct StringContext : public Context {

    // PrintVisitor
    static const int tab = 2;
    int indent = 0;
};

struct VoidContext : public Context {
    
    // ParentVisitor
    AST_node_* parent = 0;

    // VariableLibraryVisitor
    ST<var_info> local_variable_library = ST<var_info>();
    int field_index = 0;  // used in A_fundec_ _args (A_fieldList_) attribute
};

template<typename T, typename Ctx>
struct Visitor {
    virtual ~Visitor() = default;
    virtual T visitAST_node(AST_node_*, Ctx) = 0;
    virtual T visitRoot(A_root_*, Ctx) = 0;
    virtual T visitNilExp(A_nilExp_*, Ctx) = 0;
    virtual T visitBoolExp(A_boolExp_*, Ctx) = 0;
    virtual T visitIntExp(A_intExp_*, Ctx) = 0;
    virtual T visitStringExp(A_stringExp_*, Ctx) = 0;
    virtual T visitRecordExp(A_recordExp_*, Ctx) = 0;
    virtual T visitArrayExp(A_arrayExp_*, Ctx) = 0;
    virtual T visitVarExp(A_varExp_*, Ctx) = 0;
    virtual T visitOpExp(A_opExp_*, Ctx) = 0;
    virtual T visitAssignExp(A_assignExp_*, Ctx) = 0;
    virtual T visitLetExp(A_letExp_*, Ctx) = 0;
    virtual T visitCallExp(A_callExp_*, Ctx) = 0;
    virtual T visitIfExp(A_ifExp_*, Ctx) = 0;
    virtual T visitWhileExp(A_whileExp_*, Ctx) = 0;
    virtual T visitForExp(A_forExp_*, Ctx) = 0;
    virtual T visitBreakExp(A_breakExp_*, Ctx) = 0;
    virtual T visitSeqExp(A_seqExp_*, Ctx) = 0;
    virtual T visitSimpleVar(A_simpleVar_*, Ctx) = 0;
    virtual T visitFieldVar(A_fieldVar_*, Ctx) = 0;
    virtual T visitSubscriptVar(A_subscriptVar_*, Ctx) = 0;
    virtual T visitExpList(A_expList_*, Ctx) = 0;
    virtual T visitEfield(A_efield_*, Ctx) = 0;
    virtual T visitEfieldList(A_efieldList_*, Ctx) = 0;
    virtual T visitDecList(A_decList_*, Ctx) = 0;
    virtual T visitVarDec(A_varDec_*, Ctx) = 0;
    virtual T visitTypeDec(A_typeDec_*, Ctx) = 0;
    virtual T visitFunctionDec(A_functionDec_*, Ctx) = 0;
    virtual T visitFundecList(A_fundecList_*, Ctx) = 0;
    virtual T visitFundec(A_fundec_*, Ctx) = 0;
    virtual T visitNamety(A_namety_*, Ctx) = 0;
    virtual T visitNametyList(A_nametyList_*, Ctx) = 0;
    virtual T visitFieldList(A_fieldList_*, Ctx) = 0;
    virtual T visitField(A_field_*, Ctx) = 0;
    virtual T visitNameTy(A_nameTy_*, Ctx) = 0;
    virtual T visitRecordty(A_recordty_*, Ctx) = 0;
    virtual T visitArrayty(A_arrayty_*, Ctx) = 0;
};

#endif
