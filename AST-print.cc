#include "util.h"
#include "AST.h"
#include "symbol.h"
#include "errormsg.h"
#include <logic.h>

/*
  This file contains the basic AST printing stuff;
  you shouldn't need to modify it unless you change the structure of the AST,
  e.g. by adding a new type of node.

  To change the attribute printing system, see AST-print-attributes.cc
 */



bool print_ASTs_with_attributes = false;  // can be overridden in main

static String print_rep(Symbol &s)
{
	return "to_Symbol(\"" + (Symbol_to_string(s)) + "\")";
}

// array of token number / token name pairs
static struct {
	A_oper op;
	const char *name;
} opnames[] = {
	{A_plusOp, "A_plusOp"},
	{A_minusOp, "A_minusOp"},
	{A_timesOp, "A_timesOp"},
	{A_divideOp, "A_divideOp"},
	{A_eqOp, "A_eqOp"},
	{A_neqOp, "A_neqOp"},
	{A_ltOp, "A_ltOp"},
	{A_leOp, "A_leOp"},
	{A_gtOp, "A_gtOp"},
	{A_geOp, "A_geOp"}
};

// function to convert token numbers to printable names
String print_rep(const A_oper &o) {
	for (unsigned int i = 0; i < sizeof(opnames)/sizeof(*opnames); i++)
	{
		if (opnames[i].op == o)
			return opnames[i].name;
	}
	EM_error("Internal compiler bug detected in opnames array in AST.cc", true, Position::undefined());
	return "Bug detected in opnames array in AST.cc";
}

static String print_pos_rep(A_pos pos, String possible_comma)
{
	if (AST_print_positions) {
		return str(pos) + possible_comma;  // print in a way that could be read back in, not a clear way...
	} else {
		return "";
	}
}

static const int tab = 2;  // how much more indentation as we go "into" structure
static String linebreak(const int indent)
{
	String it = "\n";
	for (int i = 0; i<indent; i++)
		it = it + " ";
	return it;
}

static String print_rep_or_zero(AST_node_ *maybe_null_pointer, const int indent, bool with_attributes)
{
	if (maybe_null_pointer == 0)
		return "0";
	else
		return linebreak(indent) + maybe_null_pointer->print_rep(indent, with_attributes);
}

static String print_rep_or_zero(Symbol maybe_null_symbol)
{
	if (maybe_null_symbol == 0)
		return "0";
	else
		return ::print_rep(maybe_null_symbol);
}


// turn s into C++ comment if it isn't just spaces and/or tabs
// first, see if it's just spaces and/or tabs
static bool contains_only_blanks(const String &s)
{
	for (unsigned int i=0; i<s.length(); i++) {
		if (s[i] != ' ' || s[i+1]!='\t') {
			return false;
		}
	}
	return true;
}
// then, make sure it isn't already a comment -- change */ into ./
static String anticomment(String s)
{
	for (unsigned int i=0; i<s.length()-1; i++) {
		if (s[i] == '*' && s[i+1]=='/') {
			s[i] = '.';
		}
	}
	return s;
}
static String as_comment(const String &s)
{
	if (contains_only_blanks(s)) {
		return s;
	} else {
		return "/* " + anticomment(s) + " */ ";
	}
}

/*
 * And now, the actual printing functions for the AST node types...
 */

