#ifndef FUNCTION_LIBRARY_VISITOR_H
#define FUNCTION_LIBRARY_VISITOR_H
#include "../AST.h"
#include "visitor.h"



/*
 * NOTE: Recursive Functions and Functions Declarations that reference each other
 *
 * From Appel 'Modern Compiler Implementation in C' p.122
 * Mutually recursive functions are handled similarly. The first pass gathers information about the header of each function 
 * (function name, formal parameter list, return type) but leaves the bodies of the functions untouched.
 * In this pass, the types of the formal parameters are needed, but not their names (which cannot be seen from outside
 * the function).
 *
 * The second pass processes the bodies of all functions in the mutually recursive declaration, taking advantage of the
 * environment augmented with all the function headers. For each body, the formal parameter list is processed again,
 * this time entering the parameters as varDecs in the value environment.
*/

struct FunctionLibraryVisitor : Visitor<ST<function_info>, VoidContext> {
    ST<function_info> accept(AST_node_* node, VoidContext ctx) {
        if (node == 0) {
            return ST<function_info>();
        }
        return node->accept(*this, ctx);
    }

    ST<function_info> visitAST_node(AST_node_* node, VoidContext ctx) {
         EM_error("Not implemented");
        return ST<function_info>();
    }
    ST<function_info> visitRoot(A_root_* node, VoidContext ctx) {
        EM_debug("setting function library for A_root_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_main_expr(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitNilExp(A_nilExp_* node, VoidContext ctx) {
        EM_debug("setting function library for A_nilExp_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        return ST<function_info>();
    }
    ST<function_info> visitBoolExp(A_boolExp_* node, VoidContext ctx) {
        EM_debug("setting function library for A_boolExp_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        return ST<function_info>();
    }
    ST<function_info> visitIntExp(A_intExp_* node, VoidContext ctx) {
        EM_debug("setting function library for A_intExp_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        return ST<function_info>();
    }
    ST<function_info> visitStringExp(A_stringExp_* node, VoidContext ctx) {
        EM_debug("setting function library for A_stringExp_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        return ST<function_info>();
    }
    ST<function_info> visitRecordExp(A_recordExp_* node, VoidContext ctx) {
        EM_debug("setting function library for A_recordExp_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_fields(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitArrayExp(A_arrayExp_* node, VoidContext ctx) {
        EM_debug("setting function library for A_arrayExp_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_size(), ctx);
        accept(node->get_init(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitVarExp(A_varExp_* node, VoidContext ctx) {
        EM_debug("setting function library for A_varExp_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        return accept(node->get_var(), ctx);
    }
    ST<function_info> visitOpExp(A_opExp_* node, VoidContext ctx) {
        EM_debug("setting function library for A_opExp_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_left(), ctx);
        accept(node->get_right(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitAssignExp(A_assignExp_* node, VoidContext ctx) {
        EM_debug("setting function library for A_assignExp_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_var(), ctx);
        accept(node->get_exp(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitLetExp(A_letExp_* node, VoidContext ctx) {
        EM_debug("setting function library for A_letExp_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        ctx.curr_pass = FirstPass;
        ST<function_info> dec_func_lib = accept(node->get_decs(), ctx);

        ST<function_info> new_local_func_lib = MergeAndShadow(dec_func_lib, local_func_lib);
        ctx.local_function_library = new_local_func_lib;
        accept(node->get_body(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitCallExp(A_callExp_* node, VoidContext ctx) {
        EM_debug("setting function library for A_callExp_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_args(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitIfExp(A_ifExp_* node, VoidContext ctx) {
        EM_debug("setting function library for A_ifExp_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_test(), ctx);
        accept(node->get_then(), ctx);
        accept(node->get_else_or_null(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitWhileExp(A_whileExp_* node, VoidContext ctx) {
        EM_debug("setting function library for A_whileExp_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_test(), ctx);
        accept(node->get_body(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitForExp(A_forExp_* node, VoidContext ctx) {
        EM_debug("setting function library for A_forExp_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_lo(), ctx);
        accept(node->get_hi(), ctx);

        int this_SP_counter = node->calculate_my_SP(node);
        ST<function_info> for_func_lib = ST<function_info>(node->get_var(), function_info(Ty_Int(), this_SP_counter, false));
        ST<function_info> new_local_func_lib = MergeAndShadow(for_func_lib, local_func_lib);
        ctx.local_function_library = new_local_func_lib;

        accept(node->get_body(), ctx);


        return ST<function_info>();
    }
    ST<function_info> visitBreakExp(A_breakExp_* node, VoidContext ctx) {
        EM_debug("setting function library for A_breakExp_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        return ST<function_info>();
    }
    ST<function_info> visitSeqExp(A_seqExp_* node, VoidContext ctx) {
        EM_debug("setting function library for A_seqExp_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_seq(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitSimpleVar(A_simpleVar_* node, VoidContext ctx) {
        EM_debug("setting function library for A_simpleVar_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        return ST<function_info>();
    }
    ST<function_info> visitFieldVar(A_fieldVar_* node, VoidContext ctx) {
        EM_debug("setting function library for A_fieldVar_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_var(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitSubscriptVar(A_subscriptVar_* node, VoidContext ctx) {
        EM_debug("setting function library for A_subscriptVar_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_var(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitExpList(A_expList_* node, VoidContext ctx) {
        EM_debug("setting function library for A_expList_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_head(), ctx);
        accept(node->get_tail(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitEfield(A_efield_* node, VoidContext ctx) {
        EM_debug("setting function library for A_efield_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_exp(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitEfieldList(A_efieldList_* node, VoidContext ctx) {
        EM_debug("setting function library for A_efieldList_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_head(), ctx);
        accept(node->get_tail(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitDecList(A_decList_* node, VoidContext ctx) {
        EM_debug("setting function library for A_decList_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        ST<function_info> head_func_lib = accept(node->get_head(), ctx);

        ST<function_info> new_local_func_lib = MergeAndShadow(head_func_lib, local_func_lib);
        ctx.local_function_library = new_local_func_lib;
        ST<function_info> tail_func_lib = accept(node->get_tail(), ctx);

        // Return the decs that were declared 
        ST<function_info> declist_func_lib = MergeAndShadow(tail_func_lib, head_func_lib);
        return declist_func_lib;
    }
    ST<function_info> visitVarDec(A_varDec_* node, VoidContext ctx) {
        EM_debug("setting function library for A_varDec_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_init(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitFunctionDec(A_functionDec_* node, VoidContext ctx) {
        EM_debug("setting function library for A_functionDec_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        // First Pass (Appel p.122)
        ST<function_info> declared_func_lib = accept(node->get_theFunctions(), ctx);

        // Second Pass
        ctx.curr_pass = SecondPass;
        ST<function_info> new_local_func_lib = MergeAndShadow(declared_func_lib, local_func_lib);
        ctx.local_function_library = new_local_func_lib;
        accept(node->get_theFunctions(), ctx);

        return declared_func_lib;
    }

    ST<function_info> visitFundecList(A_fundecList_* node, VoidContext ctx) {
        string msg = "setting function library for A_fundecList_ ";
        msg = msg + (ctx.curr_pass == 0 ? "First Pass" : "Second Pass");
        EM_debug(msg);

        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        ST<function_info> head_func_lib = accept(node->get_head(), ctx);

        ST<function_info> new_local_func_lib;
        if (ctx.curr_pass == FirstPass) {
            new_local_func_lib = MergeAndShadow(head_func_lib, local_func_lib);
        } else {
            new_local_func_lib = local_func_lib;
        }
        ctx.local_function_library = new_local_func_lib;
        ST<function_info> tail_func_lib = accept(node->get_tail(), ctx);

        // Return the decs that were declared 
        ST<function_info> fundeclist_func_lib;
        if (ctx.curr_pass == FirstPass) {
            fundeclist_func_lib = MergeAndShadow(tail_func_lib, head_func_lib);
        }
        return fundeclist_func_lib;
    }
    ST<function_info> visitFundec(A_fundec_* node, VoidContext ctx) {
        string msg = "setting function library for A_fundec_ ";
        msg = msg + (ctx.curr_pass == 0 ? "First Pass" : "Second Pass");
        EM_debug(msg);
        ST<function_info> local_func_lib = ctx.local_function_library;

        accept(node->get_params(), ctx);
        if (ctx.curr_pass == SecondPass) {
            node->set_local_function_library(local_func_lib);
            accept(node->get_body(), ctx);
            return ST<function_info>();
        }

        Ty_ty my_return_type = 0;
        if (is_name_there(node->get_result(), type_library)) {
            type_info type_struct = lookup(node->get_result(), type_library);
            my_return_type = type_struct.my_type();
        }

        Ty_fieldList param_types = get_ty_fieldlist(node->cast_params());
        function_count++;
        bool is_tiger_function = false;
        ST<function_info> declared_func_lib = ST<function_info>(
            node->get_name(),
            function_info(
                Ty_Function(
                    my_return_type,
                    param_types
                ),
                function_count,
                is_tiger_function
            )
        );

        ST<function_info> fundec_func_lib = MergeAndShadow(declared_func_lib, local_func_lib);
        node->set_local_function_library(fundec_func_lib);

        return declared_func_lib;
    }

    Ty_fieldList get_ty_fieldlist(A_fieldList_* params) {
        if (params == 0) {
            return 0;
        }
        Ty_field head_field = get_ty_field(params->cast_head());
        Ty_fieldList tail_fields = get_ty_fieldlist(params->cast_tail());

		return Ty_FieldList(head_field, tail_fields);	
    }

    Ty_field get_ty_field(A_field_* param) {
        if (is_name_there(param->get_typ(), type_library)) { 
            type_info type_struct = lookup(param->get_typ(), type_library);
            Ty_ty this_type = type_struct.my_type();
            return Ty_Field(param->get_name(), this_type);
        } else {
            EM_warning("Init_Ty_field: Var " + Symbol_to_string(param->get_name()) + " in function declaration does not have type in" +
                     "type library.");
            return Ty_Field(param->get_name(), Ty_Error());
        }
    }

    ST<function_info> visitTypeDec(A_typeDec_* node, VoidContext ctx) {
        EM_debug("setting function library for A_typeDec_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_theTypes(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitNametyList(A_nametyList_* node, VoidContext ctx) {
        EM_debug("setting function library for A_nametyList_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_head(), ctx);
        accept(node->get_tail(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitNamety(A_namety_* node, VoidContext ctx) {
        EM_debug("setting function library for A_namety_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_ty(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitFieldList(A_fieldList_* node, VoidContext ctx) {
        EM_debug("setting function library for A_fieldList_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        ST<function_info> head_func_lib = accept(node->get_head(), ctx);

        ST<function_info> new_local_func_lib = MergeAndShadow(head_func_lib, local_func_lib);
        ctx.local_function_library = new_local_func_lib;
        ctx.field_index++;
        ST<function_info> tail_func_lib = accept(node->get_tail(), ctx);

        // Return the decs that were declared 
        ST<function_info> fieldlist_func_lib = MergeAndShadow(tail_func_lib, head_func_lib);
        return fieldlist_func_lib;
    }
    ST<function_info> visitField(A_field_* node, VoidContext ctx) {
        EM_debug("setting function library for A_field_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        int SP_OFFSET = 3;
        int field_index = ctx.field_index + SP_OFFSET; 
        type_info type_struct = lookup(node->get_typ(), type_library);
        Ty_ty field_type = type_struct.my_type();
        ST<function_info> field_func_lib = ST<function_info>(
            node->get_name(),
            function_info(field_type,
                           field_index,
                           true
            )
        );

        return field_func_lib;
    }
    ST<function_info> visitNameTy(A_nameTy_* node, VoidContext ctx) {
        EM_debug("setting function library for A_nameTy_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        return ST<function_info>();
    }
    ST<function_info> visitRecordty(A_recordty_* node, VoidContext ctx) {
        EM_debug("setting function library for A_recordty_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        accept(node->get_record(), ctx);

        return ST<function_info>();
    }
    ST<function_info> visitArrayty(A_arrayty_* node, VoidContext ctx) {
        EM_debug("setting function library for A_arrayty_");
        ST<function_info> local_func_lib = ctx.local_function_library;
        node->set_local_function_library(local_func_lib);

        return ST<function_info>();
    }
};
#endif
