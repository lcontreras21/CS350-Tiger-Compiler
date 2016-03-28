/*
 * types.c - 
 *
 * All types and functions declared in this header file begin with "Ty_"
 * Linked list types end with "..list"
 */

#include <iostream>
using namespace std;
#include <hc_list.h>  // Haverford "list" class
#include <hc_list_helpers.h>  // and associated extra functionality
using namespace HaverfordCS;
#include "errormsg.h"
#include "util.h"
#include "types.h"

// produce and print type info for the types in this tiger code:
// 
// let
// 		var x : int = 5
// 		function power(b: int, e: int) : int ....
// 		function is_prime(i: int): bool ...
// 		type complex = { real : int, imag : int }
// 		type list    = { head : int, tail : list }
// in
// 		is_prime(power(2, x)-1)
// end

void Ty_examples()  // examples
{
// 		var x : int = 5
	Ty_ty type_of_x = Ty_Int();
	cout << "type of x is: " << to_String(type_of_x) << endl;
	cout << "is type of x == Ty_Int()?  " << ((type_of_x == Ty_Int())?"true":"false")  << endl;
	cout << "is type of x == Ty_Bool()? " << ((type_of_x == Ty_Bool())?"true":"false") << endl;

	cout << endl;
// 		function power(b: int, e: int) : int ....
	Ty_ty type_of_power =
		Ty_Function(Ty_Int(), // return
			    Ty_FieldList(Ty_Field(to_Symbol("b"), Ty_Int()), // b
					 Ty_FieldList(Ty_Field(to_Symbol("e"), Ty_Int()), // e
						      0))
			);
	cout << "type of power is: " << to_String(type_of_power) << endl;
	cout << "is power a function? " << (type_of_power->kind == Ty_function ? "yes" : "no --- bother!") << endl;
	cout << "return type of power is: " << to_String(type_of_power->u.function.return_type) << endl;
	cout << "is type of power == Ty_Function( ... same stuff as before ...)? " <<
	  ((type_of_power == 
		Ty_Function(Ty_Int(), // return
					 Ty_FieldList(Ty_Field(0, Ty_Int()), // b
						      Ty_FieldList(Ty_Field(0, Ty_Int()), // e
								   0))))
	   ? "Yes ... Dave is surprised" : "No ... if we want to compare more interesting types, we'll need a better function") <<
	  endl;

// 		function is_prime(i: int) : bool ....
	Ty_ty type_of_is_prime = 
		Ty_Function(Ty_Bool(), // return
					 Ty_FieldList(Ty_Field(0, Ty_Int()), // x ... note that parameter name is just for debugging/error messages
						      0)
			);
	cout << "type of is_prime is: " << to_String(type_of_is_prime) << endl;

	cout << endl;
// 		type complex = { real : int, imag : int }
//		for this, we could in principle build the whole type (the ty_record for the fields, and the ty_name to name it) at once:
	Ty_ty type_of_complex = Ty_Name(to_Symbol("complex"),
								    Ty_Record(Ty_FieldList(Ty_Field(to_Symbol("real"), Ty_Int()),
														   Ty_FieldList(Ty_Field(to_Symbol("imag"), Ty_Int()),
														                0))));
	cout << "type of complex is: " << to_String(type_of_complex) << endl;

//      type list    = { head : int, tail : list }
//      for list, we can't build the full ty_record until we know what list is ... hmmm ... what to do?
//      we build the ty_name first with a placeholder (such as ty_error) and then fill it in later:

	Ty_ty type_of_list = Ty_Name(to_Symbol("list"), Ty_Error());
	// o.k., there's the Ty_Name ... now we can _overwrite_ the type
	//
	assert(type_of_list->kind == Ty_name);
	type_of_list->u.name.ty = Ty_Record(Ty_FieldList(Ty_Field(to_Symbol("head"), Ty_Int()),
													 Ty_FieldList(Ty_Field(to_Symbol("rest"), type_of_list), // NOTE USE OF TYPE WE'RE DEFINING
																  0)));

	cout << "type of list is: " << to_String(type_of_list) << endl;

	cout << endl;
}

static struct Ty_ty_ tyerr = {Ty_error};
Ty_ty Ty_Error() {return &tyerr;}

static struct Ty_ty_ tynil = {Ty_nil};
Ty_ty Ty_Nil() {return &tynil;}

static struct Ty_ty_ tyint = {Ty_int};
Ty_ty Ty_Int() {return &tyint;}

static struct Ty_ty_ tybool = {Ty_bool};
Ty_ty Ty_Bool() {return &tybool;}

static struct Ty_ty_ tyvoid = {Ty_void};
Ty_ty Ty_Void() {return &tyvoid;}

static struct Ty_ty_ tystring = {Ty_string};
Ty_ty Ty_String() {return &tystring;}

Ty_ty Ty_Record(Ty_fieldList fields)
{
	Ty_ty p = new Ty_ty_;
	p->kind=Ty_record;
	p->u.record=fields;
	return p;
}

Ty_ty Ty_Array(Ty_ty ty)
{
	Ty_ty p = new Ty_ty_;
	p->kind=Ty_array;
	p->u.array=ty;
	return p;
}


