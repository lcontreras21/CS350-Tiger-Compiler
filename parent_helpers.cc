#include "AST.h"

/*
	Functions used by *children to find out information about their parent
EG:
	break asking about earliest while or error
*/


int A_root_::am_i_in_loop(AST_node_ *child) {
	EM_error("Break not found within WHILE expression");
	return -1;
}

int AST_node_::am_i_in_loop(AST_node_ *child) {
	return stored_parent->am_i_in_loop(this);
}

int A_whileExp_::am_i_in_loop(AST_node_ *child) {
	return my_num;
}

int A_forExp_::am_i_in_loop(AST_node_ *child) {
	if (child == _lo or child == _hi) {
		return my_num;
	}
	return my_num;
}



int A_expList_::length() {
	if (_tail == 0) {
		return 1; 
	} else {
		return 1 + _tail->length();
	}
}

string A_callExp_::func_returnq(ST<type_info> tiger_library) {
	// Lookup function and get type_info struct
	type_info func_struct = lookup(_func, tiger_library);
	Ty_ty return_type = func_struct.my_return_type();
	if (return_type == Ty_Void()) {
		return "";
	} else {
		return "    LOAD(" + this->result_reg_s() + ", 3, FP_alt)\n";
	}
}
