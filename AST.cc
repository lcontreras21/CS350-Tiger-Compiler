#include "AST.h"
#include "errormsg.h"
#include <logic.h>

// Abstract Syntax for Tiger
//
// January 2002, davew@cs.haverford.edu
//
// See AST-print.cc for printing, various other files for attributes other than pos


// First, an example to help illustrate the creation of AST nodes;
// After that come the class declarations

// To run the examples, use the command
//	g++ -std=c++0x -I/home/courses/include -D AST_EXAMPLES_IS_MAIN=1 -D ERRORMSG_SKIP_LEX=1 AST.cc AST-print.cc AST-print-attributes.cc HERA_code.cc my_reg.cc util.cc errormsg.cc -L/home/courses/lib -lcourses -o AST_examples && ./AST_examples


void AST_examples()
{
/*
  Build (using the functions from AST_appel.h and Figure 4.7) and print
  the AST corresponding to the following tiger program:

    let
	var wombat : int := 14+6
	var arthropod : int := 2
    in
	(let
	    var wombat : int := 35
	 in
	    arthropod := wombat+arthropod
	 end)
       +
	(let
	    var arthropod : int := 4
	 in
	    wombat/arthropod
	 end)
    end

    Since this isn't coming from the lexical scanner
      (the example is run before the actual compiler starts),
      the "adjust" function hasn't had a chance to record the lengths of lines,
      and we can't refer to "positions" in any useful way.

    So ... in this example, the "pos" parameter (for position)
      is just left at the initial EM_currentPos(), which will show up as line 1.
    In the compiler, you could just use EM_currentPos() for all "pos" fields as well,
      but this would end up identifying the position at the _end_ of each complete
      expression, (i.e., line 15 would be associated with the "+" on line 10 of the example).
    Better would be to associate a "pos" attribute with the PLUS token,
      and use that as the "pos" for the A_opExp_.

    The function names below are the same as those used in Appel's book.
    I usually use the most general appropriate type of pointer for the AST node produced,
      e.g. A_exp for both integers and operations, but A_dec for a vardec or declist.

    This gets taught before types, so for the moment all types are just given as "0" (null pointer).
*/

	A_exp fourteen = A_IntExp(Position::undefined(), 14);  // 14
	A_exp six      = A_IntExp(Position::undefined(), 6);  // 6
	A_exp twenty   = A_OpExp(Position::undefined(), A_plusOp, fourteen, six);  // 14+6

	A_dec wombat1  = A_VarDec(Position::undefined(), to_Symbol("wombat"), 0, twenty); // no type info given
	A_dec arth1    = A_VarDec(Position::undefined(), to_Symbol("arthropod"), 0, A_IntExp(Position::undefined(), 2));
	A_decList let1_decs = A_DecList(wombat1, A_DecList(arth1, 0));  // that's the declarations for let #1


	// *** Now the stuff for the 2nd let (the first one inside the outer let) ***
	A_dec wombat2  = A_VarDec(Position::undefined(), to_Symbol("wombat"), 0, A_IntExp(Position::undefined(), 35));
	A_decList let2_decs = A_DecList(wombat2, 0);

	// wombat + arthropod
	A_var w_var2   = A_SimpleVar(Position::undefined(), to_Symbol("wombat"));
	A_exp w_use2   = A_VarExp(Position::undefined(), w_var2);  // for the use of "wombat" in "wombat+arthropod"
	A_exp a_use2   = A_VarExp(Position::undefined(), A_SimpleVar(Position::undefined(), to_Symbol("arthropod")));
	A_exp sum2     = A_OpExp(Position::undefined(), A_plusOp, w_use2, a_use2);

	// arthropod := 
	A_var a_var2   = A_SimpleVar(Position::undefined(), to_Symbol("arthropod"));

	// arthropod := wombat + arthropod
	A_exp assign_a = A_AssignExp(Position::undefined(), a_var2, sum2);

	// now, build the node for "let2" from everything from "*** Now the stuff for the 2nd let" to here
	A_exp let2     = A_LetExp(Position::undefined(), let2_decs, assign_a);


	// *** Now the stuff for the 3rd let (the 2nd one inside the outer let)
	A_exp let3     = A_LetExp(Position::undefined(),
				  A_DecList(A_VarDec(Position::undefined(), to_Symbol("arthropod"), 0, A_IntExp(Position::undefined(), 4)),
					    0),
				  A_OpExp(Position::undefined(), A_divideOp,
					  A_VarExp(Position::undefined(), A_SimpleVar(Position::undefined(), to_Symbol("wombat"))),
					  A_VarExp(Position::undefined(), A_SimpleVar(Position::undefined(), to_Symbol("arthropod")))));

	// *** At long last, we can build that "+" that sums the two inner lets, and the main let itself:
	A_exp let1      = A_LetExp(Position::undefined(),
				   let1_decs,
				   A_OpExp(Position::undefined(), A_plusOp, let2, let3));

	A_root_ *local_AST_root = new A_root_(let1);

	// Phew. Done at last. Let's print it.

	EM_debug("Here's a simple AST for 14+6, printed with to_String");
	EM_debug(str(twenty));

	EM_debug("Now,  here's the HERA code we get at the moment for that:");
	EM_debug(twenty->HERA_code());

	EM_debug("Here's the full example AST, printed with to_String");
	EM_debug(str(local_AST_root));
}

