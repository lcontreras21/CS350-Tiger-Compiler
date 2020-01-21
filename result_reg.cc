#include "AST.h"

/*
 * methods for working with "result_reg" attribute
 */


static int next_unique_number = 1;

int A_exp_::init_result_reg()  // generate unique numbers, starting from 1, each time this is called
{
	// for those who've taken CS355/356, this should be an atomic transaction, in a concurrent environment
	int my_number = next_unique_number;
	next_unique_number = next_unique_number + 1;
	// end of atomic transaction
	return my_number;
}
