// Abstract Syntax for Tiger
//
// C++ Version adapted from Andrew Appel's files by davew@cs.haverford.edu
//
// Copyright (c) 1998 Andrew W. Appel
// Copyright (c) 2002,2004,2006, etc. David Wonnacott
//
// This is a collection of classes that are designed to be
//  used in an abstract syntax tree for the tiger language.
//
// These classes are a hierarchy of types of nodes in the AST.
// There is no "tree" class to form a "wrapper" for pointers
//  to these nodes, in part because we need to connect with
//  bison, and need to have a naked pointer (rather than a
//  class with a constructor) in the %union statement, and
//  we make to attempt to free up memory (this is left to
//  the user of the class or anyone working on conservative
//  garbage collection in C++).
//
// The hierarchy is given below.  All leaf classes can be
//  allocated using the function names given by appel (see AST_appel.h).
//
// The current hierarchy is directly based on Appel's structures.
// You are allowed to rearrange it if you wish, but retain all
//  the existing leaf classes.
//
// Printing can be done with or without the "position" information.
//  With this information,
//	the output should be suitable for cut and paste into source files
//  Without it, it is easier to compare the output of two different compilers.
//  This option is controlled by the boolean constant at the end of this file.
//
// CHANGES FROM APPEL'S STUFF:
//  * the "A_stringExp" has a C++ string, not a C-style char *
//  * all AST nodes now have a "pos" associated with them
//
// HEIRARCHY OF AST NODE TYPES:
//
//	AST_node_
//		A_exp_
//			A_literalExp_
//				A_leafExp_
//					A_nilExp_
//					A_intExp_
//					A_boolExp_
//					A_stringExp_
//				A_recordExp_
//				A_arrayExp_
//			A_varExp_	(variable use, e.g. "d" in the expression "d+1", but not "a" in "a := 2")
//			A_opExp_	(-, +, *, /, =, >=, ...)
//			A_assignExp_
//			A_letExp_
//			A_callExp_
//			A_controlExp_
//				A_ifExp_
//				A_whileExp_
//				A_forExp_
//				A_breakExp_
//				A_seqExp_	(Like {} In C++)
//
//  NOTE: A_var (below) is used in assignment operations and A_VarExp nodes,
//   but NOT used in declarations, i.e., the header of A_ForExp or A_VarDec
//
//		A_var_              (various lvalues, i.e., occurrances of variables on the _left_ of a :=, like the following)
//			A_simpleVar_    (simple variable, e.g. "a" in a := 2 or "b" in b[3] := 4 or "c" in r.real := 5)
//			A_fieldVar_     (a field of a record, e.g. "c.real" in c.real := 5)
//			A_subscriptVar_ (a subscripted array, e.g. "b[3]" in b[3] := 4
//
//  See the end of Appel's Chapter 4, and the examples in AST.cc, for information about declarations; these are important in "let"
//		A_dec_ ... one _unit_ of scope's declaration(s), specifically
//			A_varDec_		(Variable Declaration, e.g. var a : int := 42)
//			A_functionDec_		(A COLLECTION OF Potentially-Recursive Functions, e.g. function f(i: int): int = 2*g(i-1) function g(a: int) = if a<1 then 1 else f(x-1)+1)
//			A_typeDec_		(A COLLECTION OF Potentially-Recursive Types, e.g., type trees = array of tree    type tree  = {val: string, kids: trees} )
//
//		// In modern C++, the following would just be list<whatever>, but Appel's C code defines them each a types ... FEEL FREE TO UPDATE THIS IF YOU WISH
//		    A_expList_		(e.g., List Of Function Arguments In A Call, Or Of Expressions In A Sequence)
//		    (an A_expList has A_exp entries, as above)
//		    A_decList_		(List Of A_dec: -- this is what we find in a "let"
//		    (has A_dec, as above)
//		    A_efieldList_	(List Of Fields In A RecordExp, e.g. within the { } of point {x=12, y=30} )
//		    (has A_efield_'s)	(One Field In A RecordExp, e.g. the "x=12" above)
//		    A_fundecList_	(List Of Function Declarations, as below ... this exists only within A_FunctionDec)
//		    (has A_fundec_'s)	(One Function Declaration With Parameters And Body, e.g., function answer(): int = 42)
//		    A_nametyList_	(List Of Type Declarations, as below ... this exists only within A_TypeDec)
//		    (has A_namety_'s)	(Single Type Name Declaration, with the type it names, e.g. the connection of the name "point" to the type it names in type point = {x: int, y:int})
//		    A_fieldList_	(List Of Function Parameters Or Record Fields in a declaration)
//		    (has A_field_'s)	(One Function Parameter Or Record Field in a declaration, e.g.,the "x: int" in the type 'point' above, or as a function parameter)
//		A_ty_			(Type Declarations, used only when we're naming a type via a_namety (note lower-case t), i.e. after the "=" in type t = ...)
//			A_arrayTy_	(Array  type, e.g., "array of int" in type ia = array of int)
//			A_recordTy_	(Record type, e.g., "{x: int, y:int}" in type point = {x: int, y:int})
//			A_nameTy_	(a type that's been named already, e.g., "point" in type point2d = point)
//
//  It is ok to use a null pointer (i.e. 0) as a parameter in the following
//
//   - any "list" parameter
//   - the "else" statement in an if/then/else
//   - the "typ" parameter when creating an A_VarDec
//
//
//  Q: What's the deal with fundec, and fundecList, and functionDec?
//     (and likewise for namety, nametyList, and typeDec?)
//
//  A: These are used to collect/organize declarations of functions (and likewise types).
//     Each individual function declaration, e.g. "function two(): int = 2" should be
//      represented as an A_fundec_ (note the parameters to the A_Fundec constructor match the example above).
//     However, we don't want each _individual_ declaration to appear in the list of declarations
//      below a "let", because we can simplify the process of following Tiger's scope rules
//      by collecting a set of potentially-mutually-recursive functions (consecutive functions
//      without type or variable declarations between them). The name for this collection is
//      A_fundecList_, since it's just a list of A_fundec_'s. The A_functionDec_ identifies the top of
//	an A_fundecList_, and lets you perform various functions there by overloading that function
//	for A_functionDec_ to be different from A_varDec_ (or A_fundecList_).
//     Likewise for A_namety_ (a type declaration in a let), A_nametyList (a list of A_namety_), and A_typeDec_.
//      But, note nameTy is entirely different from namety.
//
//
//  Q: OK, so what are namety and nameTy and why does capitalization matter so much?
//
//  A: Because types are given names, and then these names can be used as types.
//     namety is used to give a name to a type, in a type declaration, as in
//          type point = { x: int, y: int }  /* This line creates one namety */
//          type nums  = array of int        /* This line creates one namety */
//          type also_point = point          /* This line creates one namety */
//           /* the three above go into a nametylist */
//
//     nameTy is used to identify an existing type by name.
//      note that there are three ways to provide a type on the right hand side of the namety,
//      as shown on the three examples above. They all produce some sort of A_ty node, of which
//      there are three kinds:
//          a recordTy node is used for a record type, as in the { x: int, y: int} above;
//          an arrayTy node is used for an array type, as in the "array of int" above
//          a nameTy   node is used to identify an existing type by name, as in "point" on the third line.
//
//     As usual, the class name starts with "A_" and ends with "_" ;
//     starting with a capital gets you Appel's constructor,
//     and for namety there is a helpful abbreviation for the pointer type that is needed all over the place
//      (like A_exp is a helpful abbreviation for A_exp_ *).
//     To give more detail:
//
//       Giving a name to a type with Namety (e.g. type point =  { x : int, y : int }):
//        A_namety	a pointer type, for a node in the AST such as the node declaring "point" (above)
//        A_namety_	the node type pointed to by A_namety, e.g. the node declaring point
//        A_Namety	a function producing a A_namety from a name and a type, to create the node above
//
//       Using the name of a type to declare a variable with NameTy (e.g. "point" in "var  p : point := ...")
//        A_nameTy_	a node representing the use of a type name
//        A_NameTy	a function producing a type from a name,
//       			 for use where a name is used as a type
//
//      In an attempt to reduce confusion, I pronounce nameTy "name tie", and namety "nam-itty"
//       (In the latter, it's as if the "e" were in the 2nd syllable.)
//

