# Note there are some "sleep" commands here because otherwise Eclipse sometimes doesn't realize the files are there

CC=g++
CFLAGS=--std=c++1y -I/home/courses/include -Wall -Wno-sign-compare #that last due to auto-generated code from flex
LDFLAGS=-L/home/courses/lib -lcourses
REBUILD_AFTER_CLEAN=Debug/tiger.o

tiger-pre-build: lex.yy.cc tiger-grammar.tab.cc

tiger-grammar.tab.cc: tiger-grammar.yy ${REBUILD_AFTER_CLEAN}
	@rm tiger-grammar.tab.cc tiger-grammar.tab.hh 2>/dev/null || true
	bison tiger-grammar.yy

# I could get a .cc by using "%option c++", but that isn't what's done in the example
#  and it breaks a bunch of other stuff that I can't fix tonight :-(
lex.yy.cc: tiger-lex.ll ${REBUILD_AFTER_CLEAN}
	@rm lex.yy.c lex.yy.cc 2>/dev/null || true
	flex tiger-lex.ll
	mv lex.yy.c lex.yy.cc

# Something weird is causing Eclipse to be sad after a clean, but a 2nd build does the trick
# It looks like Eclipse checks "what should I build", then does the pre-build, and then builds,
#  so things created by the pre-build go "under the radar" until the 2nd build.
# As of January 2018, this might be an unresolved low-priority CDT bug, not going to be fixed soon?
#
# Doing "Project -> Clean", and then two builds usually works, except
#  that it doesn't always know to that a "clean project" should re-run flex and bison,
#  which I'd like it to do.
#
# KLUDGE: Make things depend on Debug/tiger.o, so that a make clean will (eventually) trigger it
#         (see setting and use of REBUILD_AFTER_CLEAN variable above)
# Note that this means occasional unnecessary re-bisoning/re-flexing, but it's better than
#   failing to re-run those steps when we actually wanted a clean.
# If I could figure out how to have Eclipse do a pre-clean step, that would be even better.

Debug/tiger.o:
	true

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
