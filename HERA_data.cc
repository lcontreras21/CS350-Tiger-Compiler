#include "AST.h"

/*
 * HERA_data methods
 */

const string indent_math = "    ";  // might want to use something different for, e.g., branches
int string_counter = 0;

string AST_node_::HERA_data()  // Default used during development; could be removed in final version 
{
	return "";
}

string A_root_::HERA_data() {
	return  main_expr->HERA_data(); 
}

string A_stringExp_::HERA_data() {
	count = string_counter; 
	string this_str_label = "string_" + std::to_string(string_counter);
	string_counter++;
	string output = "DLABEL(" + this_str_label + ")\n" + 
					indent_math + "LP_STRING(" + value + ")\n";
	return output; 
}

string A_callExp_::HERA_data() {
	return _args->HERA_data();
}

string A_expList_::HERA_data() {
	if (_tail == 0) {
		return _head->HERA_data();
	} else {
		return _head->HERA_data() + _tail->HERA_data(); 
	}
}
