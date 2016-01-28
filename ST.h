#if ! defined ST_H
#define ST_H 1

// General Symbol Table abstraction
//  Fall 2001 - Spring 2002, by davew@cs.haverford.edu
// Basic operations:
//  Create null or 1-element ST's with constructor.
//  "Fuse" (a.k.a. FuseOneScope) combines disjoint ST's (for dec's in same scope).
//  "Merge" (a.k.a. MergeAndShadow) combines ST's from different scopes
//	merge(outer,inner) has symbols from "inner" shadow those of "outer".
// For details & sample uses of operations, see test_ST in ST.c.


// First, what information do we keep in the symbol table?
// This could be templated, but since we only need to use one
//  kind of symbol table in this compiler, the following works:

#include "symbol.h"
typedef Symbol nametype;  // This would let us change to using something other than Symbols (e.g. Strings) as names


// some pre-declarations keep G++ happy:
template <class symbol_info> class ST;
template <class symbol_info> class ST_node; // should be private to class ST, but this breaks g++

template <class symbol_info> bool is_name_there(const nametype &look_for_me, const ST<symbol_info> &in_this_table);
template <class symbol_info> symbol_info &lookup(const nametype &must_find_this, const ST<symbol_info> &in_this_table);
template <class symbol_info> ST<symbol_info> merge_or_fuse(const ST<symbol_info> &outer, const ST<symbol_info> &inner, bool merge_dups);



template <class symbol_info> class ST {
public:
// Class ST has FOUR constructors, two classic C++ constructors,
//  and also "fuse" and "merge":
	
	// make empty ST -- always works
	ST();
	// make 1-entry ST -- always works
	ST(const nametype &name, const symbol_info &info);
	// fuse and merge are listed below the class (they were friends, but G++ was getting sick from this)
	

// Now the accessor  functions: is_name_there and lookup and to_String

	// return true iff "look_for_me" is present in "in_this_table"
	//
	// axioms:
	// is_name_there(look, ST())      	    === false
	// is_name_there(look, ST(n, i))  	    === look == n
	// is_name_there(look, fuse(T1, T2))	=== is_name_there(look, T1) || is_name_there(look, T2)
	// is_name_there(look, merge(in, out))	=== is_name_there(look, in) || is_name_there(look, out)
 	friend bool is_name_there<symbol_info>(const nametype &look_for_me, const ST<symbol_info> &in_this_table);


	// check for name,
	//   returning info for "first" match (the one that's not shadowed),
	//   or throw ST::undefined_symbol(name) if no matches
	// Return a REFERENCE to the symbol_info in the table.
	//	NOTE that FUSE, MERGE, ST::ST, and op== DO NOT copy the symbol_info,
	//	so if something is added once to a table and then propogates into
	//	into many tables, ALL the symbol_info's for that thing are SHARED
	// Thus, changes made via "lookup" are visible EVERYWHERE the symbol_info can be found
	//
	// axioms:
	// lookup(look, ST())      	    === <exception thrown due to missing name>
	// lookup(look, ST(n, i))  	    ===    i, if look == n
	//                                  or <exception thrown due to missing name>, otherwise
	// lookup(look, fuse(T1, T2))	===    lookup(look, T1), if is_name_there(look, T1),
	//                                  or lookup(look, T2), otherwise
	// lookup(look, merge(in, out))	===    lookup(look, in), if is_name_there(look, T1),
	//                                  or lookup(look, out), otherwise
	friend symbol_info &lookup<symbol_info>(const nametype &must_find_this, const ST<symbol_info> &in_this_table);
	string __repr__();
	string __str__()  { return this->__repr__(); }
	
	class duplicate_symbol { // error type for exceptions
	public:
		nametype name;
		duplicate_symbol(const nametype &n);
	};

	class undefined_symbol {  // another exception type
	public:
		nametype name;
		undefined_symbol(const nametype &n);
	};

private:
	// DATA
	ST_node<symbol_info> *head; // Null pointer for empty ST

	// A FEW PRIVATE OPERATIONS 
	const ST_node<symbol_info> *check_for(const nametype &name) const;

	friend ST<symbol_info> merge_or_fuse<symbol_info>(const ST<symbol_info> &outer, const ST<symbol_info> &inner, bool merge_dups);
};

// Build a "single-scope" Symbol Table from 2 single-scope tables.
//   If name sets not disjoint, throw ST::duplicate_symbol(name)
// Implementations are free to throw some other error if
//   s1 or s2 are result of a "merge"
//   rather than constructors & fuse operations.
// NOTE that this does not COPY any symbol_info structures -- they are all SHARED
template <class symbol_info> ST<symbol_info> fuse(const ST<symbol_info> &s1, const ST<symbol_info> &s2);

// Merge two tables for different scopes.
//   The names in the first ST shadow those in the second ST
//   Never complains (unless out of memory)
// NOTE that this does not COPY any symbol_info structures -- they are all SHARED
template <class symbol_info> ST<symbol_info> merge(const ST<symbol_info> &inner, const ST<symbol_info> &outer);

// Friendlier names
template <class symbol_info>  ST<symbol_info> FuseOneScope(const ST<symbol_info> &s1, const ST<symbol_info> &s2) { return fuse(s1, s2); }
template <class symbol_info>  ST<symbol_info> MergeAndShadow(const ST<symbol_info> &inner, const ST<symbol_info> &outer) { return merge(inner, outer); }

// also useful (e.g. in debugging)
template <class symbol_info> String to_String(const ST<symbol_info> &table);

void ST_test();   // check internal consistency
void ST_examples();  // show some interesting examples to understand how ST works

#include "ST.t"

#endif
