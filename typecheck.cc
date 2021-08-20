#include "util.h"
#include "AST.h"
#include "errormsg.h"
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
				A_seqExp_
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
	// Must check that types work in args and body of function
	// Then return that type
	// TODO: Deal with later when implemented tiger standard library
	return Ty_Void();
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
x
}