#if defined(AST_EXAMPLES_IS_MAIN) && AST_EXAMPLES_IS_MAIN
int main()
{
	EM_reset("Examples in AST_examples() in AST.cc", -1, true);
	AST_examples();
	return 0;
}
#endif


					  
				  
// Now, the functions for the actual AST classes...

AST_node_::~AST_node_()
{
}

String to_String(AST_node_ *n)
{
	return n->print_rep(0, have_AST_attrs);
}


A_nilExp_::A_nilExp_(A_pos pos) :  A_literal_(pos)
{
}

A_intExp_::A_intExp_(A_pos pos, int i) :  A_literal_(pos), value(i)
{
}

A_stringExp_::A_stringExp_(A_pos pos, String s) :  A_literal_(pos), value(s)
{
}
A_recordExp_::A_recordExp_(A_pos pos, Symbol typ, A_efieldList fields) :  A_literal_(pos), _typ(typ), _fields(fields)
{
	precondition(typ != 0);
}

A_arrayExp_::A_arrayExp_(A_pos pos, Symbol typ, A_exp size, A_exp init) :  A_literal_(pos), _typ(typ), _size(size), _init(init)
{
	precondition(typ!=0 && size!=0 && init!=0);
}


A_varExp_::A_varExp_(A_pos pos, A_var var) :  A_exp_(pos), _var(var)
{
	precondition(var != 0);
}


A_opExp_::A_opExp_(A_pos pos, A_oper oper, A_exp left, A_exp right) :  A_exp_(pos), _oper(oper), _left(left), _right(right)
{
	precondition(left != 0 && right != 0);
}

A_assignExp_::A_assignExp_(A_pos pos, A_var var, A_exp exp) : A_exp_(pos), _var(var), _exp(exp)
{
	precondition(exp != 0 && var != 0);
}

A_letExp_::A_letExp_(A_pos pos, A_decList decs, A_exp body) :  A_exp_(pos), _decs(decs), _body(body)
{
	// Appel says body and decs can each be null
}

A_callExp_::A_callExp_(A_pos pos, Symbol func, A_expList args) :  A_exp_(pos), _func(func), _args(args)
{
	precondition(func != 0);
}

A_ifExp_::A_ifExp_(A_pos pos, A_exp test, A_exp then, A_exp elsee) :  A_controlExp_(pos), _test(test), _then(then), _elsee(elsee)
{
	precondition(test != 0 && then != 0);
}


A_forExp_::A_forExp_(A_pos pos, Symbol var, A_exp lo, A_exp hi, A_exp body) :  A_controlExp_(pos), _var(var), _lo(lo), _hi(hi), _body(body)
{
	precondition(var != 0 && lo != 0 && hi != 0 && body != 0);
}

A_breakExp_::A_breakExp_(A_pos pos) :  A_controlExp_(pos)
{
}

A_seqExp_::A_seqExp_(A_pos pos, A_expList seq) :  A_controlExp_(pos), _seq(seq)
{
}

A_simpleVar_::A_simpleVar_(A_pos pos, Symbol sym) :  A_var_(pos), _sym(sym)
{
	precondition(sym != 0);
}

