#ifndef VARIABLE_LIBRARY_VISITOR_H
#define VARIABLE_LIBRARY_VISITOR_H
#include "../AST.h"
#include "visitor.h"

struct VariableLibraryVisitor : Visitor<ST<var_info>, VoidContext> {
    ST<var_info> accept(AST_node_* node, VoidContext ctx) {
        if (node == 0) {
            return ST<var_info>();
        }
        return node->accept(*this, ctx);
    }

    ST<var_info> visitAST_node(AST_node_* node, VoidContext ctx) {
         EM_error("Not implemented");
        return ST<var_info>();
    }
    ST<var_info> visitRoot(A_root_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_root_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        return accept(node->get_main_expr(), ctx);
    }
    ST<var_info> visitNilExp(A_nilExp_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_nilExp_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        return ST<var_info>();
    }
    ST<var_info> visitBoolExp(A_boolExp_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_boolExp_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        return ST<var_info>();
    }
    ST<var_info> visitIntExp(A_intExp_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_intExp_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        return ST<var_info>();
    }
    ST<var_info> visitStringExp(A_stringExp_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_stringExp_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        return ST<var_info>();
    }
    ST<var_info> visitRecordExp(A_recordExp_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_recordExp_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_fields(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitArrayExp(A_arrayExp_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_arrayExp_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_size(), ctx);
        accept(node->get_init(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitVarExp(A_varExp_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_varExp_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        return accept(node->get_var(), ctx);
    }
    ST<var_info> visitOpExp(A_opExp_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_opExp_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_left(), ctx);
        accept(node->get_right(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitAssignExp(A_assignExp_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_assignExp_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_var(), ctx);
        accept(node->get_exp(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitLetExp(A_letExp_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_letExp_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        ST<var_info> dec_var_lib = accept(node->get_decs(), ctx);

        ST<var_info> new_local_var_lib = MergeAndShadow(dec_var_lib, local_var_lib);
        ctx.local_variable_library = new_local_var_lib;
        accept(node->get_body(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitCallExp(A_callExp_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_callExp_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_args(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitIfExp(A_ifExp_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_ifExp_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_test(), ctx);
        accept(node->get_then(), ctx);
        accept(node->get_else_or_null(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitWhileExp(A_whileExp_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_whileExp_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_test(), ctx);
        accept(node->get_body(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitForExp(A_forExp_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_forExp_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_lo(), ctx);
        accept(node->get_hi(), ctx);

        int this_SP_counter = node->calculate_my_SP(node);
        ST<var_info> for_var_lib = ST<var_info>(node->get_var(), var_info(Ty_Int(), this_SP_counter, false));
        ST<var_info> new_local_var_lib = MergeAndShadow(for_var_lib, local_var_lib);
        ctx.local_variable_library = new_local_var_lib;

        accept(node->get_body(), ctx);


        return ST<var_info>();
    }
    ST<var_info> visitBreakExp(A_breakExp_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_breakExp_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        return ST<var_info>();
    }
    ST<var_info> visitSeqExp(A_seqExp_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_seqExp_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_seq(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitSimpleVar(A_simpleVar_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_simpleVar_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        return ST<var_info>();
    }
    ST<var_info> visitFieldVar(A_fieldVar_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_fieldVar_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_var(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitSubscriptVar(A_subscriptVar_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_subscriptVar_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_var(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitExpList(A_expList_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_expList_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_head(), ctx);
        accept(node->get_tail(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitEfield(A_efield_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_efield_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_exp(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitEfieldList(A_efieldList_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_efieldList_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        accept(node->get_head(), ctx);
        accept(node->get_tail(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitDecList(A_decList_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_decList_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        ST<var_info> head_var_lib = accept(node->get_head(), ctx);

        ST<var_info> new_local_var_lib = MergeAndShadow(head_var_lib, local_var_lib);
        ctx.local_variable_library = new_local_var_lib;
        ST<var_info> tail_var_lib = accept(node->get_tail(), ctx);

        // Return the decs that were declared 
        ST<var_info> declist_var_lib = MergeAndShadow(tail_var_lib, head_var_lib);
        return declist_var_lib;
    }
    ST<var_info> visitVarDec(A_varDec_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_varDec_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_init(), ctx);


        int my_SP = node->calculate_my_SP(node);
        ST<var_info> declared_variable_library = ST<var_info>(node->get_var(), var_info(node->get_init()->typecheck(), my_SP, true));
        return declared_variable_library;
    }
    ST<var_info> visitFunctionDec(A_functionDec_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_functionDec_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_theFunctions(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitFundecList(A_fundecList_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_fundecList_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_head(), ctx);
        accept(node->get_tail(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitFundec(A_fundec_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_fundec_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        ST<var_info> param_var_lib = accept(node->get_params(), ctx);

        ST<var_info> func_var_lib = MergeAndShadow(param_var_lib, local_var_lib);
        ctx.local_variable_library = func_var_lib;
        accept(node->get_body(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitTypeDec(A_typeDec_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_typeDec_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_theTypes(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitNametyList(A_nametyList_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_nametyList_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_head(), ctx);
        accept(node->get_tail(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitNamety(A_namety_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_namety_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_ty(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitFieldList(A_fieldList_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_fieldList_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        ST<var_info> head_var_lib = accept(node->get_head(), ctx);

        ST<var_info> new_local_var_lib = MergeAndShadow(head_var_lib, local_var_lib);
        ctx.local_variable_library = new_local_var_lib;
        ctx.field_index++;
        ST<var_info> tail_var_lib = accept(node->get_tail(), ctx);

        // Return the decs that were declared 
        ST<var_info> fieldlist_var_lib = MergeAndShadow(tail_var_lib, head_var_lib);
        return fieldlist_var_lib;
    }
    ST<var_info> visitField(A_field_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_field_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        int SP_OFFSET = 3;
        int field_index = ctx.field_index + SP_OFFSET; 
        type_info type_struct = lookup(node->get_typ(), type_library);
        Ty_ty field_type = type_struct.my_type();
        ST<var_info> field_var_lib = ST<var_info>(
            node->get_name(),
            var_info(field_type,
                           field_index,
                           true
            )
        );

        return field_var_lib;
    }
    ST<var_info> visitNameTy(A_nameTy_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_nameTy_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        return ST<var_info>();
    }
    ST<var_info> visitRecordty(A_recordty_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_recordty_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        ctx.local_variable_library = local_var_lib;
        accept(node->get_record(), ctx);

        return ST<var_info>();
    }
    ST<var_info> visitArrayty(A_arrayty_* node, VoidContext ctx) {
        EM_debug("setting variable library for A_arrayty_");
        ST<var_info> local_var_lib = ctx.local_variable_library;
        node->set_local_variable_library(local_var_lib);

        return ST<var_info>();
    }
};
#endif
