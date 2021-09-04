#include "AST.h"

/*
	Functions used by children to find out information about their parent
EG:
	break asking about earliest while or error
*/


int A_root_::am_i_in_while() {
	EM_error("Break not found within WHILE expression");
	return -1;
}

int AST_node_::am_i_in_while() {
	return stored_parent->am_i_in_while();
}

int A_whileExp_::am_i_in_while() {
	return my_num;
}
