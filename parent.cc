#include "AST.h"

// The following should never be called, but this "general" version
//   lets us compile and test some things without having to fill in the set_parent_pointers stuff first

void AST_node_::set_parent_pointers_for_me_and_my_decendents(AST_node_ *my_parent_or_null_if_i_am_the_root)
{
	stored_parent = my_parent_or_null_if_i_am_the_root;
	EM_debug("Uh-oh, need to make set_parent_pointers_for_me_and_my_descendents actually do its full job...", pos());
	EM_debug(" rewrite or overrride it, instead of running this hack that's in the AST_node_ class now.", pos());
}


void A_literal_::set_parent_pointers_for_me_and_my_decendents(AST_node_ *my_parent_or_null_if_i_am_the_root)
{
	stored_parent = my_parent_or_null_if_i_am_the_root;
}


void A_opExp_::set_parent_pointers_for_me_and_my_decendents(AST_node_ *my_parent_or_null_if_i_am_the_root)
{
	// record my parent
	stored_parent = my_parent_or_null_if_i_am_the_root;
	// now, tell my children to record me as theirs... they'll tell the grandkids
	_right->set_parent_pointers_for_me_and_my_decendents(this);
	 _left->set_parent_pointers_for_me_and_my_decendents(this);
}

void A_root_::set_parent_pointers_for_me_and_my_decendents(AST_node_ *my_parent_or_null_if_i_am_the_root)
{
	// This has been inlined into the root expression constructor,
	//   so it shouldn't actually be needed again...
	if (main_expr->get_parent_without_checking() == this) {
		EM_warning("Strange ... called set_parent_pointers_for_me_and_my_decendents for A_root, rather than relying on constructor", Position::undefined());
	} else {
		EM_error("Called set_parent_pointers_for_me_and_my_decendents for A_root, rather than relying on constructor, AND NOTICED AN INCONSISTENCY!", Position::undefined());
	}
	// otherwise, we would have done this:
	// assert(my_parent_or_null_if_i_am_the_root == 0);
	// stored_parent = my_parent_or_null_if_i_am_the_root;
	// main_expr->set_parent_pointers_for_me_and_my_decendents(this);
}


AST_node_ *AST_node_::get_parent_without_checking()
{
	return stored_parent;
}
AST_node_ *AST_node_::parent()	// get the parent node, after the 'set parent pointers' pass
{
	assert("parent pointers have been set" && stored_parent);
	return stored_parent;
}

AST_node_ *A_root_::parent() {
	assert("Uh-oh ... called A_root_::parent() :-(" && false);
	return stored_parent;  // shuts up compiler warnings, handles case when assertions are off
}
