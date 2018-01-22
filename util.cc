/*
 * util.c - commonly used utility functions.

 To run the example/test of string printing, use the command
	g++ -std=c++0x  -D UTIL_EXAMPLES_IS_MAIN=1 util.cc -o util_examples && ./util_examples

*/

#include <string>
#include "util.h"


// Given a string s, what could we enter into a C++ (or HERA-C) file to put s into a program?
//
// Remember that in C/C++, unlike Python, an element of a string is a
//   *character*, not a one-character string.
// In C/C++, apostrophes (single-quotes) indicate characters,
//   while standard double-quotes indicate strings.
// So 'c' is the character c,
//    "c" is a string of length 1
//    "c++" is a string of length 3, and
//    'c++' is something you probably should not use.

string repr_for_std_string(const string  &s)
{
	string rep = "\"";
	string hexdigits = "0123456789ABCDEF";
	for (unsigned int i=0; i<s.length(); i++) {
		if (0 > s[i] || s[i] > 0x7F) { // non-ASCII
			// for now, do nothing
		} else if (32 <= s[i] && s[i] <= 0x7E && s[i] != '\"' && s[i]!='\\') { // print these as they are
			rep.append(1, s[i]);
		} else {
			// as per http://en.cppreference.com/w/cpp/language/escape
			rep.append(1, '\\');  // that's one backslash character, of course
#if 1
			rep.append(1, hexdigits[s[i] / (8*8)]);
			rep.append(1, hexdigits[(s[i] / 8) % 8]);
			rep.append(1, hexdigits[s[i] % 8]);
#else
			// GCC bug give weird results for \x09DaveW
			rep.append(1, 'x');  // that's one backslash character, of course
			rep.append(1, hexdigits[s[i] / 16]);
			rep.append(1, hexdigits[s[i] % 16]);
#endif
		}
	}
	rep.append(1, '\"');
	return rep;
}


// useful for testing/demonstrating repr vs. plain strings
string util_example_string_chars_1_through_7f()
{
	string example = "";
	example.append(0x7f, 'x');
	for (char c=1; c<=0x7e; c++) { // stop at 7e to avoid infinite loop
		example[c-1] = c;
	}
	example[0x7e] = 0x7f;
	return example;
}


#if defined UTIL_EXAMPLES_IS_MAIN && UTIL_EXAMPLES_IS_MAIN
#include <iostream>
using namespace std;
int main(int argc, char *argv[])
{
	string example = util_example_string_chars_1_through_7f();

	cout << "example.length() and repr(example) are: " << example.length() << " " << repr(example) << endl;
	cout << "what happens if we just cout example?   " << example << endl;

	string example_copy_paste_from_output = "\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017\020\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037 !\042#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\134]^_`abcdefghijklmnopqrstuvwxyz{|}~\177";
//	string example_copy_paste_from_output = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F !\x22#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\x5C]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7F";

	if (example == example_copy_paste_from_output) {
		cout << "\n  check vs. copy-paste works :-)" << endl;
	} else {
		cout << "uh-oh ... copy-paste doesn't seem to agree.... lengths are " << example.length() << " and " << example_copy_paste_from_output.length() << " and the differ at:" << endl;
		for (unsigned int i=0; i<=min(example.length(), example_copy_paste_from_output.length()); i++) {
			if (example[i] != example_copy_paste_from_output[i]) {
				cout << "character " << i << " is #" << (int(example[i])) << " and #" << int(example_copy_paste_from_output[i]) << endl;
			}
		}
	}
}
#endif

string repr(string s)    { return repr_for_std_string(s); }
string repr(bool b)      { return (b ? "true" : "false"); }
string repr(int x)       { return std::to_string(x); }
string repr(long x)      { return std::to_string(x); }
string repr(unsigned x)  { return std::to_string(x); }

// template<class T> string str(T *p)   { return p->__str__(); }
// template<class T> string str(T &x)   { return x.__str__(); }
string str(bool b) { return repr(b); }
string str(int x)  { return repr(x); }
string str(long x)  { return repr(x); }
string str(unsigned x)  { return repr(x); }
