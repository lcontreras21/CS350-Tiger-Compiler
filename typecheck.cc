#include "util.h"
#include "AST.h"
#include "errormsg.h"
#include "ST.h"
//#include "typecheck.h"

//
// Change this comment to describe the attributes you'll be using
//  to check types in tiger programs.
//

#define SYM_INFO_FOR_STDLIB

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


/* Methods with typecheck:
Have ** on their name
	AST_node_
		A_exp_
			A_literalExp_
				A_leafExp_
					A_nilExp_
					A_intExp_**
					A_boolExp_**
					A_stringExp_**
				A_recordExp_
				A_arrayExp_
			A_varExp_**
			A_opExp_**
			A_assignExp_**
			A_letExp_
			A_callExp_**
			A_controlExp_
				A_ifExp_**
				A_whileExp_
				A_forExp_**
				A_breakExp_
				A_seqExp_**
		A_var_
			A_simpleVar_**
			A_fieldVar_
			A_subscriptVar_
		A_dec_
			A_varDec_
			A_functionDec_**
			A_typeDec_
			A_decList
		A_fundec_**
		A_expList_**
		A_efield_
		A_efieldList_
		A_fundecList_**
		A_namety_
		A_nametyList_
		A_field_**
		A_fieldList_**
		A_ty_
			A_nameTy_
			A_arrayty_
			A_recordty_


*/

Ty_ty AST_node_::init_typecheck() {
	EM_error("Typecheck on node not yet having typecheck method");
	return Ty_Error();
}

Ty_ty A_root_::init_typecheck() {
	Ty_ty result = main_expr->typecheck();
	return result;
}

Ty_ty A_intExp_::init_typecheck() {
	return Ty_Int();
}

Ty_ty A_boolExp_::init_typecheck() {
	return Ty_Bool();
}

Ty_ty A_stringExp_::init_typecheck() {
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
			EM_warning("Left side of op does not have Ty_Int()");
			error = true;
		}
		if (right_type != Ty_Int()) {
			EM_warning("Right side of op does not have Ty_Int()");
			error = true;
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
	// have name _func and args _args
	A_expList args_exps = _args;
	// Look up name in ST tiger_library to get args as Ty_fieldList and iterate through to check if all are correct type
	
	// First Check if _func is in symbol table is there	
	if (not is_name_there(_func, tiger_library)) {
		EM_error("Function " + str(_func) + " is not in Tiger Standard Library. Define or check again");
	   return Ty_Error();
	}
	// Get Ty_Function	
	// My defined struct to store info
	function_info func_struct = lookup(_func, tiger_library);
	// Get Ty_Function
	Ty_ty func_type_info = func_struct.type_of_function;
	// Get return type of function
	Ty_ty return_type = func_type_info->u.function.return_type;
	// Get types of args in Ty_fieldList_ struct
	Ty_fieldList arg_types = func_type_info->u.function.parameter_types;
	// Simple Base case, if both are 0
	if (arg_types == 0 and args_exps == 0) {
		return return_type;
	}
	// Iterate through arg_types and args_exps (A_expList _head _tail)
	int arg_counter = 1;
	while (args_exps != 0) {
		// Base case if too little args in args_exps
		if (arg_types == 0) {
			EM_error("Function " + str(_func) + " has extra arguments. Please Check");
			return Ty_Error(); 
		}
		// Check if types are the same
		Ty_ty head_type = args_exps->_head->typecheck();
		Ty_ty expected_type = arg_types->head->ty;
		if (head_type != expected_type) {
		   	EM_error("Typechecking callExp: Arg " + std::to_string(arg_counter) + " type does not match in function " +
		   	         "call " + str(_func) + ". Got " + to_String(head_type) + " but expected " + to_String(expected_type));
			return Ty_Error();
		} 
		arg_types = arg_types->tail;
		args_exps = args_exps->_tail;
		arg_counter++;
	}
	if (args_exps == 0 and arg_types != 0) {
		// Too few arguments
		EM_error("Function " + str(_func) + " has too few arguments.");
		return Ty_Error();
	}
	// If made it this far, then all types check out
	return return_type;
}

Ty_ty A_ifExp_::init_typecheck() {
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
	// return Ty_Void if _seq is 0, else return type of last item
	Ty_ty return_type = Ty_Void();
	A_expList seq = _seq;
	if (seq == 0) {
		return return_type;
	} else {
		while (seq != 0) {
			return_type = seq->_head->typecheck();
			seq = seq->_tail;
		}
		return return_type;
	}
}

Ty_ty A_whileExp_::init_typecheck() {
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
	return Ty_Void();
}

Ty_ty A_forExp_::init_typecheck() {
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
	return _var->typecheck();
}

Ty_ty A_simpleVar_::init_typecheck() {
    EM_debug("Typechecking simpleVar: " + Symbol_to_string(_sym));

    ST<var_info> my_variable_library = get_my_variable_library(this);
    EM_debug("Typechecking simpleVar: " + Symbol_to_string(_sym) + " ST: " + my_variable_library.__repr__());
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
	Ty_ty head_type = _head->typecheck();
	if (_tail == 0) {
		return head_type;
	} else {
		return _tail->typecheck();
	}
}

int let_counter = 0;

