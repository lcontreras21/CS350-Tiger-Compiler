#if ! defined _TYPES_H
#define _TYPES_H

/*
 * types.h - 
 *
 * All types and functions declared in this header file begin with "Ty_"
 * Linked list types end with "..list"
 */

#include "symbol.h"

/* These typedefs have to come first in C-style programming  */
typedef struct Ty_ty_        *Ty_ty;
typedef struct Ty_tyList_    *Ty_tyList;
typedef struct Ty_field_     *Ty_field;
typedef struct Ty_fieldList_ *Ty_fieldList;

/*
 * There are six primitive types in our compiler ... Appel's four, plus Dave W added Ty_Bool and Ty_Error.
 * Note that a reference to the _same_ type object is returned for every call to any one of
 *  these functions, but of course the different functions return distinct values.
 * So, e.g. Ty_Int() == Ty_Int(), and Ty_Int() != Ty_Bool().
 * This will be quite useful for simple type checking, though for compound types much more
 *  is, of course, needed.
 */
Ty_ty Ty_Int();  	// the type of an integer expression, such as 42 or 5*8+2
Ty_ty Ty_Bool();	// the type of a boolean expression, e.g. 3<17 or "good" > "evil"
Ty_ty Ty_String();	// the type of a string expresison, e.g. "good"
Ty_ty Ty_Void();	// the type of an expression without a value, e.g.  ( )   or  if x<0 then print("D'oh ... x<0")
Ty_ty Ty_Nil();  	// the type of the nil expression e.g. nil (this is a value belonging to every record type)
Ty_ty Ty_Error();	// useful to indicate an error has already been produced for an expression

/*
 * There are four "extending" constructors for types, for functions, records, arrays,
 *  and named types, which count as a kind of type in the tiger compiler:
 */

Ty_ty Ty_Function(Ty_ty the_return_type, Ty_fieldList the_parameters);
Ty_ty Ty_Record(Ty_fieldList fields);
Ty_ty Ty_Array(Ty_ty ty);
Ty_ty Ty_Name(Symbol sym, Ty_ty ty);

/*
 * The above rely on things like fields and lists, so here are their constructors:
 */
Ty_tyList Ty_TyList(Ty_ty head, Ty_tyList tail);
Ty_field Ty_Field(Symbol name, Ty_ty ty);
Ty_fieldList Ty_FieldList(Ty_field head, Ty_fieldList tail);



/*
 * Some useful functions
 */

void Ty_print(Ty_ty t);
void Ty_print_fieldList(Ty_fieldList l);
void TyList_print(Ty_tyList list);

void Ty_examples();  // examples


/*
 * Representation of Types...
 *  this will be important when you are adding to the type system,
 *  e.g. writing a function to check to see if two types are equal
 *  (or, phrased a slightly different way, to see if a given type
 *   is suitable in a context where a particular type is needed).
 */

enum Ty_ty_kind {Ty_error, Ty_nil, Ty_int, Ty_bool, Ty_void, Ty_string,
		 Ty_function, Ty_record, Ty_array, Ty_name};

struct Ty_ty_ {
	enum Ty_ty_kind kind;
	union the_info {
		struct {Ty_ty return_type; Ty_fieldList parameter_types; } function;
		Ty_fieldList record;
		Ty_ty array;
		struct {Symbol sym; Ty_ty ty;} name;
	} u;

	string __repr__();
	string __str__()  { return this->__repr__(); }
};

struct Ty_tyList_ {Ty_ty head; Ty_tyList tail; string __str__();};
struct Ty_field_  {Symbol name; Ty_ty ty;};
struct Ty_fieldList_ {Ty_field head; Ty_fieldList tail; string __str__();};

/* the following are like repr(), but allow a null pointer, which is important for lists */
string to_String(Ty_ty t);
string to_String(Ty_fieldList fl);
string to_String(Ty_tyList tl);
#endif