string A_root_::print_rep(int indent, bool with_attributes)
{
	return "new A_root_(" + main_expr->print_rep(indent, with_attributes) + ")";
}
String A_nilExp_::print_rep(int indent, bool with_attributes)
{
	return "A_NilExp(" + print_pos_rep(pos(),"") + (with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_boolExp_::print_rep(int indent, bool with_attributes)
{
	return "A_BoolExp(" + print_pos_rep(pos(), ", ") + repr(value) + (with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_intExp_::print_rep(int indent, bool with_attributes)
{
	return "A_IntExp(" + print_pos_rep(pos(), ", ") + repr(value) + (with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_stringExp_::print_rep(int indent, bool with_attributes)
{
	return ("A_StringExp(" + print_pos_rep(pos(), ", ") +
		repr(value) +
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")");
}
String A_recordExp_::print_rep(int indent, bool with_attributes)
{
	return "A_RecordExp(" + print_pos_rep(pos(), ", ") + ::print_rep(_typ) + ", " + 
		print_rep_or_zero(_fields, indent+tab, with_attributes) + (with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_arrayExp_::print_rep(int indent, bool with_attributes)
{
	return "A_ArrayExp(" + print_pos_rep(pos(), ", ") + ::print_rep(_typ) + ", " + 
		linebreak(indent+tab) + _size->print_rep(indent+tab, with_attributes) + ", " + 
		linebreak(indent+tab) + _init->print_rep(indent+tab, with_attributes) +
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_varExp_::print_rep(int indent, bool with_attributes)
{
	return "A_VarExp(" + print_pos_rep(pos(), ", ") + 
		linebreak(indent+tab) + _var->print_rep(indent+tab, with_attributes) +
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_opExp_::print_rep(int indent, bool with_attributes) 
{
	return "A_OpExp(" + print_pos_rep(pos(), ", ") + ::print_rep(_oper) + ", " + 
		linebreak(indent+tab) + _left->print_rep(indent+tab, with_attributes) + ", " + 
		linebreak(indent+tab) + _right->print_rep(indent+tab, with_attributes) +
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_assignExp_::print_rep(int indent, bool with_attributes)
{
	return "A_AssignExp(" + print_pos_rep(pos(), ", ") + 
		linebreak(indent+tab) + _var->print_rep(indent+tab, with_attributes) + ", " + 
		linebreak(indent+tab) + _exp->print_rep(indent+tab, with_attributes) +
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_letExp_::print_rep(int indent, bool with_attributes)
{
	return "A_LetExp(" + print_pos_rep(pos(), ", ") +
		print_rep_or_zero(_decs, indent+tab, with_attributes) + ", " + 
		print_rep_or_zero(_body, indent+tab, with_attributes) +
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_callExp_::print_rep(int indent, bool with_attributes)
{
	return "A_CallExp(" + print_pos_rep(pos(), ", ") + ::print_rep(_func) + ", " +
		print_rep_or_zero(_args,indent+tab, with_attributes) +
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_ifExp_::print_rep(int indent, bool with_attributes)
{
	return "A_IfExp(" + print_pos_rep(pos(), ", ") + 
		linebreak(indent+tab) + _test->print_rep(indent+tab, with_attributes) + ", " + 
		linebreak(indent+tab) + _then->print_rep(indent+tab, with_attributes) + ", " +
		print_rep_or_zero(_else_or_null, indent+tab, with_attributes) +
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_forExp_::print_rep(int indent, bool with_attributes)
{
	return "A_ForExp(" + print_pos_rep(pos(), ", ") + ::print_rep(_var) + ", " + 
		linebreak(indent+tab) + _lo->print_rep(indent+tab, with_attributes) + ", " + 
		linebreak(indent+tab) + _hi->print_rep(indent+tab, with_attributes) + ", " + 
		linebreak(indent+tab) + _body->print_rep(indent+tab, with_attributes) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_breakExp_::print_rep(int indent, bool with_attributes)
{
	return "A_BreakExp(" + print_pos_rep(pos(), "") + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_seqExp_::print_rep(int indent, bool with_attributes)
{
	return "A_SeqExp(" + print_pos_rep(pos(), ", ") +
		print_rep_or_zero(_seq, indent+tab, with_attributes) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_simpleVar_::print_rep(int indent, bool with_attributes)
{
	return "A_SimpleVar(" + print_pos_rep(pos(), ", ") + ::print_rep(_sym) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_fieldVar_::print_rep(int indent, bool with_attributes)
{
	return "A_FieldVar(" + print_pos_rep(pos(), ", ") + 
		linebreak(indent+tab) + _var->print_rep(indent+tab, with_attributes) + ", " +
		linebreak(indent+tab) + ::print_rep(_sym) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_subscriptVar_::print_rep(int indent, bool with_attributes)
{
	return "A_SubscriptVar(" + print_pos_rep(pos(), ", ") + 
		linebreak(indent+tab) + _var->print_rep(indent+tab, with_attributes) + ", " + 
		linebreak(indent+tab) + _exp->print_rep(indent+tab, with_attributes) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_expList_::print_rep(int indent, bool with_attributes)
{
	return "A_ExpList(" + 
		linebreak(indent+tab) + _head->print_rep(indent+tab, with_attributes) + ", " + 
		print_rep_or_zero(_tail, indent+tab, with_attributes) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_efield_::print_rep(int indent, bool with_attributes)
{
	return "A_Efield(" + ::print_rep(_name) + ", " + 
		linebreak(indent+tab) + _exp->print_rep(indent+tab, with_attributes) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_efieldList_::print_rep(int indent, bool with_attributes)
{
	return "A_EfieldList(" + 
		linebreak(indent+tab) + _head->print_rep(indent+tab, with_attributes) + ", " + 
		print_rep_or_zero(_tail, indent+tab, with_attributes) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_decList_::print_rep(int indent, bool with_attributes)
{
	return "A_DecList(" + 
		linebreak(indent+tab) + _head->print_rep(indent+tab, with_attributes) + ", " + 
		print_rep_or_zero(_tail, indent+tab, with_attributes) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_varDec_::print_rep(int indent, bool with_attributes)
{
	return "A_VarDec(" + print_pos_rep(pos(), ", ") + ::print_rep(_var) + ", " + print_rep_or_zero(_typ) + ", " + 
		linebreak(indent+tab) + _init->print_rep(indent+tab, with_attributes) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}

String A_functionDec_::print_rep(int indent, bool with_attributes)
{
	return ("A_FunctionDec("+ print_pos_rep(pos(), ", ") +
		print_rep_or_zero(theFunctions, indent, with_attributes));
}

String A_fundecList_::print_rep(int indent, bool with_attributes)
{
	return "A_FundecList(" + 
		linebreak(indent+tab) + _head->print_rep(indent+tab, with_attributes) + ", " +
		print_rep_or_zero(_tail, indent+tab, with_attributes) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_fundec_::print_rep(int indent, bool with_attributes)
{
	return "A_Fundec(" + print_pos_rep(pos(), ", ") + ::print_rep(_name) + ", " +
		print_rep_or_zero(_params, indent+tab, with_attributes) + ", " +
		linebreak(indent+tab) + print_rep_or_zero(_result) + ", " + 
		linebreak(indent+tab) + _body->print_rep(indent+tab, with_attributes) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}

String A_typeDec_::print_rep(int indent, bool with_attributes)
{
	return ("A_TypeDec("+ print_pos_rep(pos(), ", ") +
		print_rep_or_zero(theTypes, indent, with_attributes));
}
String A_nametyList_::print_rep(int indent, bool with_attributes)
{
	return "A_NametyList(" + 
		linebreak(indent+tab) + _head->print_rep(indent+tab, with_attributes) + ", " + 
		print_rep_or_zero(_tail, indent+tab, with_attributes) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_namety_::print_rep(int indent, bool with_attributes)
{
	return "A_Namety(" + ::print_rep(_name) + ", " + 
		linebreak(indent+tab) + _ty->print_rep(indent+tab, with_attributes) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_fieldList_::print_rep(int indent, bool with_attributes)
{
	return "A_FieldList(" + 
		linebreak(indent+tab) + _head->print_rep(indent+tab, with_attributes) + ", " + 
		print_rep_or_zero(_tail, indent+tab, with_attributes) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_field_::print_rep(int indent, bool with_attributes)
{
	return "A_Field(" + print_pos_rep(pos(), ", ") + ::print_rep(_name) + ", " + ::print_rep(_typ) +
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}

String A_nameTy_::print_rep(int indent, bool with_attributes)
{
	return "A_NameTy(" + print_pos_rep(pos(), ", ") + ::print_rep(_name) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_recordty_::print_rep(int indent, bool with_attributes)
{
	return "A_RecordTy(" + print_pos_rep(pos(), ", ") +
		print_rep_or_zero(_record, indent+tab, with_attributes) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"") + ")";
}
String A_arrayty_::print_rep(int indent, bool with_attributes)
{
	return "A_ArrayTy(" + print_pos_rep(pos(), ", ") + ::print_rep(_array) + 
		(with_attributes?linebreak(indent+tab)+as_comment(attributes_for_printing()):"")+ ")";
}

