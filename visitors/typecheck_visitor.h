#ifndef TYPECHECK_VISITOR_H
#define TYPECHECK_VISITOR_H
#include "../AST.h"
#include "visitor.h"


static Ty_ty check_return_type(A_oper op) {
	if (op == A_plusOp || op == A_minusOp || op == A_timesOp) {
		return Ty_Int();
	} else if (op == A_eqOp || op == A_neqOp || op == A_leOp || op == A_ltOp
			|| op == A_geOp || op == A_gtOp) {
		return Ty_Bool();
	} else {
		return Ty_Error();
	}
}


struct TypecheckLibraryVisitor : Visitor<Ty_ty, VoidContext> {
    Ty_ty accept(AST_node_* node, VoidContext ctx) {
        Ty_ty final_type;
        if (node == 0) {
            final_type = Ty_Void();
        } else {
            final_type = node->accept(*this, ctx);
            node->set_local_type(final_type);
        }

        return final_type;
    }

    Ty_ty visitAST_node(AST_node_* node, VoidContext ctx) {
         EM_error("Not implemented");
        return Ty_Void();
    }
    Ty_ty visitRoot(A_root_* node, VoidContext ctx) {
        EM_debug("typechecking A_root_");

        Ty_ty expr_type = accept(node->get_main_expr(), ctx);
        node->set_local_type(expr_type);
        return expr_type;
    }
    Ty_ty visitNilExp(A_nilExp_* node, VoidContext ctx) {
        EM_debug("typechecking A_nilExp_");

        return Ty_Void();
    }
    Ty_ty visitBoolExp(A_boolExp_* node, VoidContext ctx) {
        EM_debug("typechecking A_boolExp_");

        return Ty_Bool();
    }
    Ty_ty visitIntExp(A_intExp_* node, VoidContext ctx) {
        EM_debug("typechecking A_intExp_");

        return Ty_Int();
    }
    Ty_ty visitStringExp(A_stringExp_* node, VoidContext ctx) {
        EM_debug("typechecking A_stringExp_");
        
        return Ty_String();
    }
    Ty_ty visitRecordExp(A_recordExp_* node, VoidContext ctx) {
        EM_debug("typechecking A_recordExp_");

        // NOTE: not implemented yet
        accept(node->get_fields(), ctx);

        return Ty_Void();
    }
    Ty_ty visitArrayExp(A_arrayExp_* node, VoidContext ctx) {
        EM_debug("typechecking A_arrayExp_");

        // NOTE: not implemented yet
        accept(node->get_size(), ctx);
        accept(node->get_init(), ctx);

        return Ty_Void();
    }
    Ty_ty visitVarExp(A_varExp_* node, VoidContext ctx) {
        EM_debug("typechecking A_varExp_");

        return accept(node->get_var(), ctx);
    }
    Ty_ty visitOpExp(A_opExp_* node, VoidContext ctx) {
        EM_debug("typechecking A_opExp_");

        Ty_ty left_type = accept(node->get_left(), ctx);
        Ty_ty right_type = accept(node->get_right(), ctx);
        Ty_ty return_type = check_return_type(node->get_oper());


        if (return_type == Ty_Bool()) {
            if (left_type == right_type) {
                return return_type;
            } else {
                EM_error("Comparison operator does not have left and right being the same type");
                return Ty_Error();
            }	
        } else if (return_type == Ty_Int()) {
            bool error = false;
            if (left_type != Ty_Int()) {
                if (left_type == Ty_Bool()) {
                    EM_warning("Left side of op has type `Boolean`, still works but flagging");
                } else {
                    EM_warning("Left side of op does not have valid type");
                    error = true;
                }
            }
            if (right_type != Ty_Int()) {
                if (right_type == Ty_Bool()) {
                    EM_warning("Right side of op has type `Boolean`, still works but flagging");
                } else {
                    EM_warning("Right side of op does not have valid type");
                    error = true;
                }
            }
            if (error) {
                EM_error("Type error in op expression");
                return Ty_Error();
            } else {
                return return_type;
            }
        } else {
            EM_error("Make sure you have used the correct operator");
            return Ty_Error();
        }
    }
    Ty_ty visitAssignExp(A_assignExp_* node, VoidContext ctx) {
        EM_debug("typechecking A_assignExp_");

        Ty_ty var_type = accept(node->get_var(), ctx);
        Ty_ty exp_type = accept(node->get_exp(), ctx);
        if (var_type != exp_type) {
            return Ty_Error();
        }
        if (var_type == Ty_Error() || exp_type == Ty_Error()) {
            return Ty_Error();
        }

        return Ty_Void();
    }
    Ty_ty visitLetExp(A_letExp_* node, VoidContext ctx) {
        EM_debug("typechecking A_letExp_");

        Ty_ty decs_type = accept(node->get_decs(), ctx);
        Ty_ty body_type = accept(node->get_body(), ctx);

        if (decs_type == Ty_Error()) {
            return Ty_Error();
        }

        return body_type;
    }
    Ty_ty visitCallExp(A_callExp_* node, VoidContext ctx) {
        EM_debug("typechecking A_callExp_");
        ctx.func_call_args = 0;

        ST<function_info> parent_function_library = node->get_local_function_library();
        if (not is_name_there(node->get_func(), parent_function_library)) {
            EM_error("Function " + str(node->get_func()) + " is not in Tiger Standard Library. Define or check again");
            return Ty_Error();
        }

        function_info func_struct = lookup(node->get_func(), parent_function_library);
        Ty_ty return_type = func_struct.my_return_type();
        Ty_fieldList arg_types = func_struct.my_args();

        if (arg_types == 0 and node->get_args() == 0) {
            return return_type;
        } else if (arg_types != 0 and node->get_args() == 0) {
            EM_error("Function " + str(node->get_func()) + " has too few arguments.");
            return Ty_Error();
        }

        ctx.func_call_args = arg_types;
        ctx.arg_counter = 0;
        ctx.for_callExp = true;
        ctx.callExp_name = str(node->get_func());
        Ty_ty args_type = accept(node->get_args(), ctx);
        if (args_type == Ty_Error()) {
            return Ty_Error();
        }

        return return_type;
    }
    Ty_ty visitIfExp(A_ifExp_* node, VoidContext ctx) {
        EM_debug("typechecking A_ifExp_");

        Ty_ty test_type = accept(node->get_test(), ctx);

        if (test_type != Ty_Int() && test_type != Ty_Bool()) {
            EM_error("Type of test expression must be Int or Bool");
            return Ty_Error();
        } 

        Ty_ty else_or_null_type = accept(node->get_else_or_null(), ctx);
        Ty_ty then_type = accept(node->get_then(), ctx);
        // Then check if it is an IF-THEN-ELSE versus IF-THEN
        if (node->get_else_or_null() != 0) {
            // In IF-THEN-ELSE, both then and else must be the same type,
            // 	and return that type
            if (else_or_null_type != then_type) {
                EM_error("Type of then and else statement must be the same");
                return Ty_Error();
            }

            return then_type;
        } else {
            // In IF-THEN, the then must be void, and return void
            if (then_type != Ty_Void()) {
                EM_error("IF-THEN statement must have THEN statement be void");
                return Ty_Error();
            }
            return Ty_Void();
        }

    }
    Ty_ty visitWhileExp(A_whileExp_* node, VoidContext ctx) {
        EM_debug("typechecking A_whileExp_");

        Ty_ty test_type = accept(node->get_test(), ctx);
        if (test_type != Ty_Int() && test_type != Ty_Bool()) {
            EM_error("WHILE expression test must be type int");
            return Ty_Error();
        }

        Ty_ty body_type = accept(node->get_body(), ctx);
        if (body_type != Ty_Void()) {
            EM_error("WHILE expression body must be void");
            return Ty_Error();
        }

        return Ty_Void();
    }
    Ty_ty visitForExp(A_forExp_* node, VoidContext ctx) {
        EM_debug("typechecking A_forExp_");

        Ty_ty lo_type = accept(node->get_lo(), ctx);
        Ty_ty hi_type = accept(node->get_hi(), ctx);
        Ty_ty _body_type = accept(node->get_body(), ctx);
        if (lo_type != Ty_Int()) {
            EM_error("For expression must have type int in lower bound");
            return Ty_Error();
        } else if (hi_type != Ty_Int()) {
            EM_error("For expression must have type int in upper bound");
            return Ty_Error();
        } else if (_body_type != Ty_Void()) {
            EM_error("For expression must have type void in body bound");
            return Ty_Error();
        }

        return Ty_Void();
    }
    Ty_ty visitBreakExp(A_breakExp_* node, VoidContext ctx) {
        EM_debug("typechecking A_breakExp_");

        return Ty_Void();
    }
    Ty_ty visitSeqExp(A_seqExp_* node, VoidContext ctx) {
        EM_debug("typechecking A_seqExp_");

        return accept(node->get_seq(), ctx);
    }
    Ty_ty visitSimpleVar(A_simpleVar_* node, VoidContext ctx) {
        EM_debug("typechecking A_simpleVar_");

        ST<var_info> local_var_lib = node->get_local_variable_library();
        // Lookup in symbol type what the stored type is
        // If not in symbol table, return Ty_Error
        if (is_name_there(node->get_sym(), local_var_lib)) {
            var_info var_struct = lookup(node->get_sym(), local_var_lib);
            return var_struct.my_type();
        } else {
            EM_error("Typecheck simpleVar: Variable " + Symbol_to_string(node->get_sym()) + " has not been declared");
            return Ty_Error();
        }
    }
    Ty_ty visitFieldVar(A_fieldVar_* node, VoidContext ctx) {
        EM_debug("typechecking A_fieldVar_");
        // NOTE: not implemented yet

        accept(node->get_var(), ctx);

        return Ty_Void();
    }
    Ty_ty visitSubscriptVar(A_subscriptVar_* node, VoidContext ctx) {
        EM_debug("typechecking A_subscriptVar_");
        // NOTE: not implemented yet

        accept(node->get_var(), ctx);

        return Ty_Void();
    }
    Ty_ty visitExpList_for_callExp(A_expList_* node, VoidContext ctx) {
        EM_debug("\ttypechecking v.callExp A_expList_");
        if (ctx.func_call_args == 0) {
            EM_error("Function has extra arguments. Please Check");
            return Ty_Error(); 
        }

        VoidContext head_ctx;
        Ty_ty head_type = accept(node->get_head(), head_ctx);
        Ty_ty expected_type = ctx.func_call_args->head->ty;
        if (head_type != expected_type) {
            EM_error("Typechecking callExp: Arg " + std::to_string(ctx.arg_counter + 1) + " type does not match in function "
               + "call " + ctx.callExp_name + ". Got " + to_String(head_type) + " but expected " + to_String(expected_type));
            return Ty_Error();
        } 

        if (ctx.func_call_args) {
            ctx.func_call_args = ctx.func_call_args->tail;
        }
        
        if (node->get_tail()) {
            ctx.arg_counter++;
            return accept(node->get_tail(), ctx);
        }
        if (node->get_tail() == 0 && ctx.func_call_args != 0) {
            EM_error("Function has too few arguments.");
            return Ty_Error();
        }

        return Ty_Void();
    }
    Ty_ty visitExpList_for_others(A_expList_* node, VoidContext ctx) {
        EM_debug("\ttypechecking v.others A_expList_");
        Ty_ty head_type = accept(node->get_head(), ctx);
        if (head_type == Ty_Error()) {
            return Ty_Error();
        }
        Ty_ty tail_type = accept(node->get_tail(), ctx);
        if (tail_type == Ty_Error()) {
            return Ty_Error();
        }
        if (node->get_tail() == 0) {
            return head_type;
        } else {
            return tail_type;
        }
    }
    Ty_ty visitExpList(A_expList_* node, VoidContext ctx) {
        EM_debug("typechecking A_expList_");
        if (ctx.for_callExp) {
            return visitExpList_for_callExp(node, ctx);
        } else {
            return visitExpList_for_others(node, ctx);
        }
    }
    Ty_ty visitEfield(A_efield_* node, VoidContext ctx) {
        EM_debug("typechecking A_efield_");
        // NOTE: not implemented yet

        accept(node->get_exp(), ctx);

        return Ty_Void();
    }
    Ty_ty visitEfieldList(A_efieldList_* node, VoidContext ctx) {
        EM_debug("typechecking A_efieldList_");
        // NOTE: not implemented yet

        accept(node->get_head(), ctx);
        accept(node->get_tail(), ctx);

        return Ty_Void();
    }
    Ty_ty visitDecList(A_decList_* node, VoidContext ctx) {
        EM_debug("typechecking A_decList_");

        Ty_ty head_type = accept(node->get_head(), ctx);

        if (node->get_tail() == 0) {
            return head_type;
        }
        return accept(node->get_tail(), ctx);
    }
    Ty_ty visitVarDec(A_varDec_* node, VoidContext ctx) {
        EM_debug("typechecking A_varDec_");

        Ty_ty init_type = accept(node->get_init(), ctx);

        if (Symbols_are_equal(node->get_typ(), to_Symbol("NA"))) {
            // If no declared type available, return type of implicit type 
            return init_type;
        } 

        if (! is_name_there(node->get_typ(), type_library)) {
            EM_error("Var " + Symbol_to_string(node->get_var()) + " declared type is not in Type Library");
            return Ty_Error();
        }

        // Lookup declared type in type_library
        type_info type_struct = lookup(node->get_typ(), type_library);
        Ty_ty return_type = type_struct.my_type();
        if (return_type != init_type) {
            EM_error("Var " + Symbol_to_string(node->get_var()) + " declared type does not match the initialization type, got: " + repr(init_type));
            return Ty_Error();
        }

        return return_type;
    }
    Ty_ty visitFunctionDec(A_functionDec_* node, VoidContext ctx) {
        EM_debug("typechecking A_functionDec_");

        return accept(node->get_theFunctions(), ctx);
    }

