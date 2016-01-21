#include "AST.h"

// WARNING:  since we're using #define rather than a reasonable language mechanism,
//           it is important *NOT* to use pointer types without first using "typedef".
//           For example, to pinherit an "AST_node_ *" attribute, we
//           *** DO NOT *** write "#define some_type AST_node_ *", but instead use
//             typedef AST_node_ *AST_pointer;
//             #define some_type AST_pointer
//           Using the first would give the wrong meaning to "const some_type", since
//           "const AST_pointer" means the pointer is constant (which is what pinherit needs),
//           but "const AST *" means the pointer points to a constant AST.
// For simple types and classes (e.g., int, bool, String), this is not a problem.


// for e.g. "depth" attribute
#define some_type int
#include "pinherit-implementation.h"
#undef some_type

// for setting up parent pointers
// typedef AST_node_ *AST_pointer;
#define some_type AST_pointer
#include "pinherit-implementation.h"
#undef some_type





