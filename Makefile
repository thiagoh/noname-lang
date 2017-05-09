CLASSDIR=.
SRC= noname.flex
CSRC= 
CGEN= noname-lex.cc noname-parse.cc src/lexer-utilities.cc src/noname-assignment-node.cc src/noname-ast-context.cc src/noname-binary-exp-node.cc src/noname-call-exp-node.cc src/noname-codegen-utils.cc src/noname-declaration-assignment-node.cc src/noname-declaration-node.cc src/noname-function-def-node.cc src/noname-main.cc src/noname-node-value.cc src/noname-top-level-exp-node.cc src/noname-types.cc src/noname-unary-exp-node.cc
LIBS=
CFIL= ${CSRC} ${CGEN}
LSRC= Makefile
OBJS= ${CFIL:.cc=.o}
OUTPUT= test.output
CPPINCLUDE= -I${CLASSDIR}/include -I/usr/local/opt/flex/include
# FLEX_FLAGS= -d -X -P noname_yy -o noname-lex.cc
# BISON_FLAGS= -d -v -y -b noname --debug -p noname_yy
#TO ENAMBLE DEBUG => FLEX_FLAGS= -d -X -o noname-lex.cc
FLEX_FLAGS= -X -P noname_yy -o noname-lex.cc
BISON_FLAGS= -d -v -t -y -b noname --debug
CC=g++
# CC=clang++
# CFLAGS= -g -Wall -Wno-unused -Wno-write-strings ${CPPINCLUDE}
# `llvm-config --cxxflags --ldflags --system-libs --libs core`
#CFLAGS= -g -std=c++11 `llvm-config --cxxflags` -Wall -Wno-unused -Wno-deprecated -Wno-write-strings ${CPPINCLUDE}
##
##
## How to use llvm-config
## llvm-config - Print LLVM compilation options
## http://releases.llvm.org/2.6/docs/CommandGuide/html/llvm-config.html
##
LLVM_MODULES= core mcjit native
CFLAGS= -g3 `llvm-config --cxxflags` -Wall -Wno-unused -Wno-deprecated -Wno-write-strings ${CPPINCLUDE}
LDFLAGS= -g3 `llvm-config --ldflags`
LDLIBS= `llvm-config --libfiles --system-libs --libs $(LLVM_MODULES)` -Wno-unused -Wno-deprecated ${CPPINCLUDE}
FLEX= flex ${FLEX_FLAGS}
BISON= bison ${BISON_FLAGS}
DEPEND = ${CC} -MM `llvm-config --cxxflags` ${CPPINCLUDE}

source : ${SRC} ${LSRC} ${LIBS} lsource

lsource: ${LSRC}

${OUTPUT}:lexer noname.nn
	@rm -f test.output
	-./lexer noname.nn >test.output 2>&1 

lexer: ${OBJS}
	${CC} $(LDFLAGS) $(CFLAGS) $(OBJS) -o lexer $(LDLIBS)

parser: ${OBJS}
	${CC} $(LDFLAGS) $(CFLAGS) $(OBJS) -o parser $(LDLIBS)

noname: ${OBJS} noname-lex.cc 
	${CC} $(LDFLAGS) $(CFLAGS) ${OBJS} -o noname $(LDLIBS)

%.o: %.cc 
	${CC} ${CFLAGS} -o $@ -c $<

# flex: noname-lex.cc

noname-lex.cc: noname.flex noname.tab.h
	${FLEX} noname.flex

noname.tab.c noname.tab.h: noname.y
	${BISON} noname.y
	mv -f noname.tab.c noname-parse.cc

dotest:	lexer noname.nn
	./lexer noname.nn

clean:
	-rm -f ${OUTPUT} *.s core ${OBJS} noname-*.d lexer noname-lex.cc noname.tab.c noname-parse.cc src/*.d src/*.o	noname.tab.h *~ parser cgen semant

clean-compile:
	@-rm -f core ${OBJS} noname-lex.cc

%.d: %.cc ${SRC} ${LSRC}
	${SHELL} -ec '${DEPEND} $< > $@'

#${SHELL} -ec '${DEPEND} $< | sed '\''s/\($*\.o\)[ :]*/\1 $@ : /g'\'' > $@'

-include ${CFIL:.cc=.d}