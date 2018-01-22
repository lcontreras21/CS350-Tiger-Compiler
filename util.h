#if ! defined UTIL_H
#define UTIL_H

#include <string>
#include <logic.h>
using HaverfordCS::precondition_failed;
using HaverfordCS::postcondition_failed;
using HaverfordCS::assert_failed;
// add others here if needed

using std::string;
typedef std::string String;  // for compatibility with some old code


string repr_for_std_string(const string &s);  // non-trivial; in util.cc (changes non-printing characters into \x##)
template<class T> string repr(T *p)   { return p->__repr__(); }
template<class T> string repr(T &x)   { return x.__repr__(); }
// template<>        string repr(string &s) { return repr_for_std_string(s); }
// template<>        string repr(string s) { return repr_for_std_string(s); }
string repr(string s);
string repr(bool b);
string repr(int x);
string repr(long x);
string repr(unsigned x);

// figuring out template specialization, based in part on http://en.cppreference.com/w/cpp/language/template_specialization

template<class T> string str(T *p)   { return p->__str__(); }
template<class T> string str(T &x)   { return x.__str__(); }
string str(bool b);
string str(int x);
string str(long x);
string str(unsigned x);

#endif