A_fieldVar_::A_fieldVar_(A_pos pos, A_var var, Symbol sym) :  A_var_(pos), _var(var), _sym(sym)
{
	precondition(var != 0 && sym != 0);
}

A_subscriptVar_::A_subscriptVar_(A_pos pos, A_var var, A_exp exp) :  A_var_(pos), _var(var), _exp(exp)
{
	precondition(exp != 0 && var != 0);
}


A_expList_::A_expList_(A_exp head, A_expList tail) :  AST_node_(head->pos()), _head(head), _tail(tail)
{
	precondition(head != 0);
}
int A_expList_::length()
{
	if (_tail == 0)
		return 1;
	else
		return 1+_tail->length();
}


A_efield_::A_efield_(Symbol name, A_exp exp) :  AST_node_(exp->pos()), _name(name), _exp(exp)
{
	precondition(exp != 0);
}
String A_efield_::fieldname()
{
	return Symbol_to_string(_name);
}

A_efieldList_::A_efieldList_(A_efield head, A_efieldList tail) :  AST_node_(head->pos()), _head(head), _tail(tail)
{
	precondition(head != 0);
}


A_decList_::A_decList_(A_dec head, A_decList tail) :  A_dec_(head->pos()), _head(head), _tail(tail)
{
	precondition(head != 0);
}

A_varDec_::A_varDec_(A_pos pos, Symbol var, Symbol typ, A_exp init) :  A_dec_(pos), _var(var), _typ(typ), _init(init)
{
	precondition(var != 0 && init != 0);
}

A_fundecList_::A_fundecList_(A_fundec head, A_fundecList tail) :  A_dec_(head->pos()), _head(head), _tail(tail)
{
	precondition(head != 0);
}

A_fundec_::A_fundec_(A_pos pos, Symbol name, A_fieldList params, Symbol result,  A_exp body) :  A_dec_(pos), _name(name), _params(params), _result(result), _body(body)
{
	precondition(name != 0 && body != 0);
}


A_nametyList_::A_nametyList_(A_namety head, A_nametyList tail) :  A_dec_(head->pos()), _head(head), _tail(tail)
{
	precondition(head != 0);
}

A_namety_::A_namety_(A_pos pos, Symbol name, A_ty ty) :  A_dec_(pos), _name(name), _ty(ty)
{
	precondition(name != 0 && ty != 0);
}

A_fieldList_::A_fieldList_(A_field head, A_fieldList tail) :  A_dec_(head->pos()), _head(head), _tail(tail)
{
	precondition(head != 0);
}

A_field_::A_field_(A_pos pos, Symbol name, Symbol typ) :  A_dec_(pos), _name(name), _typ(typ)
{
	precondition(name != 0 && typ != 0);
}


A_nameTy_::A_nameTy_(A_pos pos, Symbol name) :  A_ty_(pos), _name(name)
{
	precondition(name != 0);
}

A_recordty_::A_recordty_(A_pos pos, A_fieldList record) :  A_ty_(pos), _record(record)
{
}

A_arrayty_::A_arrayty_(A_pos pos, Symbol array) :  A_ty_(pos), _array(array)
{
	precondition(array != 0);
}

A_root_ *AST_root;
bool have_AST_attrs;

void A_root_::set_parent_pointers_for_me_and_my_decendents(AST_node_ *my_parent_or_null_if_i_am_the_root)
{
	my_parent = my_parent_or_null_if_i_am_the_root;
	main_expr->set_parent_pointers_for_me_and_my_decendents(this);
}

void AST_node_::set_parent_pointers_for_me_and_my_decendents(AST_node_ *my_parent_or_null_if_i_am_the_root)
{
	my_parent = my_parent_or_null_if_i_am_the_root;
	EM_debug("Uh-oh, better set the parent pointhers for *each*class* ... there's just this hack there at the moment");
}

AST_node_ *AST_node_::parent()	// get the parent node, after the 'set parent pointers' pass
{
	assert("parent pointers have been set" && my_parent);
	return my_parent;
}

AST_node_ *A_root_::parent() {
	assert("Uh-oh ... called A_root_::parent() :-(" && false);
	return my_parent;  // shuts up compiler warnings, handles case when assertions are off
}
