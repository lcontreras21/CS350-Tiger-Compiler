#include "AST.h"

int AST_node_::compute_height(){
	EM_error("Sorry, no height method for this node");
	return -666;
}

int A_leafExp_::compute_height(){
	return 0;
}


int A_opExp_::compute_height(){
	// This will be exponentially slower than is necessary,
	//   due to doing two calls where there could be one,
	//   AT EVERY LEVEL OF THE AST
	if (_left->compute_height() > _right->compute_height()) {
		return _left->compute_height() + 1;
	} else {
		return _right->compute_height() + 1;
	}
/*
        // This version avoids redundant calls within one function,
        //   but there could be redundancy between two different functions

	int rh = _right->compute_height();
	int lh =  _left->compute_height();
	// In this version, we've already computed the heights ...
	//   so, for an attribute that depends on, say "parent",
	//   we couldn't put the code above anywhere that might run
	//   before the parent pointers are set (e.g., as an initializer in the class)
	if (lh > rh) {
		return lh + 1;
	} else {
		return rh + 1;
	}
 */
/*
	// This version uses the lazy evaluation template in lazy.h to define a local variable.
	// It has the same asymptotic complexity as the above.

	lazy<int> rh = lazy<int>([this]() { return _right->compute_height(); });
	lazy<int> lh = lazy<int>([this]() { return  _left->compute_height(); });
	if (lh.get() > rh.get()) {
		return lh.get() + 1;
	} else {
		return rh.get() + 1;
	}	
*/
	// This version expects a height field in AST.h, defined a lazy data field,
	//  i.e., we'd add these two lines to class A_exp,
	//
	//	const lazy<int> height = lazy<int>([this]() { return this->compute_height(); });
	//
	//  (and still have the compute_height stuff for all classes)
/*
	if (_left->height.get() > _right->height.get()) {
		return _left->height.get() + 1;
	} else {
		return _right->height.get() + 1;
	}
*/
}