// **** NOTE: If Eclipse just shows one line of comment above this, click the circled "+" to the left to open the main comment ******


#if ! defined _AST_H
#define _AST_H

#include "errormsg.h"
typedef Position A_pos;
#include "symbol.h"
#include "types.h"  // we'll need this for attributes


void AST_examples();  // Examples, to help understand what't going on here ... see AST.cc


/*
  Following the notation/techniques used in Appel'c C code,
  we'll use type names like A_exp and A_field for _pointers_ to objects on the free-store heap.
  I've introduced C++ classes instead of Appel's C structs, and use an "_" at the end of each class name.
 */

typedef class A_var_ *A_var;
typedef class A_exp_ *A_exp;
typedef class A_dec_ *A_dec;
typedef class A_ty_ *A_ty;

typedef class A_decList_ *A_decList;
typedef class A_expList_ *A_expList;
typedef class A_field_ *A_field;
typedef class A_fieldList_ *A_fieldList;
typedef class A_fundec_ *A_fundec;
typedef class A_fundecList_ *A_fundecList;
typedef class A_namety_ *A_namety;
typedef class A_nametyList_ *A_nametyList;
typedef class A_efield_ *A_efield;
typedef class A_efieldList_ *A_efieldList;

// #include "AST_attribute_types.h"   // This is for the old (non-lazy) attribute system

