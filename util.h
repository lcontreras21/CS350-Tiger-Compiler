#if ! defined UTIL_H
#define UTIL_H

#include <logic.h>
#include <string>
typedef std::string String;  // for compatibility with some old code
typedef std::string string;  // like 'using namespace std', but doesn't pull in the whole namespace, just 'string'


template<class T> string str(T *p)   { return p->__str__(); }
template<class T> string str(T &x)   { return x.__str__(); }
static            string str(int x)  { return std::to_string(x); }
static            string str(long x)  { return std::to_string(x); }
static            string str(unsigned x)  { return std::to_string(x); }

template<class T> string repr(T *p)   { return p->__repr__(); }
template<class T> string repr(T &x)   { return x.__repr__(); }
static            string repr(int x)  { return std::to_string(x); }
static            string repr(long x)  { return std::to_string(x); }
static            string repr(unsigned x)  { return std::to_string(x); }
extern            string repr(string   s);  // non-trivial; in util.cc (changes non-printing characters into \x##)

#endif
