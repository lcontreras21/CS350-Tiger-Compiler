#if ! defined _SYMBOL_H
#define _SYMBOL_H

#include "util.h"

// this leaks memory all over the place,
//   but avoids problems with calling constructor/destructor in unions
//   in types.h and lexical scanner

typedef const string *Symbol;


static inline Symbol to_Symbol(const string &s)
{
	return new string(s);
}

static inline Symbol to_Symbol(const char *s)
{
	precondition(s != 0);
	return new string(s);
}

static inline String Symbol_to_string(Symbol sym)
{
	return *sym;
}
static inline String str(Symbol sym) { return Symbol_to_string(sym); }
static inline String repr(Symbol sym) { return "to_Symbol(" + repr(Symbol_to_string(sym)) + ")"; }

static inline bool Symbols_are_equal(Symbol s1, Symbol s2)
{
	return Symbol_to_string(s1) == Symbol_to_string(s2);
}

#endif
