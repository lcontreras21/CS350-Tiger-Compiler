#if ! defined UTIL_H
#define UTIL_H

#include <logic.h>
#include <string>
typedef std::string String;  // for compatibility with some old code
typedef std::string string;  // like 'using namespace std', but doesn't pull in the whole namespace, just 'string'

#if ! defined __cplusplus
typedef char bool;
#define true 1
#define false 0
#endif

void *checked_malloc(int);

typedef struct U_boolList_ *U_boolList;
struct U_boolList_ {bool head; U_boolList tail;};
U_boolList U_BoolList(bool head, U_boolList tail);

void error(const char *s);

/*
 string_as_tiger_source_code(const String &s):
 Convert a string into a form that Tiger can understand,
  "normal" strings stay the same, e.g. tiger_repr("test string") == "test_string"
  but those with special characters have those characters turned back into a
  form one could enter them in tiger, e.g.,
  a three-character string with an x, then a newline, then a y,
  would be returned as a four-character string: x, then backslash, then n, then y
  
 This may be useful for error messages or other things involving strings;
 note that the conventions are not *exactly* the same as those of C++/HERA,
 so this function may not be 100% accurate for code generation.

 Characters not in the ASCII sequence (above value 127) are (currently) not translated


 Note this function sort of like what happens when the Python interpreter prints a string;
 
>>> s = "x\ny"
>>> len(s)
3
>>> print s
x
y
>>> s
'x\ny'
>>> 

*/

String string_as_tiger_source_code(const String &s);
bool test_string_as_tiger_source_code();  // self-check
// the above makes use of:
void appendchar(String &s, char c);



template<class T> string str(T *p)   { return p->__str__(); }
template<class T> string str(T &x)   { return x.__str__(); }
static            string str(int x)  { return std::to_string(x); }
static            string str(long x)  { return std::to_string(x); }
static            string str(unsigned x)  { return std::to_string(x); }

#endif