extern bool print_ASTs_with_attributes;  // defaults to false; can be overridden in main with "-A" option


class AST_node_ {  // abstract class with some common data
public:
	AST_node_(A_pos pos);
	virtual ~AST_node_();

	A_pos pos() { return stored_pos; }

	// Each node will know its parent, except the root node (on which this is an error):
	virtual AST_node_ *parent();	// get the parent node, after the 'set all parent nodes' pass
	// Those parent pointers are set by the set_parent... function below,
	//    WHICH MUST ONLY BE CALLED FROM THE ROOT CONSTRUCTOR AND THEN RECURSIVELY
	// (I don't know how to say that in C++ without a zillion "friend" definitions, though.)
	virtual void set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent);

	virtual string print_rep(int indent, bool with_attributes) = 0;
	virtual String attributes_for_printing();
	string __repr__() { return this->print_rep(0, print_ASTs_with_attributes); }  // allow repr(x), which is more familiar to Python programmers ... see also util.h
	string __str__()  { return this->__repr__(); }

	// Shadow the default EM_error, etc., so that it automatically uses this->pos by default
	void EM_error  (string message, bool fatal=false) {   ::EM_error(message, fatal, this->pos()); }
	void EM_warning(string message, bool fatal=false) { ::EM_warning(message, this->pos()); }
	void EM_debug  (string message, bool fatal=false) {   ::EM_debug(message, this->pos()); }

	
	// And now, the attributes that exist in ALL kinds of AST nodes.
	//  See Design_Documents/AST_Attributes.txt for details.
	virtual string HERA_code();  // defaults to a warning, with HERA code that would error if compiled; could be "=0" in final compiler
	virtual string HERA_data();  // defaults to empty string 
	virtual Ty_ty typecheck();	// Perform typechecking all along the tree 
	int height();  // example we'll play with in class, not actually needed to compile
	virtual int compute_height();  // just for an example, not needed to compile
	int depth();   // example we'll play with in class, not actually needed to compile
	virtual int compute_depth();   // just for an example, not needed to compile

protected:  // so that derived class's set_parent should be able to get at stored_parent for "this" object ... Smalltalk allows this by default
	AST_node_ *stored_parent = 0;

private:
	virtual AST_node_ *get_parent_without_checking();	// NOT FOR GENERAL USE: get the parent node, either before or after the 'set all parent nodes' pass, but note it will be incorrect if done before (this is usually just done for assertions)
	A_pos stored_pos;
};

class A_exp_ : public AST_node_ {
public:
	A_exp_(A_pos p);

