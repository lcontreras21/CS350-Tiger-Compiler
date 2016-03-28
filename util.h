#if ! defined UTIL_H
#define UTIL_H

#include <string>
#include <logic.h>
using HaverfordCS::precondition_failed;
using HaverfordCS::postcondition_failed;
using HaverfordCS::assert_failed;
// add others here if needed

typedef std::string String;  // for compatibility with some old code
typedef std::string string;  // like 'using namespace std', but doesn't pull in the whole namespace, just 'string'


string repr_for_std_string(const string &s);  // non-trivial; in util.cc (changes non-printing characters into \x##)
template<class T> string repr(T *p)   { return p->__repr__(); }
template<class T> string repr(T &x)   { return x.__repr__(); }
// template<>        string repr(string &s) { return repr_for_std_string(s); }
// template<>        string repr(string s) { return repr_for_std_string(s); }
static            string repr(string s)    { return repr_for_std_string(s); }
static            string repr(bool b)      { return (b ? "true" : "false"); }
static            string repr(int x)       { return std::to_string(x); }
static            string repr(long x)      { return std::to_string(x); }
static            string repr(unsigned x)  { return std::to_string(x); }

// figuring out template specialization, based in part on http://en.cppreference.com/w/cpp/language/template_specialization

template<class T> string str(T *p)   { return p->__str__(); }
template<class T> string str(T &x)   { return x.__str__(); }
static            string str(bool b) { return repr(b); }
static            string str(int x)  { return repr(x); }
static            string str(long x)  { return repr(x); }
static            string str(unsigned x)  { return repr(x); }

// Restored for 2016, since it had been in the starter files:
string appendchar(string, char);

#endif
