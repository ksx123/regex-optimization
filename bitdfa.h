#ifndef __BITDFA_H               
#define __BITDFA_H

#include "dfa.h"

#pragma pack (1)

using namespace std;


class BitCmpDfSampleRange{
public:
	state_t target;
	unsigned char start;
	unsigned char end;
};

class BitCmpDfaBitmapRange{
public:
	state_t target;
	unsigned char bitmap[CHAR_BIRMAP_SIZE];
};

class BitCmpDfaEdage{
public:
	BitCmpDfaBitmapRange *data;
	unsigned lenght;
};

class BitCmpDfa
{
	protected:
		BitCmpDfaEdage* edges;
		linked_set **accepted_rules;
		unsigned size;
	public:
		BitCmpDfa(DFA* dfa);
		~BitCmpDfa();
		unsigned getSize();
		unsigned int getMemSize(); //B
		linked_set *accepts(state_t state);
		// linked_set* match(FILE *file);
		state_t getNext(state_t current, unsigned char input);
	protected:
		void forOneState(state_t s, state_t ** state_table);
};

inline unsigned BitCmpDfa::getSize(){ return this->size;}

inline linked_set *BitCmpDfa::accepts(state_t state) { return accepted_rules[state]; }

#endif /*__ECDFA_H*/