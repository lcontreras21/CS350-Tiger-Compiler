/*
 * The DECLARE_SYNTHESIZE, DECLARE_PINHERIT, and DECLARE_TRAVERSAL_TYPES macros
 *   are used to implement some generic tree traversals in the AST classes.
 *
 *
 * These are macros because G++ can't seem to do what I want with templates for virtual functions,
 *   and it's not clear to me from the C++ language references I've seen that it can be done at all in C++.
 *
 * These macros are used to create virtual "synthesize" and "pinherit" function for each needed
 *   type of attribute. They must be included in each non-leaf AST class (since leaf classes do
 *   not need to traverse their children, and can thus rely on the AST class's generic version).
 */
#define DECLARE_SYNTHESIZE(some_type)  \
	virtual some_type synthesize(some_type (AST_node_::*get_what_from_children)(), \
				 				some_type (*how_to_combine)(const some_type &, const some_type &), \
				 				const some_type &default_value_for_leaves);

#define DECLARE_PINHERIT(some_type) \
	virtual void pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_for_you);


/* ****************************************************************************** */
/* ***    *WARNING* IF YOU EDIT THE "DECLARE" LINES BELOW, REMEMBER THAT      *** */
/* ***              1. EVERY LINE *MUST* END WITH A BACKSLASH                 *** */
/* ***              2. COMMENTS MUST BE WITH SLASH-STAR, NOT DOUBLE-SLASH     *** */
/* ****************************************************************************** */

#define DECLARE_TRAVERSAL_TYPES() \
	/* Declarations of synthesized and inherited attributes go here, e.g.     */ \
	DECLARE_SYNTHESIZE(int)  /* to allow synthesized integer attributes (e.g. const_int_value), or  */ \
	DECLARE_SYNTHESIZE(bool) /* to allow synthesized boolean attributes (e.g. is_constant), or  */ \
	DECLARE_PINHERIT(int)    /* to allow inherited real number attributes (e.g. "depth") */ \
	DECLARE_PINHERIT(AST_pointer)    /* to allow inherited references to node, specifically for setting up my_parent() info */ \
	/* Remember not to use double-slash comments, to end with a backslash     */ \
	/* and to change synthesize.cc or pinherit.cc to be consistent            */ \

