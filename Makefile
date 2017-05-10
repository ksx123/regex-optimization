CC = /usr/bin/g++
#CFLAGS = -Wall -g -O4 -I..
CFLAGS = -w -g -O4 -I.. 

all:	main main_tracegen main_nfa main_dfas main_regexgen main_new exe

clean:
	-rm *.o regex* 

.c.o:
	${CC} ${CFLAGS} -c $*.c 

stdinc.o : stdinc.h 
int_set.o : stdinc.h int_set.h
linked_set.o : stdinc.h linked_set.h
wgraph.o : stdinc.h wgraph.h
partition.o : stdinc.h partition.h
dheap.o : stdinc.h dheap.h
dfa.o : stdinc.h dfa.h 
subset.o : stdinc.h subset.h dfa.h nfa.h
nfa.o : stdinc.h nfa.h dfa.h 
hybrid_fa.o : stdinc.h nfa.h dfa.h hybrid_fa.h
trace.o : stdinc.h trace.h 
parser.o :  stdinc.h nfa.h dfa.h parser.h

main: stdinc.o int_set.o linked_set.o dheap.o partition.o wgraph.o dfa.o nfa.o c_type.o ecdfa.o subset.o parser.o trace.o hybrid_fa.o main.o cache.o fa_memory.o dfas_memory.o
	${CC} ${CFLAGS} stdinc.o int_set.o linked_set.o partition.o wgraph.o dheap.o dfa.o nfa.o c_type.o ecdfa.o subset.o parser.o main.o trace.o hybrid_fa.o cache.o fa_memory.o dfas_memory.o -o regex 

main_dfas: stdinc.o int_set.o linked_set.o dheap.o partition.o wgraph.o cache.o nfa.o dfa.o c_type.o ecdfa.o  hybrid_fa.o subset.o parser.o trace.o dfas_memory.o fa_memory.o main_dfas.o 
	${CC} ${CFLAGS} stdinc.o int_set.o linked_set.o dheap.o partition.o wgraph.o cache.o nfa.o dfa.o c_type.o ecdfa.o hybrid_fa.o subset.o parser.o dfas_memory.o fa_memory.o main_dfas.o trace.o -o regex_dfas 

main_nfa: stdinc.o int_set.o linked_set.o dheap.o partition.o wgraph.o cache.o nfa.o dfa.o c_type.o ecdfa.o hybrid_fa.o subset.o parser.o trace.o dfas_memory.o fa_memory.o main_nfa.o 
	${CC} ${CFLAGS} stdinc.o int_set.o linked_set.o dheap.o partition.o wgraph.o cache.o nfa.o c_type.o dfa.o ecdfa.o hybrid_fa.o subset.o parser.o dfas_memory.o fa_memory.o main_nfa.o trace.o -o regex_nfa 
	
main_tracegen: stdinc.o int_set.o linked_set.o dheap.o partition.o wgraph.o cache.o nfa.o dfa.o c_type.o ecdfa.o hybrid_fa.o subset.o parser.o trace.o dfas_memory.o fa_memory.o main_tracegen.o 
	${CC} ${CFLAGS} stdinc.o int_set.o linked_set.o dheap.o partition.o wgraph.o cache.o nfa.o dfa.o c_type.o ecdfa.o hybrid_fa.o subset.o parser.o dfas_memory.o fa_memory.o main_tracegen.o trace.o -o regex_tracegen 

main_regexgen: stdinc.o main_regexgen.o 
	${CC} ${CFLAGS} stdinc.o main_regexgen.o -o regex_gen 

main_new: stdinc.o int_set.o linked_set.o dheap.o subset.o partition.o wgraph.o nfa.o dfa.o parser.o rcdfa.o mdfa.o hybrid_fa.o main_new.o 
	${CC} ${CFLAGS} stdinc.o int_set.o linked_set.o dheap.o subset.o partition.o wgraph.o nfa.o dfa.o parser.o rcdfa.o mdfa.o hybrid_fa.o main_new.o -o regex_new

main_test2: stdinc.o linked_set.o partition.o wgraph.o dheap.o subset.o nfa.o dfa.o int_set.o parser.o c_type.o ecdfa.o ecdfab.o rcdfa.o bitdfa.o rledfa.o orledfa.o main_test2.o
	${CC} ${CFLAGS} stdinc.o linked_set.o partition.o wgraph.o dheap.o subset.o nfa.o dfa.o int_set.o parser.o c_type.o ecdfa.o ecdfab.o rcdfa.o bitdfa.o rledfa.o orledfa.o main_test2.o -o regex_test2

main_nmdfa: stdinc.o int_set.o linked_set.o subset.o dheap.o partition.o wgraph.o nfa.o dfa.o c_type.o ecdfa.o parser.o nmdfa.o main_nmdfa.o
	${CC} ${CFLAGS} stdinc.o int_set.o linked_set.o subset.o dheap.o partition.o wgraph.o nfa.o dfa.o c_type.o ecdfa.o parser.o nmdfa.o main_nmdfa.o -o regex_nmdfa

main_becchi: stdinc.o int_set.o linked_set.o subset.o dheap.o partition.o wgraph.o nfa.o dfa.o c_type.o ecdfa.o parser.o becchi.o main_becchi.o
	${CC} ${CFLAGS} stdinc.o int_set.o linked_set.o subset.o dheap.o partition.o wgraph.o nfa.o dfa.o c_type.o ecdfa.o parser.o becchi.o main_becchi.o -o regex_becchi


exe:
	-chmod a+x regex*
