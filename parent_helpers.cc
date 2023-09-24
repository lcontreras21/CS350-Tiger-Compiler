#include "AST.h"
#include "ST.h"

/*
	Functions used by *children to find out information about their parent
EG:
	break asking about earliest while or error
*/

//--------------------------------------------------------------------------------

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


//--------------------------------------------------------------------------------

int A_expList_::length() {
	if (_tail == 0) {
		return 1; 
	} else {
		return 1 + _tail->length();
	}
}

int A_decList_::length() {
	if (_tail == 0) {
		return 1;
	} else {
		return 1 + _tail->length();
	}
}

//--------------------------------------------------------------------------------
string A_callExp_::func_returnq(ST<function_info> tiger_library) {
	// Lookup function and get function_info struct
	if (is_name_there(_func, tiger_library)) {
		function_info func_struct = lookup(_func, tiger_library);
		Ty_ty return_type = func_struct.my_return_type();
		if (return_type == Ty_Void()) {
			return "";
		} else {
			return "    LOAD(" + this->result_reg_s() + ", 3, FP_alt)\n";
		}
	} else {
		EM_error("HERA_code: A_callExp: Check return: Function call for function " + Symbol_to_string(_func) + " not found in function library");
		return "";
	}
}

//--------------------------------------------------------------------------------


int AST_node_::calculate_my_SP(AST_node_ *_parent_or_child) {
	// Agnostic, so can only really traverse UP the tree
	// Should be fine?
	return stored_parent->calculate_my_SP(this); 
}

int A_root_::calculate_my_SP(AST_node_ *_parent_or_child) {
	// Should never be called? Through an error
	// EM_error("Tried to find SP value for root node. Should be inside a LET expression");
	return 0;
}

int A_callExp_::calculate_my_SP(AST_node_ *_parent_or_child) {
	if (_parent_or_child == _args) {
	   return 3 + _args->length(); 	
	}
	return 0;
}

int A_forExp_::calculate_my_SP(AST_node_ *_parent_or_child) {
	if (_parent_or_child == _body) {
		return 2 + stored_parent->calculate_my_SP(this);
	} else {
		return stored_parent->calculate_my_SP(this);
	}	
}

int A_letExp_::calculate_my_SP(AST_node_ *_parent_or_child) {
	// If called by _body, trying to find TOTAL SP value
	if (_parent_or_child == _body) {
		// Might not be any declarations 
		if (_decs == 0) {
			return stored_parent->calculate_my_SP(this);
		} else {
			return _decs->calculate_my_SP(this) + stored_parent->calculate_my_SP(this);;
		}
	} else {
		return stored_parent->calculate_my_SP(this);
	}
}

int A_decList_::calculate_my_SP(AST_node_ *_parent_or_child) {
	// If called by parent, should be a let, return the number of VarDecs
	if (stored_parent == _parent_or_child) {
		if (_tail == 0) {
			return _head->calculate_my_SP(this);
		} else {
			return _head->calculate_my_SP(this) + _tail->calculate_my_SP(this);
		}
	} else {
		// Two case if not called by parent, called by _head or by _tail
		// If by _head, start of getting the SP for storing in ST var_library
		// Else by _tail, _dec further down wants an SP count, return _head + parent SP check
		if (_parent_or_child == _head) {
			return stored_parent->calculate_my_SP(this);	
		} else {
			return _head->calculate_my_SP(this) + stored_parent->calculate_my_SP(this);
		}
	}
}

int A_varDec_::calculate_my_SP(AST_node_ *_parent_or_child) {
	// If called by parent, return 1. Someone wants its stack offset
	if (stored_parent == _parent_or_child) {
		return 1;
	} else {
	// Otherwise, go up the tree and find what THIS Vars stack offset is
		return stored_parent->calculate_my_SP(this);
	}
}

int A_fundec_::calculate_my_SP(AST_node_ *_parent_or_child) {
	if (_params != 0) {
		return 3 + _params->calculate_my_SP(this); 
	} else {
		return 3;
	}
}

int A_fieldList_::calculate_my_SP(AST_node_ *_parent_or_child) {
	if (_tail == 0) {
		return 1;
	} else {
		return 1 + _tail->calculate_my_SP(this);
	}
}

//--------------------------------------------------------------------------------

int AST_node_::am_i_in_assignExp_(AST_node_ *child) {
	return stored_parent->am_i_in_assignExp_(this);	
}

int A_root_::am_i_in_assignExp_(AST_node_ *child) {
	// Should this be an error?
	return -2;
}

int A_assignExp_::am_i_in_assignExp_(AST_node_ *child) {
	return _exp->result_reg();
}

int A_varExp_::am_i_in_assignExp_(AST_node_ *child) {
	return -1;
}

int A_simpleVar_::am_i_in_assignExp_(AST_node_ *child) {
	return stored_parent->am_i_in_assignExp_(this);
}

//--------------------------------------------------------------------------------

ST<function_info> AST_node_::get_my_funclib(AST_node_ *child) {
	// Return parent?
	return ST<function_info>();
}

ST<function_info> A_root_::get_my_funclib(AST_node_ *child) {
	// Return tiger_library
	return tiger_library;
}

ST<function_info> A_letExp_::get_my_funclib(AST_node_ *child) {
	return ST<function_info>();
}

ST<function_info> A_callExp_::get_my_funclib(AST_node_ *child) {
    return ST<function_info>();
}

ST<function_info> A_fundec_::get_my_funclib(AST_node_ *child) {
    return ST<function_info>();
}