Ty_ty Ty_Function(Ty_ty the_return_type, Ty_fieldList the_parameters)
{
	Ty_ty p = new Ty_ty_;
	p->kind=Ty_function;
	p->u.function.return_type = the_return_type;
	p->u.function.parameter_types = the_parameters;
	return p;
}

Ty_ty Ty_Name(Symbol sym, Ty_ty ty)
{
	Ty_ty p = new Ty_ty_;
	p->kind=Ty_name;
	p->u.name.sym=sym;
	p->u.name.ty=ty;
	return p;
}


Ty_tyList Ty_TyList(Ty_ty head, Ty_tyList tail)
{
	Ty_tyList p = new Ty_tyList_;
	p->head=head;
	p->tail=tail;
	return p;
}

Ty_field Ty_Field(Symbol name, Ty_ty ty)
{
	Ty_field p = new Ty_field_;
	p->name=name;
	p->ty=ty;
	return p;
}

Ty_fieldList Ty_FieldList(Ty_field head, Ty_fieldList tail)
{
	Ty_fieldList p = new Ty_fieldList_;
	p->head=head;
	p->tail=tail;
	return p;
}

/* printing functions - used for debugging */
#define INVALID_TYPE_KIND "<unknown-type-kind>"

static string to_String_rec(Ty_ty t, list<Ty_ty> &types_done);
static string to_String_rec(Ty_fieldList fl, list<Ty_ty> &types_done);
static string to_String_rec(Ty_tyList tl, list<Ty_ty> &types_done);

static string to_String(Ty_ty_kind what_kind)
{
  switch (what_kind) {
  case Ty_error: 
    return "Ty_Error";
  case Ty_nil:
    return "Ty_Nil";
  case Ty_int:
    return "Ty_Int";
  case Ty_bool:
    return "Ty_Bool";
  case Ty_void:
    return "Ty_Void";
  case Ty_string:
    return "Ty_String";
  case Ty_function:
    return "Ty_Function";
  case Ty_record:
    return "Ty_Record";
  case Ty_array:
    return "Ty_Array";
  case Ty_name:
    return "Ty_Name";
  default:
    EM_warning("Hmmm... unrecognized Ty_ty_kind in types.cc; perhaps the compiler is about to crash...");
    return INVALID_TYPE_KIND;
  }
}


static bool print_type_addresses = false;  // set in debugger
static string type_address(Ty_ty t)
{
	if (print_type_addresses)
		return "[@" + str(long(t)) + "]";
	else
		return "";
}

static string to_String_rec(Ty_ty t, list<Ty_ty> &types_done)
{
  precondition(t != 0 && t->kind >= 0 && to_String(t->kind) != INVALID_TYPE_KIND );
  	string result = to_String(t->kind) + type_address(t);
  	bool repeated = find(t, types_done);
	types_done = list<Ty_ty>(t, types_done);
  	
	switch (t->kind) {
	case Ty_function:
	   	result = result +
	   		(!repeated ? ("("+ to_String_rec(t->u.function.return_type, types_done) + ", " +
							   to_String_rec(t->u.function.parameter_types, types_done) + ")")
			 : ("..."));
	   	break;
	case Ty_array:
		result = result + 
			(!repeated ? ("(" + to_String_rec(t->u.array, types_done) + ")") : "...");
	   	break;
	case Ty_record:
	   	result = result + 
	   		(!repeated ? ("(" + to_String_rec(t->u.record, types_done) + ")") : "...");
	   	break;
	case Ty_name:
	   	result = result +
				 "(" + repr(str(t->u.name.sym)) + ", " +
				   ( !repeated? to_String_rec(t->u.name.ty, types_done) : ("...") ) +
				 ")";
	   	break;
	default:
		result = result + "()";
		;
	}
	return result;
}
static string to_String_rec(Ty_fieldList fl, list<Ty_ty> &types_done)
{
	if (fl==NULL)
		return "0";
	else
		return ("Ty_FieldList(Ty_Field(" +
				((fl->head->name) ? repr(fl->head->name) : "0") + ", " +
			    to_String_rec(fl->head->ty, types_done) + "), " +
			    to_String_rec(fl->tail, types_done) + ")");
}
static string to_String_rec(Ty_tyList tl, list<Ty_ty> &types_done)
{
	if (tl == NULL)
		return "0";
	else
		return ("Ty_TyList( " +
				to_String_rec(tl->head, types_done) + ", " +
				to_String_rec(tl->tail, types_done) + ")");
}



string to_String(Ty_ty t)
{
	if (t != 0) {
		list<Ty_ty> types_done;
		return to_String_rec(t, types_done);
	} else {
		EM_warning("WARNING: null type (0) passed to to_String");
		return "(*** WARNING --- NULL TYPE ***)";
	}
}
string Ty_ty_::__repr__() { return to_String(this); }

string to_String(Ty_fieldList fl)
{
	list<Ty_ty> types_done;
	return to_String_rec(fl, types_done);
}
string Ty_fieldList_::__str__() { return to_String(this); }

string to_String(Ty_tyList tl)
{
	list<Ty_ty> types_done;
	return to_String_rec(tl, types_done);
}
string Ty_tyList_::__str__() { return to_String(this); }



void Ty_print(Ty_ty t)
{
	cout << to_String(t);
}
void Ty_printfieldList(Ty_fieldList list)
{
	cout << to_String(list);
}
void TyList_print(Ty_tyList list)
{
	cout << to_String(list);
}

