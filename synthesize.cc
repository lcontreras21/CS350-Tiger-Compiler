#if defined COMPILE_SYNTHESIZE

#include "AST.h"

// WARNING:  since we're using #define rather than a reasonable language mechanism,
//           it is important *NOT* to use pointer types without first using "typedef".
//           For example, to synthesize an "AST_node_ *" attribute, you should
//             *** NOT *** write "#define some_type AST_node_ *", but rather use
//             typedef AST_node_ *AST_pointer
//             #define some_type AST_pointer
//           Using the first would give the wrong meaning to "const some_type", since
//           "const AST_pointer" means the pointer is constant (which is what synthesize needs),
//           but "const AST *" means the pointer points to a constant AST.
// For simple types and classes (e.g., int, bool, String), this is not a problem.

#define some_type bool
#include "synthesize-implementation.h"
#undef some_type

#define some_type int
#include "synthesize-implementation.h"
#undef some_type

#endif