Ty_ty A_letExp_::init_typecheck() {
    if (this->my_let_number < 0) {
        this->my_let_number = let_counter;
        let_counter++;
    }

    Ty_ty return_type;

    ST<function_info> copy_func_lib = tiger_library;

    EM_debug("Typechecking letExp #" + get_my_let_number_s() + " declarations");
    Ty_ty dec_type = _decs ? _decs->typecheck() : Ty_Void();
    if (dec_type != Ty_Error()) {
        EM_debug("Typechecking letExp #" + get_my_let_number_s() + " body");
        return_type = _body ? _body->typecheck() : Ty_Void();
    } else {
        EM_warning("Typechecking error in letExp #" + get_my_let_number_s() + " declarations");
        return_type = Ty_Error();
    }

    tiger_library = copy_func_lib;
    return return_type;
}

Ty_ty A_decList_::init_typecheck() {
    EM_debug("Typechecking decList head");

	Ty_ty head_type = _head->typecheck();
	if (_tail != 0) {
	    EM_debug("Typechecking decList tail");
		return _tail->typecheck();
	}
	return head_type;
}


Ty_ty A_varDec_::init_typecheck() {
    EM_debug("Typechecking varDec " + Symbol_to_string(_var));

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
				EM_error("Var " + Symbol_to_string(_var) + " declared type does not match the initialization type. " +
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
	// Make sure type of _exp matches type initially stored in ST?
	// Or can type info be overwritten?
	if (_exp->typecheck() != _var->typecheck()) {
		return Ty_Error();
	} else {
		return Ty_Void();
	}
}

/*
Type Checking on Functions

Mutually recursive functions are handled similarly. The first pass gathers
information about the header of each function (function name, formal pa-
rameter list, return type) but leaves the bodies of the functions untouched. 
In this pass, the types of the formal parameters are needed, but not their names (which cannot be seen from outside the function).

The second pass processes the bodies of all functions in the mutually recur-
sive declaration, taking advantage of the environment augmented with all the
function headers. For each body, the formal parameter list is processed again,
this time entering the parameters as VarEntrys in the value environment. 
*/

Ty_ty A_functionDec_::init_typecheck() {
	// Go through functions twice, for each pass
	EM_debug("Typechecking FunctionDec");

	Ty_ty firstPass = theFunctions->init_typecheck();
	Ty_ty secondPass = theFunctions->typecheck();
	if (firstPass != Ty_Error()) {
		if (secondPass != Ty_Error()) {
			// Passed All TypeChecking
			return secondPass;
		} else {
			EM_error("Typechecking error while doing the second Pass for function declarations.");
			return Ty_Error();
		} 
	} else {
		EM_error("Typechecking error while doing the first Pass for function declarations.");
		return Ty_Error();
	}
}

Ty_ty A_fundecList_::init_typecheck() {
	// Go through each fundec
	_head->typecheck();
	if (_tail != 0) {
		return _tail->init_typecheck();
	}
	return _head->init_typecheck();
}

Ty_ty A_fundec_::init_typecheck() {
	// First Pass: Add function name, params, return type to function library.	
	//		Don't type check the body
	//		Save it in private field and reset it. Use that in second pass. This is to avoid using param from another function in your own body
	// Second Pass: Check the body not that parameters have been added
	if (firstPass) {
	    EM_debug("Typechecking FunDec in first pass");
		firstPass = false;
		// Get return type from type library, if there
		if (is_name_there(_result, type_library)) {

			type_info type_struct = lookup(_result, type_library);
			Ty_ty this_type = type_struct.my_type();

			// Type check _params, adding them to var_lib
			Ty_fieldList params;
			if (_params != 0) {
				_params->typecheck();
				params = _params->init_Ty_fieldList();
			} else {
				params = 0;
			}

			ST<function_info> new_func = ST<function_info>(_name, function_info(Ty_Function(this_type, params)));	
			this_func_ST = new_func;
			tiger_library = FuseOneScope(tiger_library, new_func);
			return this_type;
		} else {
			EM_error("Function declaration " + Symbol_to_string(_name) + " does not have a valid return type");
			return Ty_Error();
		}
		return Ty_Void();
	} else {
	    EM_debug("Typechecking FunDec in second pass");
		// Second Pass
		// Load stored Symbol Table and typecheck body
		Ty_ty body_type = _body->typecheck();
		return body_type;
	}
}


Ty_ty A_fieldList_::init_typecheck() {
	// Go through each field
	Ty_ty curr_type = _head->typecheck();
	if (_tail != 0) {
		return _tail->typecheck();
	} 
	return curr_type;
}

Ty_ty A_field_::init_typecheck() {
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

Ty_fieldList A_fieldList_::init_Ty_fieldList() {
	if (_tail == 0) {
		return Ty_FieldList(_head->init_Ty_field(), 0);
	} else {
		return Ty_FieldList(_head->init_Ty_field(), _tail->init_Ty_fieldList());	
	}
}

Ty_field A_field_::init_Ty_field() {
	// Check if type _typ is in allowed or declared types
	if (is_name_there(_typ, type_library)) { 
		type_info type_struct = lookup(_typ, type_library);
		Ty_ty this_type = type_struct.my_type();
		return Ty_Field(_name, this_type);
	} else {
		EM_error("Init_Ty_field: Var " + Symbol_to_string(_name) + " in function declaration does not have type in" +
		         "type library.");
		return Ty_Field(_name, Ty_Error());
	}
}
