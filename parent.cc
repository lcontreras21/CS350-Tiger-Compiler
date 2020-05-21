#include "AST.h"

void A_leafExp_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root)
{
	stored_parent = my_parent_or_null_if_i_am_the_root;
}

void A_root_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root)
{
	// This has been inlined into the root expression constructor,
	//   so it shouldn't actually be needed again...
	EM_error("Strange ... called set_parent_pointers_for_me_and_my_descendants for A_root, rather than relying on constructor");
	// otherwise, we would have done this:
	// assert(my_parent_or_null_if_i_am_the_root == 0);
	// stored_parent = my_parent_or_null_if_i_am_the_root;
	// main_expr->set_parent_pointers_for_me_and_my_descendants(this);
}

void A_opExp_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent_or_null_if_i_am_the_root)
{
	// record my parent
	stored_parent = my_parent_or_null_if_i_am_the_root;
	// now, tell my children to record me as theirs... they'll tell the grandkids
	_right->set_parent_pointers_for_me_and_my_descendants(this);
	 _left->set_parent_pointers_for_me_and_my_descendants(this);
}



// The following should never be called, but this "general" version
//   lets us compile and test some things without having to fill in the set_parent_pointers stuff first
// Better design would use "= 0" in the AST_node_ class, and not have this function body,
//   except that we want to leave the bulk of the work for the labs...


void AST_node_::set_parent_pointers_for_me_and_my_descendants(AST_node_ *my_parent)
{
	EM_warning("Uh-oh, need to make set_parent_pointers_for_me_and_my_descendants actually do its full job...");
	EM_warning(" rewrite or overrride it, instead of running this code the AST_node_ class.");
}



AST_node_ *AST_node_::get_parent_without_checking()  // NOT FOR GENERAL USE ... this is only for things like checking assertions
{
	return stored_parent;
}
AST_node_ *AST_node_::parent()	// get the parent node, after the 'set parent pointers' pass
{
	assert("parent pointers have been set" && stored_parent);
	return stored_parent;
}

AST_node_ *A_root_::parent()
{
	EM_error("Called parent() for root node. This typically happens when A_root has not defined a method for some inherited attribute.", true);
	throw "Oops, shouldn't get here, if 'true' is on for 'is this error fatal";
}
