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
			A_varExp_
			A_opExp_**
			A_assignExp_
			A_letExp_
			A_callExp_**
			A_controlExp_
				A_ifExp_**
				A_whileExp_
				A_forExp_
				A_breakExp_
				A_seqExp_**
		A_var_
			A_simpleVar_
			A_fieldVar_
			A_subscriptVar_
		A_dec_
			A_varDec_
			A_functionDec_
			A_typeDec_
			A_decList
			A_functionDec_
		A_fundec_
		A_expList_**
		A_efield_
		A_efieldList_
		A_fundecList
		A_namety_
		A_nametyList_
		A_field_
		A_fieldList_
		A_ty_
			A_nameTy_
			A_arrayty_
			A_recordty_


*/


// Get Tiger Standard Library to use in typechecking
ST<type_info> tiger_library = get_tiger_lib();

Ty_ty AST_node_::typecheck() {
	EM_error("Typecheck on node not yet having typecheck method");
	return Ty_Error();
}

Ty_ty A_root_::typecheck() {
	return main_expr->typecheck();
}

Ty_ty A_intExp_::typecheck() {
	return Ty_Int();
}

Ty_ty A_boolExp_::typecheck() {
	return Ty_Bool();
}

Ty_ty A_stringExp_::typecheck() {
	return Ty_String();
}

Ty_ty A_opExp_::typecheck() {
	// Check types on _left and _right
	string error = "";
	if (_left->typecheck() != Ty_Int()) {
		error = error + "opExp has type error on left expression\n";
	}
	if (_right->typecheck() != Ty_Int()) {
		error = error + "opExp has type error on right  expression\n";
	}
	if (error != "") {
		EM_error(error);
		return Ty_Error();
	} else {
		return Ty_Int();
	}

}

Ty_ty A_callExp_::typecheck() {	
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
	type_info func_struct = lookup(_func, tiger_library);
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
		if (args_exps->_head->typecheck() != arg_types->head->ty) {
		   	EM_error("Arg " + std::to_string(arg_counter) + " type does not match");
			return Ty_Error();
		} 
		arg_types = arg_types->tail;
		args_exps = args_exps->_tail;
		arg_counter++;
	}
	// If made it this far, then all types check out
	return return_type;
}

Ty_ty A_expList_::func_typecheck(int n) {
	// Check each type works.
	// Return 
	if (_tail == 0) {
		return _head->typecheck();
	} else {
		if (_head->typecheck() != Ty_Error()) {
			return _tail->func_typecheck(n+1);
		} else {
			EM_error("Type error on paramater " + std::to_string(n));
			return Ty_Error();
		}	
	}
}

Ty_ty A_ifExp_::typecheck() {
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

Ty_ty A_seqExp_::typecheck() {
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