	// Attributes for all expressions: result_reg() is the register number to use;
	//  in the first call, it is defined by the init_result_reg for the class,
	//  and then reused each time we ask for it.
	int    result_reg() {
		if (this->stored_result_reg < 0) this->stored_result_reg = this->init_result_reg();
		return stored_result_reg;
	}
	string result_reg_s() { // return in string form, e.g. "R2"
		return "R" + std::to_string(this->result_reg());
	}
	virtual int init_result_reg();

	// we'll need to print the register number attribute for exp's
	virtual String attributes_for_printing();

private:
	int stored_result_reg = -1;  // Initialize to -1 to be sure it gets replaced by "if" in result_reg() above
};

class A_root_ : public AST_node_ {
public:
	A_root_(A_exp main_exp);
	A_exp *main();

	string HERA_code();
	string HERA_data();
	Ty_ty typecheck();
	AST_node_ *parent();	// We should never call this
	string print_rep(int indent, bool with_attributes);

	virtual void set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent);  // should not be called, since it's in-line in the constructor
	virtual int compute_depth();  // just for an example, not needed to compile
private:

	A_exp main_expr;
};


class A_literalExp_ : public A_exp_ {
public:
	A_literalExp_(A_pos p);
};

class A_leafExp_ : public A_literalExp_ {
public:
	A_leafExp_(A_pos p);

	void set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent);  // this one's easy, by definition :-)
	virtual int compute_height();  // just for an example, not needed to compile
	virtual int init_result_reg();
};

class A_nilExp_ : public A_leafExp_ {
public:
	A_nilExp_(A_pos p);
	virtual string print_rep(int indent, bool with_attributes);
};


class A_boolExp_ : public A_leafExp_ {
public:
	A_boolExp_(A_pos pos, bool b);
	virtual string print_rep(int indent, bool with_attributes);
	virtual string HERA_code();
	Ty_ty typecheck();
private:
  bool value;
};

class A_intExp_ : public A_leafExp_ {
public:
	A_intExp_(A_pos pos, int i);
	virtual string print_rep(int indent, bool with_attributes);

	virtual string HERA_code();
	Ty_ty typecheck();
private:
	int value;
};

class A_stringExp_ : public A_leafExp_ {
public:
	A_stringExp_(A_pos pos, String s);
	virtual string print_rep(int indent, bool with_attributes);
private:
	int count;
	String value;
	virtual string HERA_code();
	virtual string HERA_data();
	Ty_ty typecheck();
};

class A_recordExp_ : public A_literalExp_ {
public:
	A_recordExp_(A_pos pos, Symbol typ, A_efieldList fields);
	virtual string print_rep(int indent, bool with_attributes);
private:
	Symbol _typ;
	A_efieldList _fields;
};

class A_arrayExp_ : public A_literalExp_ {
public:
	A_arrayExp_(A_pos pos, Symbol typ, A_exp size, A_exp init);
	virtual string print_rep(int indent, bool with_attributes);
private:
	Symbol _typ;
	A_exp _size;
	A_exp _init;
};


class A_varExp_ : public A_exp_ {
public:
	A_varExp_(A_pos pos, A_var var);
	virtual string print_rep(int indent, bool with_attributes);
private:
	A_var _var;
};

typedef enum {A_plusOp, A_minusOp, A_timesOp, A_divideOp,
	     A_eqOp, A_neqOp, A_ltOp, A_leOp, A_gtOp, A_geOp} A_oper;

class A_opExp_ : public A_exp_ {
public:
	A_opExp_(A_pos pos, A_oper oper, A_exp left, A_exp right);
	virtual string print_rep(int indent, bool with_attributes);
	virtual string HERA_data();
	virtual string HERA_code();
	Ty_ty typecheck();
	virtual int init_result_reg();


	void set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent);
	virtual int compute_height();  // just for an example, not needed to compile
private:
	A_oper _oper;
	A_exp _left;
	A_exp _right;
};

class A_assignExp_ : public A_exp_ {
public:
	A_assignExp_(A_pos pos, A_var var, A_exp exp);
	virtual string print_rep(int indent, bool with_attributes);
private:
	A_var _var;
	A_exp _exp;
};

