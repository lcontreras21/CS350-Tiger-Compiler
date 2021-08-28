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
	int left_reg = _left->init_result_reg();
	int right_reg = _right->init_result_reg();
	if (left_reg == right_reg) {
		return left_reg + 1;
	} else {
		return std::max(left_reg, right_reg);
	}
}

int A_callExp_::init_result_reg() {
	/* Have _args */
	/* Return highest register in expList 
	 Iterate through each _head and check
	 Can create pointer and go like that
	 */
	int max_reg = 0;
	A_expList pointer = _args;
	if (pointer == 0) {
		return max_reg;
	} else {
		while (pointer != 0) {
			int head_reg = pointer->_head->init_result_reg();
			if (head_reg > max_reg) {
				max_reg = head_reg;
			}
			pointer = pointer->_tail;
		}
		return max_reg;
	}
}

int A_ifExp_::init_result_reg() {
	// Sethi-Ullman with three instead of two --- ish
	// Return highest register
	if (_else_or_null == 0) {
		return std::max(_test->init_result_reg(), _then->init_result_reg());
	} else {
		return std::max(_test->init_result_reg(), std::max(_then->init_result_reg(), _else_or_null->init_result_reg()));
	}
}

int A_leafExp_::init_result_reg() {
   return min_reg;
} 


/* Old Code for naive register allocation
static int next_unique_number = 1;

int A_exp_::init_result_reg()  // generate unique numbers, starting from 1, each time this is called
{
	// for those who've taken CS355/356, this should be an atomic transaction, in a concurrent environment
	int my_number = next_unique_number;
	next_unique_number = next_unique_number + 1;
	// end of atomic transaction
	return my_number;
}

*/
