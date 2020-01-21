#include "AST.h"

int A_root_::compute_depth(){
	return 0;
}

int AST_node_::compute_depth(){
	return parent()->compute_depth()+1; // once "parent" works, this should do it
}