class A_letExp_ : public A_exp_ {
public:
	A_letExp_(A_pos pos, A_decList decs, A_exp body);
	virtual string print_rep(int indent, bool with_attributes);
private:
	A_decList _decs;
	A_exp _body;
};

class A_callExp_ : public A_exp_ {
	public:
		A_callExp_(A_pos pos, Symbol func, A_expList args);
	virtual string print_rep(int indent, bool with_attributes);
private:
	Symbol _func;
	A_expList _args;
	virtual string HERA_code();
	virtual string HERA_data();
	virtual int init_result_reg();
	Ty_ty typecheck();
	void set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent);
};

class A_controlExp_ : public A_exp_ {
public:
	A_controlExp_(A_pos p);
};

class A_ifExp_ : public A_controlExp_ {
public:
	A_ifExp_(A_pos pos, A_exp test, A_exp then, A_exp else_or_0_pointer_for_no_else);
	virtual string print_rep(int indent, bool with_attributes);
	virtual string HERA_code();
	virtual string HERA_data();
	virtual int init_result_reg();
	Ty_ty typecheck();
	void set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent);
private:
	A_exp _test;
	A_exp _then;
	A_exp _else_or_null;
};


class A_forExp_ : public A_controlExp_ {
public:
	A_forExp_(A_pos pos, Symbol var, A_exp lo, A_exp hi, A_exp body);
	virtual string print_rep(int indent, bool with_attributes);
private:
	Symbol _var;
	A_exp _lo;
	A_exp _hi;
	A_exp _body;
};


class A_breakExp_ : public A_controlExp_ {
public:
	A_breakExp_(A_pos p);
	virtual string print_rep(int indent, bool with_attributes);

};

class A_seqExp_ : public A_controlExp_ {
public:
	A_seqExp_(A_pos pos, A_expList seq);
	virtual string print_rep(int indent, bool with_attributes);
	virtual string HERA_code();
	virtual string HERA_data();
	virtual int init_result_reg();
	Ty_ty typecheck();
	void set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent);
private:
	A_expList _seq;
};

class A_var_ : public AST_node_ {
public:
	A_var_(A_pos p);
};

class A_simpleVar_ : public A_var_ {
public:
	A_simpleVar_(A_pos pos, Symbol sym);
	virtual string print_rep(int indent, bool with_attributes);
private:
	Symbol _sym;
};

class A_fieldVar_ : public A_var_ {
public:
	A_fieldVar_(A_pos pos, A_var var, Symbol sym);
	virtual string print_rep(int indent, bool with_attributes);
private:
	A_var _var;
	Symbol _sym;
};

class A_subscriptVar_ : public A_var_ {
public:
	A_subscriptVar_(A_pos pos, A_var var, A_exp exp);
	virtual string print_rep(int indent, bool with_attributes);

private:
	A_var _var;
	A_exp _exp;
};


class A_expList_ : public AST_node_ {
public:
	A_expList_(A_exp head, A_expList tail);
	virtual string print_rep(int indent, bool with_attributes);
	int length();
	A_exp _head;
	A_expList _tail;
	string func_HERA_code(int counter);
	string func_HERA_data();
	Ty_ty func_typecheck(int n);
	void set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent);
};

// The componends of a A_recordExp, e.g. point{X = 4, Y = 12}
class A_efield_ : public AST_node_ {
public:
	A_efield_(Symbol name, A_exp exp);
	virtual string print_rep(int indent, bool with_attributes);
	String fieldname();
private:
	Symbol _name;
	A_exp _exp;
};

class A_efieldList_ : public AST_node_ {
public:
	A_efieldList_(A_efield head, A_efieldList tail);
	virtual string print_rep(int indent, bool with_attributes);
private:
	A_efield _head;
	A_efieldList _tail;
};


class A_dec_ : public AST_node_ {
public:
	A_dec_(A_pos p);
};

class A_decList_ : public A_dec_ {
public:
	A_decList_(A_dec head, A_decList tail);
	virtual string print_rep(int indent, bool with_attributes);
private:
	A_dec _head;
	A_decList _tail;
};

