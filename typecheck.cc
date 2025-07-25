#include "types.h"
#include "util.h"
#include "AST.h"
#include "ST.h"

// void typecheck(AST_node_ *root)
// {
	// This function should take care of doing type checking,
	//  assuming "root" is the root of a tiger AST.
	// It should call on some functions to define the attributes
	//  related to type checking (and, in so doing, produce any necessary error messages).
	
// }

// The bodies of other type checking functions,
//  including any virtual functions you introduce into
//  the AST classes, should go here.

Ty_ty AST_node_::init_typecheck() {
	EM_error("Typecheck on node not yet having typecheck method");
	return Ty_Error();
}

Ty_ty A_root_::init_typecheck() {
    EM_debug("typechecking for A_root_");
	Ty_ty result = main_expr->typecheck();
	return result;
}

Ty_ty A_intExp_::init_typecheck() {
    EM_debug("typechecking for A_intExp_");
	return Ty_Int();
}

Ty_ty A_boolExp_::init_typecheck() {
    EM_debug("typechecking for A_boolExp_");
	return Ty_Bool();
}

Ty_ty A_stringExp_::init_typecheck() {
    EM_debug("typechecking for A_stringExp_");
	return Ty_String();
}

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

Ty_ty A_opExp_::init_typecheck() {
    EM_debug("typechecking for A_opExp_");
	Ty_ty left_type = _left->typecheck();
	Ty_ty right_type = _right->typecheck();
	Ty_ty return_type = check_return_type(_oper);
	// If return type is Ty_Bool (comparison Op), just make sure left/right types are the same
	// Otherwise, type is Ty_Int (arithmetic op), make sure left/right are type int ( maybe Ty_bool?)

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

Ty_ty A_callExp_::init_typecheck() {	
    EM_debug("typechecking for A_callExp_");
	// have name _func and args _args
	// Look up name in ST function_library to get args as Ty_fieldList and iterate
	// through to check if all are correct type
	
	// First Check if _func is in symbol table is there
	ST<function_info> parent_function_library = local_function_library;
	if (not is_name_there(_func, parent_function_library)) {
		EM_error("Function " + str(_func) + " is not in Tiger Standard Library. Define or check again");
	   return Ty_Error();
	}
	// Get Ty_Function	
	// My defined struct to store info
	function_info func_struct = lookup(_func, parent_function_library);
	// Get Ty_Function
	// Get return type of function
	Ty_ty return_type = func_struct.my_return_type();
	// Get types of args in Ty_fieldList_ struct
	Ty_fieldList arg_types = func_struct.my_args();
	// Simple Base case, if both are 0
	if (arg_types == 0 and _args == 0) {
		return return_type;
	} else if (arg_types != 0 and _args == 0) {
        // No args but require args
        EM_error("Function " + str(_func) + " has too few arguments.");
        return Ty_Error();
    }

    Ty_ty args_typecheck = _args->compare_types(_func, 1, arg_types);
    if (args_typecheck == Ty_Error()) {
        return Ty_Error();
    }

	// If made it this far, then all types check out
	return return_type;
}

Ty_ty A_expList_::compare_types(Symbol _func, int arg_counter, Ty_fieldList expected_types) {
    if (expected_types == 0) {
        EM_error("Function " + str(_func) + " has extra arguments. Please Check");
        return Ty_Error(); 
    }

    Ty_ty head_type = _head->typecheck();
    Ty_ty expected_type = expected_types->head->ty;
    if (head_type != expected_type) {
        EM_error("Typechecking callExp: Arg " + std::to_string(arg_counter) + " type does not match in function "
           + "call " + str(_func) + ". Got " + to_String(head_type) + " but expected " + to_String(expected_type));
        return Ty_Error();
    } 

    if (expected_types) {
        expected_types = expected_types->tail;
    }
    
    if (_tail) {
        return _tail->compare_types(_func, arg_counter + 1, expected_types);
    }
    if (_tail == 0 && expected_types != 0) {
        EM_error("Function " + str(_func) + " has too few arguments.");
        return Ty_Error();
    }
    return Ty_Void();
}

Ty_ty A_ifExp_::init_typecheck() {
    EM_debug("typechecking for A_ifExp_");
	// exp1 is typed as an integer, exp2 and exp3 must have the same type which will be the type of the entire structure. The resulting type cannot be that of nil. 
	// if Test is type bool that is also allowed, since it evaluates to nonzero or zero
	// First check if the test is type int
	if (_test->typecheck() != Ty_Int()) {
		if (_test->typecheck() != Ty_Bool()) {
			EM_error("Type of test expression must be Ty_Int()");
			return Ty_Error();
		}
	} 
	// Then check if it is an IF-THEN-ELSE versus IF-THEN
	if (_else_or_null != 0) {
		// In IF-THEN-ELSE, both then and else must be the same type,
	    // 	and return that type
		if (_then->typecheck() == _else_or_null->typecheck()) {
			return _then->typecheck();
		} else {
			EM_error("Type of then and else statement must be the same");
			return Ty_Error();
		}
	} else {
		// In IF-THEN, the then must be void, and return void
		if (_then->typecheck() != Ty_Void()) {
			EM_error("IF-THEN statement must have THEN statement be void");
			return Ty_Error();
		} else {
			return Ty_Void();
		}
	}
}

Ty_ty A_seqExp_::init_typecheck() {
    EM_debug("typechecking for A_seqExp_");
	// return Ty_Void if _seq is 0, else return type of last item
	Ty_ty return_type = Ty_Void();
	A_expList seq = _seq;
	if (seq == 0) {
		return return_type;
	} else {
		return _seq->typecheck();
	}
}

Ty_ty A_whileExp_::init_typecheck() {
    EM_debug("typechecking for A_whileExp_");
	// _test must be type int
	// _body must be type void
	if (_test->typecheck() == Ty_Int() || _test->typecheck() == Ty_Bool()) {
		if (_body->typecheck() == Ty_Void()) {
			return Ty_Void();
		} else {
			EM_error("WHILE expression body must be void");
			return Ty_Error();
		}
	} else {
		EM_error("WHILE expression test must be type int");
		return Ty_Error();
	}
}

Ty_ty A_breakExp_::init_typecheck() {
    EM_debug("typechecking for A_breakExp_");
	return Ty_Void();
}

Ty_ty A_forExp_::init_typecheck() {
    EM_debug("typechecking for A_forExp_");
	// _lo, _hi must be Ty_Int()
	// _body must return Ty_Void()
	Ty_ty _body_type = _body->typecheck();
	if (_lo->typecheck() != Ty_Int()) {
		EM_error("For expression must have type int in lower bound");
		return Ty_Error();
	} else if (_hi->typecheck() != Ty_Int()) {
		EM_error("For expression must have type int in upper bound");
		return Ty_Error();
	} else if (_body_type != Ty_Void()) {
		EM_error("For expression must have type void in body bound");
		return Ty_Error();
	}
	return Ty_Void();
}

Ty_ty A_varExp_::init_typecheck() {
    EM_debug("typechecking for A_varExp_");
	return _var->typecheck();
}

Ty_ty A_simpleVar_::init_typecheck() {
    EM_debug("typechecking for A_simpleVar: " + Symbol_to_string(_sym));

    ST<var_info> my_variable_library = local_variable_library;
	// Lookup in symbol type what the stored type is
	// If not in symbol table, return Ty_Error
	if (is_name_there(_sym, my_variable_library)) {
		var_info var_struct = lookup(_sym, my_variable_library);
		return var_struct.my_type();
	} else {
		EM_error("Typecheck simpleVar: Variable " + Symbol_to_string(_sym) + " has not been declared");
		return Ty_Error();
	}
}

Ty_ty A_expList_::init_typecheck() {
    EM_debug("typechecking for A_expList_");
	Ty_ty head_type = _head->typecheck();
	if (_tail == 0) {
		return head_type;
	} else {
		return _tail->typecheck();
	}
}

int let_counter = 0;

Ty_ty A_letExp_::init_typecheck() {
    EM_debug("typechecking for A_letExp_");
    // TODO Move this to a function call
    if (this->my_let_number < 0) {
        this->my_let_number = let_counter;
        let_counter++;
    }

    Ty_ty return_type;
    EM_debug("Typechecking letExp #" + get_my_let_number_s() + " declarations");
    Ty_ty dec_type = _decs ? _decs->typecheck() : Ty_Void();
    if (dec_type != Ty_Error()) {
        EM_debug("Typechecking letExp #" + get_my_let_number_s() + " body");
        return_type = _body ? _body->typecheck() : Ty_Void();
    } else {
        EM_warning("Typechecking error in letExp #" + get_my_let_number_s() + " declarations");
        return_type = Ty_Error();
    }

    return return_type;
}

Ty_ty A_decList_::init_typecheck() {
    EM_debug("typechecking for A_decList_");

	Ty_ty head_type = _head->typecheck();
	if (_tail != 0) {
	    EM_debug("Typechecking decList tail");
		return _tail->typecheck();
	}
	return head_type;
}


Ty_ty A_varDec_::init_typecheck() {
    EM_debug("typechecking for A_varDec " + Symbol_to_string(_var));

	// If no _typ available, return type of _init
	Ty_ty implicit_type = _init->typecheck();

	if (Symbols_are_equal(_typ, to_Symbol("NA"))) {
		return implicit_type;
	} else {
		// Lookup declared type in type_library
		if (is_name_there(_typ, type_library)) {
			type_info type_struct = lookup(_typ, type_library);
			Ty_ty return_type = type_struct.my_type();
			if (return_type != implicit_type) {
				EM_error("Var " + Symbol_to_string(_var) + " declared type does not match the initialization type, got: " + repr(implicit_type) +
				         "Not adding to Variable Symbol Table.");
				return Ty_Error();
			} else {
				return return_type;
			}
		} else {
			EM_error("Var " + Symbol_to_string(_var) + " declared type is not in Type Library");
			return Ty_Error();
		}
	}

}

Ty_ty A_assignExp_::init_typecheck() {
    EM_debug("typechecking for A_assignExp_");
	// Make sure type of _exp matches type initially stored in ST?
	// Or can type info be overwritten?
	if (_exp->typecheck() != _var->typecheck()) {
		return Ty_Error();
	} else {
		return Ty_Void();
	}
}

Ty_ty A_functionDec_::init_typecheck() {
    EM_debug("typechecking for A_functionDec_");
    return theFunctions->typecheck();
}

Ty_ty A_fundecList_::init_typecheck() {
    EM_debug("typechecking for A_fundecList_");
	// Go through each fundec
	_head->typecheck();
	if (_tail != 0) {
		return _tail->typecheck();
	}
	return _head->typecheck();
}

Ty_ty A_fundec_::init_typecheck() {
    EM_debug("typechecking for A_fundec_ '" + Symbol_to_string(_name) + "' params");
    if (_params) {
        Ty_ty param_type = _params->typecheck();
        if (param_type == Ty_Error()) {
            return Ty_Error();
        }
    }
    
    EM_debug("Typechecking fundec '" + Symbol_to_string(_name) + "' return type matches body of fundec");
    // Assert that the body of the function matches the return type stored in the function library
    Ty_ty my_return_type_expected = Ty_Void();
    if (is_name_there(_result, type_library)) {
        type_info type_struct = lookup(_result, type_library);
        Ty_ty this_type = type_struct.my_type();
        my_return_type_expected = this_type;
    } else {
        EM_error("Var " + Symbol_to_string(_name) + " in function declaration does not have type in type library.");
    }
    Ty_ty my_return_type_actual = _body->typecheck();

    if (my_return_type_actual == my_return_type_expected) {
        return my_return_type_actual;
    } else {
        EM_error("Typechecking fundec '" + Symbol_to_string(_name) + "' return type given does not match actual return type");
        return Ty_Error();
    }
}


Ty_ty A_fieldList_::init_typecheck() {
    EM_debug("typechecking for A_fieldList_");
	// Go through each field
	Ty_ty curr_type = _head->typecheck();
	if (_tail != 0) {
		return _tail->typecheck();
	} 
	return curr_type;
}

Ty_ty A_field_::init_typecheck() {
    EM_debug("typechecking for A_field_");
	// Have to be added to Symbol Table on First Pass but not on second
	if (firstPass) {
		firstPass = false;
		// Check if type _typ is in allowed or declared types
		if (is_name_there(_typ, type_library)) { 
			type_info type_struct = lookup(_typ, type_library);
			Ty_ty this_type = type_struct.my_type();
			return this_type;
		} else {
			EM_error("Var " + Symbol_to_string(_name) + " in function declaration does not have type in type library.");
			return Ty_Void();
		}	
	} else {
		return Ty_Void();
	}
}
