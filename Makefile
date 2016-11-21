CC = /usr/bin/g++
#CFLAGS = -Wall -g -O4 -I..
CFLAGS = -w -g -O4 -I.. 

all:	main main_tracegen main_nfa main_dfas main_regexgen exe

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

main: stdinc.o int_set.o linked_set.o dheap.o partition.o wgraph.o dfa.o nfa.o subset.o parser.o trace.o hybrid_fa.o main.o cache.o fa_memory.o dfas_memory.o
	${CC} ${CFLAGS} stdinc.o int_set.o linked_set.o partition.o wgraph.o dheap.o dfa.o nfa.o subset.o parser.o main.o trace.o hybrid_fa.o cache.o fa_memory.o dfas_memory.o -o regex 

main_dfas: stdinc.o int_set.o linked_set.o dheap.o partition.o wgraph.o cache.o nfa.o dfa.o hybrid_fa.o subset.o parser.o trace.o dfas_memory.o fa_memory.o main_dfas.o 
	${CC} ${CFLAGS} stdinc.o int_set.o linked_set.o dheap.o partition.o wgraph.o cache.o nfa.o dfa.o hybrid_fa.o subset.o parser.o dfas_memory.o fa_memory.o main_dfas.o trace.o -o regex_dfas 

main_nfa: stdinc.o int_set.o linked_set.o dheap.o partition.o wgraph.o cache.o nfa.o dfa.o hybrid_fa.o subset.o parser.o trace.o dfas_memory.o fa_memory.o main_nfa.o 
	${CC} ${CFLAGS} stdinc.o int_set.o linked_set.o dheap.o partition.o wgraph.o cache.o nfa.o dfa.o hybrid_fa.o subset.o parser.o dfas_memory.o fa_memory.o main_nfa.o trace.o -o regex_nfa 
	
main_tracegen: stdinc.o int_set.o linked_set.o dheap.o partition.o wgraph.o cache.o nfa.o dfa.o hybrid_fa.o subset.o parser.o trace.o dfas_memory.o fa_memory.o main_tracegen.o 
	${CC} ${CFLAGS} stdinc.o int_set.o linked_set.o dheap.o partition.o wgraph.o cache.o nfa.o dfa.o hybrid_fa.o subset.o parser.o dfas_memory.o fa_memory.o main_tracegen.o trace.o -o regex_tracegen 

main_regexgen: stdinc.o main_regexgen.o 
	${CC} ${CFLAGS} stdinc.o main_regexgen.o -o regex_gen 

exe:
	-chmod a+x regex*