class A_varDec_ : public A_dec_ {
public:
	A_varDec_(A_pos pos, Symbol var, Symbol typ, A_exp init);
	virtual string print_rep(int indent, bool with_attributes);
private:
	Symbol _var;
	Symbol _typ;
	A_exp _init;
	// Appel had this here:
	//	bool escape;
	// but it's really just an inherited attribute set during escape analysis,
	// which is not necessary in the Haverford version of the labs,
	// where we conservatively assume all variables escape
};

class A_functionDec_: public A_dec_ {
public:
	A_functionDec_(A_pos pos, A_fundecList functions_that_might_call_each_other);
	virtual string print_rep(int indent, bool with_attributes);
private:
	A_fundecList theFunctions;
};

class A_typeDec_: public A_dec_ {
public:
	A_typeDec_(A_pos pos, A_nametyList types_that_might_refer_to_each_other);
	virtual string print_rep(int indent, bool with_attributes);
private:
	A_nametyList theTypes;
};

class A_fundec_ : public AST_node_ {  // possibly this would be happier as a subclass of "A_dec_"?
public:
	A_fundec_(A_pos pos, Symbol name, A_fieldList params, Symbol result_type,  A_exp body);
	virtual string print_rep(int indent, bool with_attributes);
private:
	Symbol _name;
	A_fieldList _params;
	Symbol _result;
	A_exp _body;
};
class A_fundecList_ : public AST_node_ {
public:
	A_fundecList_(A_fundec head, A_fundecList tail);
	virtual string print_rep(int indent, bool with_attributes);
private:
	A_fundec _head;
	A_fundecList _tail;
};


//  Giving a name to a type with Namety -- this is a declaration of a type
class A_namety_ : public AST_node_ {  // possibly this would be happier as a subclass of "A_dec_"?
public:
	A_namety_(A_pos pos, Symbol name, A_ty ty);
	virtual string print_rep(int indent, bool with_attributes);
private:
	Symbol _name;
	A_ty _ty;
};
class A_nametyList_ : public AST_node_ {   // possibly this would be happier as a subclass of "A_dec_"?
public:
	A_nametyList_(A_namety head, A_nametyList tail);
	virtual string print_rep(int indent, bool with_attributes);
private:
	A_namety _head;
	A_nametyList _tail;
};


// List of fields in a declaration, either
//  the function parameters: function power(B: INT, E: INT)
//  or record fields:        type point = {X: INT, Y: INT)

class A_fieldList_ : public AST_node_ {
public:
	A_fieldList_(A_field head, A_fieldList tail);
	virtual string print_rep(int indent, bool with_attributes);
private:
	A_field _head;
	A_fieldList _tail;
};

class A_field_ : public AST_node_ {
public:
	A_field_(A_pos pos, Symbol name, Symbol type_or_0_pointer_for_no_type_in_declaration);
	virtual string print_rep(int indent, bool with_attributes);
private:
	Symbol _name;
	Symbol _typ;
};


class A_ty_ : public AST_node_ {
public:
	A_ty_(A_pos p);
};

//  Using the name of a type to declare a variable with NameTy -- this is a use of a type

class A_nameTy_ : public A_ty_ {
public:
	A_nameTy_(A_pos pos, Symbol name);
	virtual string print_rep(int indent, bool with_attributes);
private:
	Symbol _name;
};

class A_recordty_ : public A_ty_ {
public:
	A_recordty_(A_pos pos, A_fieldList record);
	virtual string print_rep(int indent, bool with_attributes);
private:
	A_fieldList _record;
};

class A_arrayty_ : public A_ty_ {
public:
	A_arrayty_(A_pos pos, Symbol array);
	virtual string print_rep(int indent, bool with_attributes);
private:
	Symbol _array;   // type of element in the array
};


extern bool have_AST_attrs;	// can be set to true with command-line arguments in tiger.cc, to print attributes
const bool AST_print_positions=false;


#include "AST_appel.h"  /* For compatibility with book, and more concise object creation */
#endif