    Ty_ty visitFundecList(A_fundecList_* node, VoidContext ctx) {
        string msg = "typechecking A_fundecList_ ";
        msg = msg + (ctx.curr_pass == 0 ? "First Pass" : "Second Pass");
        EM_debug(msg);

        Ty_ty head_type = accept(node->get_head(), ctx);
        if (head_type == Ty_Error()) {
            return Ty_Error();
        }
        Ty_ty tail_type = accept(node->get_tail(), ctx);
        if (tail_type == Ty_Error()) {
            return Ty_Error();
        }

        return Ty_Void();
    }
    Ty_ty visitFundec(A_fundec_* node, VoidContext ctx) {
        string msg = "typechecking A_fundec_ ";
        msg = msg + (ctx.curr_pass == 0 ? "First Pass" : "Second Pass");
        EM_debug(msg);
        
        Ty_ty params_type = accept(node->get_params(), ctx);
        if (params_type == Ty_Error()) {
            return Ty_Error();
        }

        // Assert that the body of the function matches the return type stored in the function library
        if (!is_name_there(node->get_result(), type_library)) {
            EM_error("Var " + Symbol_to_string(node->get_name()) + " in function declaration does not have type in type library.");
            return Ty_Error();
        }

        type_info type_struct = lookup(node->get_result(), type_library);
        Ty_ty this_type = type_struct.my_type();
        Ty_ty my_return_type_expected = this_type;

        Ty_ty body_type = accept(node->get_body(), ctx);
        if (body_type != my_return_type_expected) {
            EM_error("Typechecking fundec '" + Symbol_to_string(node->get_name()) + "' return type given does not match actual return type");
            return Ty_Error();
        }

        return body_type;
    }
    Ty_ty visitTypeDec(A_typeDec_* node, VoidContext ctx) {
        EM_debug("typechecking A_typeDec_");
        // NOTE: not implemented yet

        accept(node->get_theTypes(), ctx);

        return Ty_Void();
    }
    Ty_ty visitNametyList(A_nametyList_* node, VoidContext ctx) {
        EM_debug("typechecking A_nametyList_");
        // NOTE: not implemented yet

        accept(node->get_head(), ctx);
        accept(node->get_tail(), ctx);

        return Ty_Void();
    }
    Ty_ty visitNamety(A_namety_* node, VoidContext ctx) {
        EM_debug("typechecking A_namety_");
        // NOTE: not implemented yet

        accept(node->get_ty(), ctx);

        return Ty_Void();
    }
    Ty_ty visitFieldList(A_fieldList_* node, VoidContext ctx) {
        EM_debug("typechecking A_fieldList_");
        
        Ty_ty head_type = accept(node->get_head(), ctx);
        if (head_type == Ty_Error()) {
            return Ty_Error();
        }
        Ty_ty tail_type = accept(node->get_tail(), ctx);
        if (tail_type == Ty_Error()) {
            return Ty_Error();
        }

        return Ty_Void();
    }
    Ty_ty visitField(A_field_* node, VoidContext ctx) {
        EM_debug("typechecking A_field_");

		if (!is_name_there(node->get_typ(), type_library)) { 
			EM_error("Var " + Symbol_to_string(node->get_name()) + " in function declaration does not have type in type library.");
			return Ty_Void();
		}	

        type_info type_struct = lookup(node->get_typ(), type_library);
        Ty_ty this_type = type_struct.my_type();
        return this_type;
    }
    Ty_ty visitNameTy(A_nameTy_* node, VoidContext ctx) {
        EM_debug("typechecking A_nameTy_");
        // NOTE: not implemented yet

        return Ty_Void();
    }
    Ty_ty visitRecordty(A_recordty_* node, VoidContext ctx) {
        EM_debug("typechecking A_recordty_");
        // NOTE: not implemented yet

        accept(node->get_record(), ctx);

        return Ty_Void();
    }
    Ty_ty visitArrayty(A_arrayty_* node, VoidContext ctx) {
        EM_debug("typechecking A_arrayty_");
        // NOTE: not implemented yet

        return Ty_Void();
    }
};
#endif
