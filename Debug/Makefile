SHELL   = /bin/sh
COURSES = /home/courses
# /Network/Servers/moe.cs.haverford.edu/Volumes/Users/courses
# For davew's laptop: COURSES = /Volumes/UFS/Users/davew/courses

# Look for source code above the Debug directory.
VPATH   = ..
FIND_HEADERS =                  # nothing needed here because of include's with ../ in .y and .l files
FILES_FOR_DEPEND = ../*.cc *.c  # set to *.cc if making in source dir

# Now on to the main stuff:

YACC    = bison -y -d -v --warnings=error
LEX     = flex

CC      = g++
CFLAGS  = -std=c++0x -pedantic -g ${FIND_HEADERS} -I${COURSES}/include -DYYMALLOC=malloc -DYYFREE=free -Dto_String_bad_brain=1
# or, with bison++:
# CFLAGS  = -pedantic -g ${FIND_HEADERS} -I${COURSES}/include -Dto_String_bad_brain=1
CCFLAGS = -Wall -Werror -Wno-unused-function ${CFLAGS}

LINKER  = g++
LDFLAGS = -L${COURSES}/lib -lcourses -lcourses -lstdc++ -lm

ATTRIBUTES = ./HERA_code.o ./my_reg.o
OBJ     = ${ATTRIBUTES} ./tiger-grammar.o ./tiger-lex.o ./tiger.o ./AST.o ./types.o ST.o ./AST-print.o ./AST-print-attributes.o ./errormsg.o ./util.o
OTHER_GEN = y.output y.tab.h tiger-grammar.c tiger

TARGET: tiger
all: tiger

# get rid of everything that's not under CVS control:
superclean:
	-/bin/rm ${OBJ} ${OTHER_GEN} .depend
	-/bin/rm ../HERA-C-Run-tmp* 2> /dev/null

# cannot do a full superclean or make will not build tiger-grammar.c 
# in a "make clean all" because it made it for .depend already and then killed it
clean:
	-/bin/rm ${OBJ} y.output
	-/bin/rm ../HERA-C-Run-tmp* 2> /dev/null

tiger : ${OBJ}
	${LINKER} $^ ${LDFLAGS} -o $@

%.o : %.cc
	${CC} ${CCFLAGS} -c $<

y.tab.h: tiger-grammar.c

# Thanks to Christina Oran's Makefile for the following:

.depend: y.tab.h
	-rm ../HERA-C-Run-tmp*.cc 
	g++ ${CCFLAGS} -MM ${FILES_FOR_DEPEND} > .depend

include .depend
