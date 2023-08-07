#include "AST.h"

/*
 * methods for working with "result_reg" attribute
 */

/* Implementing the Sethi-Ullman Algorithm Here :)
Following:
The S-U label for each leaf is 1.
The S-U label for an interior node n depends on the labels of its two children (call these su1 and su2)
	if su1==su2, the result is su1+1
	otherwise, the result is max(su1, su2)
*/

/* Set min_reg to where we want sethi-ullman to be based off. At 4, we cna use first three regs for function calls */ 
int min_reg = 4;

int A_exp_::init_result_reg() {
	/* Node doesn't have method yet, return -1 */
	return -1;
}

int A_opExp_::init_result_reg() {
	/* Have _left, _right children */
	int left_reg = _left->result_reg();
	int right_reg = _right->result_reg();
	if (left_reg == right_reg) {
		return left_reg + 1;
	} else {
		return std::max(left_reg, right_reg);
	}
}

int A_expList_::init_result_reg() {
    int curr_value;
	if (_tail == 0) {
		curr_value = _head->result_reg();
	} else {
		curr_value = _tail->result_reg();
	}
	return curr_value;
}

int A_expList_::init_reg_usage() {
    int curr_value;
    int head_reg = _head ? _head->result_reg() : min_reg;
	if (_tail == 0) {
		curr_value = head_reg;
	} else {
		curr_value = std::max(head_reg, _tail->result_reg());
	}
	return std::max(curr_value, min_reg);
}

int A_callExp_::init_result_reg() {
	return _args ? _args->result_reg() : min_reg;
}

int A_ifExp_::init_result_reg() {
	// Sethi-Ullman with three instead of two --- ish
	// Return highest register
	if (_else_or_null == 0) {
		return std::max(_test->result_reg(), _then->result_reg());
	} else {
		return std::max(_test->result_reg(), std::max(_then->result_reg(), _else_or_null->result_reg()));
	}
}

int A_leafExp_::init_result_reg() {
   return min_reg;
} 

int A_seqExp_::init_result_reg() {
    return _seq ? _seq->result_reg() : min_reg;
}

int A_whileExp_::init_result_reg() {
	return std::max(_test->result_reg(), _body->result_reg());
}

int A_breakExp_::init_result_reg() {
	return 0;
}

int A_forExp_::init_result_reg() {
	return std::max(_lo->result_reg(), std::max(_hi->result_reg(), _body->result_reg()));
}

int A_varExp_::init_result_reg() {
	return min_reg;
}


int A_dec_::init_result_reg() {
	EM_error("Requesting init_result_reg for A_dec without a declared function");
	return -1;
}


int A_decList_::init_result_reg() {
    int curr_value;
    int head_reg = _head ? _head->result_reg() : min_reg;
	if (_tail == 0) {
		curr_value = head_reg;
	} else {
		curr_value = std::max(head_reg, _tail->result_reg());
	}
	return std::max(curr_value, min_reg);
}

int A_letExp_::init_result_reg() {
    int curr_value;
    int decs_reg = _decs ? _decs->result_reg() : min_reg;
	if (_body == 0) {
		curr_value = decs_reg;
	} else {
		curr_value = std::max(decs_reg, _body->reg_usage());
	}
	return std::max(curr_value, min_reg);

}

int A_varDec_::init_result_reg() {
	return _init->result_reg();
}

int A_assignExp_::init_result_reg() {
	return _exp->result_reg();	
}

int A_functionDec_::init_result_reg() {
	return min_reg;
}
