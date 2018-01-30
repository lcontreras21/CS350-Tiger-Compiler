# Note there are some "sleep" commands here because otherwise Eclipse sometimes doesn't realize the files are there

CC=g++
CFLAGS=--std=c++1y -I/home/courses/include -Wall -Wno-sign-compare #that last due to auto-generated code from flex
LDFLAGS=-L/home/courses/lib -lcourses

tiger-pre-build: lex.yy.cc tiger-grammar.tab.cc

tiger-grammar.tab.cc: tiger-grammar.yy
	@rm tiger-grammar.tab.cc tiger-grammar.tab.hh 2>/dev/null || true
	bison tiger-grammar.yy

# I could get a .cc by using "%option c++", but that isn't what's done in the example
#  and it breaks a bunch of other stuff that I can't fix tonight :-(
lex.yy.cc: tiger-lex.ll
	@rm lex.yy.c lex.yy.cc 2>/dev/null || true
	flex tiger-lex.ll
	mv lex.yy.c lex.yy.cc

# Something weird is causing Eclipse to be sad after a clean, but a 2nd build does the trick
# Possibly this has to do with renaming lex.yy.c, but I can't figure it out tonight.
# Cleaning here, then "Project -> Clean", does a build that doesn't compile lex.yy.cc
#  (the second build does just that, then succeeds.)
# Thus, tried a bogus line:
#  	touch -d"Jan 1 1970" lex.yy.cc
#  but no luck, it just got weirder.

clean:
	rm stack.hh tiger-grammar.tab.hh tiger-grammar.tab.cc lex.yy.cc lex.yy.c *~ 2>/dev/null  || true
	@echo " "
	@echo " "
	@echo " ===> CAUTION: First Eclipse build may fail; just do a second <==="
	@echo " "
	@echo " "

cleaner: clean
	rm Debug/*.o Debug/tiger 2>/dev/null || true

Debug:
	mkdir Debug

# This is not as efficient as a working, sophisticated makefile, or working Eclipse build,
#  but it should consistently work, works from the command line, and is very simple.
rebuilt: Debug
	bison tiger-grammar.yy
	flex tiger-lex.ll
	mv lex.yy.c lex.yy.cc
	rm Debug/*.o 2>/dev/null || true
	${CC} ${CFLAGS} *.cc ${LDFLAGS} -o Debug/tiger
