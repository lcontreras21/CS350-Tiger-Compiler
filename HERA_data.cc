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
	string_counter++;
	string this_str_label = "string_" + std::to_string(count);
	string output = "DLABEL(" + this_str_label + ")\n" + 
					indent_math + "LP_STRING(" + value + ")\n";
	return output; 
}

string A_opExp_::HERA_data() {
	return _left->HERA_data() + _right->HERA_data();
}

string A_expList_::HERA_data() {
	if (_tail == 0) {
		return _head->HERA_data();
	} else {
		return _head->HERA_data() + _tail->HERA_data();
	}	
}

string A_callExp_::HERA_data() {
	if (_args != 0) {
		return _args->HERA_data();
	} else {
		return "";
	}
}

string A_ifExp_::HERA_data() {
	string output = _test->HERA_data() + _then->HERA_data();
	if (_else_or_null != 0) {
		output = output + _else_or_null->HERA_data();
	}
	return output;
}

string A_seqExp_::HERA_data() {
	// Iterate through A_expList _seq and add HERA_datas
	A_expList seq = _seq;
	string my_code = "";
	if (seq == 0) {
		return my_code;
	} else {
		while (seq != 0) {
			my_code = my_code + seq->_head->HERA_data();
			seq = seq->_tail;
		}
		return my_code;
	}
}

string A_whileExp_::HERA_data() {
	return _test->HERA_data() + _body->HERA_data();
}

string A_breakExp_::HERA_data() {
	return "";
}

string A_forExp_::HERA_data() {
	return _lo->HERA_data() + _hi->HERA_data() + _body->HERA_data();	
}

string A_varExp_::HERA_data() {
	return _var->HERA_data();
}

string A_simpleVar_::HERA_data() {
	return "";
}

string A_letExp_::HERA_data() {
	string output = "";
	if (_decs != 0) {
		output = output + _decs->HERA_data();
	} 
	if (_body != 0) {
		output = output + _body->HERA_data();
	}
	return output;
}

string A_decList_::HERA_data() {
	if (_tail == 0) {
		return _head->HERA_data();
	} else {
		return _head->HERA_data() + _tail->HERA_data();
	}
}


string A_varDec_::HERA_data() {
	return _init->HERA_data();
}
