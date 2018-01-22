all: lex.yy.cc tiger-grammar.tab.cc

tiger-grammar.tab.cc: tiger-grammar.yy
	bison tiger-grammar.yy

# I could get a .cc by using "%option c++", but that isn't what's done in the example
#  and it breaks a bunch of other stuff that I can't fix tonight :-(
lex.yy.cc: tiger-lex.ll
	flex tiger-lex.ll
	mv lex.yy.c lex.yy.cc

clean:
	-rm stack.hh tiger-grammar.tab.hh tiger-grammar.tab.cc lex.yy.cc lex.yy.c
