#ifndef PARENT_POINTER_H
#define PARENT_POINTER_H
#include "../AST.h"
#include "visitor.h"

struct ParentPointerVisitor : Visitor<void, VoidContext> {
    void accept(AST_node_* node, VoidContext ctx) {
        if (node == 0) {
            return;
        }
        node->accept(*this, ctx);
    }

    void visitAST_node(AST_node_* node, VoidContext ctx) {
         EM_error("Not implemented");
    }
    void visitRoot(A_root_* node, VoidContext ctx) {
        EM_debug("setting parent for A_root_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_main_expr(), ctx);
    }
    void visitNilExp(A_nilExp_* node, VoidContext ctx) {
        EM_debug("setting parent for A_nilExp_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);
    }
    void visitBoolExp(A_boolExp_* node, VoidContext ctx) {
        EM_debug("setting parent for A_boolExp_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);
    }
    void visitIntExp(A_intExp_* node, VoidContext ctx) {
        EM_debug("setting parent for A_intExp_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);
    }
    void visitStringExp(A_stringExp_* node, VoidContext ctx) {
        EM_debug("setting parent for A_stringExp_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);
    }
    void visitRecordExp(A_recordExp_* node, VoidContext ctx) {
        EM_debug("setting parent for A_recordExp_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_fields(), ctx);
    }
    void visitArrayExp(A_arrayExp_* node, VoidContext ctx) {
        EM_debug("setting parent for A_arrayExp_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_size(), ctx);
        accept(node->get_init(), ctx);
    }
    void visitVarExp(A_varExp_* node, VoidContext ctx) {
        EM_debug("setting parent for A_varExp_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_var(), ctx);
    }
    void visitOpExp(A_opExp_* node, VoidContext ctx) {
        EM_debug("setting parent for A_opExp_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_left(), ctx);
        accept(node->get_right(), ctx);
    }
    void visitAssignExp(A_assignExp_* node, VoidContext ctx) {
        EM_debug("setting parent for A_assignExp_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_var(), ctx);
        accept(node->get_exp(), ctx);
    }
    void visitLetExp(A_letExp_* node, VoidContext ctx) {
        EM_debug("setting parent for A_letExp_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_decs(), ctx);
        accept(node->get_body(), ctx);
    }
    void visitCallExp(A_callExp_* node, VoidContext ctx) {
        EM_debug("setting parent for A_callExp_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_args(), ctx);
    }
    void visitIfExp(A_ifExp_* node, VoidContext ctx) {
        EM_debug("setting parent for A_ifExp_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_test(), ctx);
        accept(node->get_then(), ctx);
        accept(node->get_else_or_null(), ctx);
    }
    void visitWhileExp(A_whileExp_* node, VoidContext ctx) {
        EM_debug("setting parent for A_whileExp_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_test(), ctx);
        accept(node->get_body(), ctx);
    }
    void visitForExp(A_forExp_* node, VoidContext ctx) {
        EM_debug("setting parent for A_forExp_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_lo(), ctx);
        accept(node->get_hi(), ctx);
        accept(node->get_body(), ctx);
    }
    void visitBreakExp(A_breakExp_* node, VoidContext ctx) {
        EM_debug("setting parent for A_breakExp_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);
    }
    void visitSeqExp(A_seqExp_* node, VoidContext ctx) {
        EM_debug("setting parent for A_seqExp_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_seq(), ctx);
    }
    void visitSimpleVar(A_simpleVar_* node, VoidContext ctx) {
        EM_debug("setting parent for A_simpleVar_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);
    }
    void visitFieldVar(A_fieldVar_* node, VoidContext ctx) {
        EM_debug("setting parent for A_fieldVar_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_var(), ctx);
    }
    void visitSubscriptVar(A_subscriptVar_* node, VoidContext ctx) {
        EM_debug("setting parent for A_subscriptVar_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_var(), ctx);
    }
    void visitExpList(A_expList_* node, VoidContext ctx) {
        EM_debug("setting parent for A_expList_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_head(), ctx);
        accept(node->get_tail(), ctx);
    }
    void visitEfield(A_efield_* node, VoidContext ctx) {
        EM_debug("setting parent for A_efield_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_exp(), ctx);
    }
    void visitEfieldList(A_efieldList_* node, VoidContext ctx) {
        EM_debug("setting parent for A_efieldList_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        accept(node->get_head(), ctx);
        accept(node->get_tail(), ctx);
    }
    void visitDecList(A_decList_* node, VoidContext ctx) {
        EM_debug("setting parent for A_decList_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_head(), ctx);
        accept(node->get_tail(), ctx);
    }
    void visitVarDec(A_varDec_* node, VoidContext ctx) {
        EM_debug("setting parent for A_varDec_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_init(), ctx);
    }
    void visitFunctionDec(A_functionDec_* node, VoidContext ctx) {
        EM_debug("setting parent for A_functionDec_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_theFunctions(), ctx);
    }
    void visitFundecList(A_fundecList_* node, VoidContext ctx) {
        EM_debug("setting parent for A_fundecList_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_head(), ctx);
        accept(node->get_tail(), ctx);
    }
    void visitFundec(A_fundec_* node, VoidContext ctx) {
        EM_debug("setting parent for A_fundec_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_params(), ctx);
        accept(node->get_body(), ctx);
    }
    void visitTypeDec(A_typeDec_* node, VoidContext ctx) {
        EM_debug("setting parent for A_typeDec_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_theTypes(), ctx);
    }
    void visitNametyList(A_nametyList_* node, VoidContext ctx) {
        EM_debug("setting parent for A_nametyList_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_head(), ctx);
        accept(node->get_tail(), ctx);
    }
    void visitNamety(A_namety_* node, VoidContext ctx) {
        EM_debug("setting parent for A_namety_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_ty(), ctx);
    }
    void visitFieldList(A_fieldList_* node, VoidContext ctx) {
        EM_debug("setting parent for A_fieldList_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_head(), ctx);
        accept(node->get_tail(), ctx);
    }
    void visitField(A_field_* node, VoidContext ctx) {
        EM_debug("setting parent for A_field_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);
    }
    void visitNameTy(A_nameTy_* node, VoidContext ctx) {
        EM_debug("setting parent for A_nameTy_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);
    }
    void visitRecordty(A_recordty_* node, VoidContext ctx) {
        EM_debug("setting parent for A_recordty_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);

        ctx.parent = node;
        accept(node->get_record(), ctx);
    }
    void visitArrayty(A_arrayty_* node, VoidContext ctx) {
        EM_debug("setting parent for A_arrayty_");
        AST_node_* parent = ctx.parent;
        node->set_stored_parent(parent);
    }
};
#endif
