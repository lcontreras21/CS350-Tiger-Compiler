all: lex.yy.cc tiger-grammar.tab.cc

tiger-grammar.tab.cc: tiger-grammar.yy
	bison tiger-grammar.yy

lex.yy.cc: tiger-lex.ll
	flex tiger-lex.ll

clean:
	-rm stack.hh tiger-grammar.tab.hh tiger-grammar.tab.cc lex.yy.cc
