// Functions to make code written to work with Appel's code work with C++ classes.
//	Copyright (c) 1998 Andrew W. Appel
//	Copyright (c) 2002 David Wonnacott
// Generally this is just #include'd from AST.h, not #included by itself

inline A_exp A_NilExp(A_pos pos)
{
	return new A_nilExp_(pos);
}
inline A_exp A_BoolExp(A_pos pos, bool init)
{
	return new A_boolExp_(pos, init);
}
inline A_exp A_IntExp(A_pos pos, int i)
{
	return new A_intExp_(pos, i);
}
inline A_exp A_StringExp(A_pos pos, String s)
{
	return new A_stringExp_(pos, s);
}
inline A_exp A_RecordExp(A_pos pos, Symbol typ, A_efieldList fields)
{
	return new A_recordExp_(pos, typ, fields);
}
inline A_exp A_ArrayExp(A_pos pos, Symbol typ, A_exp size, A_exp init)
{
	return new A_arrayExp_(pos, typ, size, init);
}
inline A_exp A_VarExp(A_pos pos, A_var var)
{
	return new A_varExp_(pos, var);
}
inline A_exp A_OpExp(A_pos pos, A_oper oper, A_exp left, A_exp right)
{
	return new A_opExp_(pos, oper, left, right);
}
inline A_exp A_AssignExp(A_pos pos, A_var var, A_exp exp)
{
	return new A_assignExp_(pos, var, exp);
}
inline A_exp A_LetExp(A_pos pos, A_decList decs, A_exp body)
{
	return new A_letExp_(pos, decs, body);
}
inline A_exp A_CallExp(A_pos pos, Symbol func, A_expList args)
{
	return new A_callExp_(pos, func, args);
}
inline A_exp A_IfExp(A_pos pos, A_exp test, A_exp then, A_exp else_or_0_pointer_for_no_else)
{
	return new A_ifExp_(pos, test, then, else_or_0_pointer_for_no_else);
}
/*
UNCOMMENT THE FUNCTION BELOW TO HAVE A_WhileExp THATS COMPATIBLE WITH APPEL's BOOK
Note that you'll also need to have the A_whileExp_ class, of course...

inline A_exp A_WhileExp(A_pos pos, A_exp test, A_exp body)
{
	return new A_whileExp_(pos, test, body);
}
*/
inline A_exp A_ForExp(A_pos pos, Symbol var, A_exp lo, A_exp hi, A_exp body)
{
	return new A_forExp_(pos, var, lo, hi, body);
}
inline A_exp A_BreakExp(A_pos pos)
{
	return new A_breakExp_(pos);
}
inline A_exp A_SeqExp(A_pos pos, A_expList seq)
{
	return new A_seqExp_(pos, seq);
}

inline A_var A_SimpleVar(A_pos pos, Symbol sym)
{
	return new A_simpleVar_(pos, sym);
}
inline A_var A_FieldVar(A_pos pos, A_var var, Symbol sym)
{
	return new A_fieldVar_(pos, var, sym);
}
inline A_var A_SubscriptVar(A_pos pos, A_var var, A_exp exp)
{
	return new A_subscriptVar_(pos, var, exp);
}

inline A_expList A_ExpList(A_exp head, A_expList tail)
{
	return new A_expList_(head, tail);
}
inline A_efield A_Efield(Symbol name, A_exp exp)
{
	return new A_efield_(name, exp);
}
inline A_efieldList A_EfieldList(A_efield head, A_efieldList tail)
{
	return new A_efieldList_(head, tail);
}


// Declarationlists, and the things that live in them...
inline A_decList A_DecList(A_dec head, A_decList tail)
{
	return new A_decList_(head, tail);
}
inline A_dec A_VarDec(A_pos pos, Symbol var, Symbol typ, A_exp init)
{
	return new A_varDec_(pos, var, typ, init);
}

inline A_dec A_FunctionDec(A_pos pos, A_fundecList functions_that_might_call_each_other)
{
	return new A_functionDec_(pos, functions_that_might_call_each_other);
}
inline A_fundecList A_FundecList(A_fundec head, A_fundecList tail)
{
	return new A_fundecList_(head, tail);
}
inline A_fundec A_Fundec(A_pos pos, Symbol name, A_fieldList params, Symbol result_type_or_0_pointer_for_no_result_type_in_declaration,  A_exp body)
{
	return new A_fundec_(pos, name, params, result_type_or_0_pointer_for_no_result_type_in_declaration, body);
}

inline A_dec A_TypeDec(A_pos pos, A_nametyList types_that_might_refer_to_each_other)
{
	return new A_typeDec_(pos, types_that_might_refer_to_each_other);
}
inline A_nametyList A_NametyList(A_namety head, A_nametyList tail)
{
	return new A_nametyList_(head, tail);
}
inline A_namety A_Namety(Symbol name, A_ty ty)
{
	return new A_namety_(ty->pos(), name, ty);
}


// Types, which can be used in the declarations of variables:
inline A_ty A_RecordTy(A_pos pos, A_fieldList record)
{
	return new A_recordty_(pos, record);
}
inline A_ty A_ArrayTy(A_pos pos, Symbol array)
{
	return new A_arrayty_(pos, array);
}
//  Using the name of a type to declare a variable with NameTy -- this is a use of a type
inline A_ty A_NameTy(A_pos pos, Symbol name)
{
	return new A_nameTy_(pos, name);
}

inline A_fieldList A_FieldList(A_field head, A_fieldList tail)
{
	return new A_fieldList_(head, tail);
}
inline A_field A_Field(A_pos pos, Symbol name, Symbol type_or_0_pointer_for_no_type_in_declaration)
{
	return new A_field_(pos, name, type_or_0_pointer_for_no_type_in_declaration);
}


// Not part of Appel.s absyn.h, but still here for consistency:
inline A_root_ *A_RootExp(A_exp the_program)
{
	return new A_root_(the_program);
}
