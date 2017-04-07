#ifndef __RLEDFA_H               
#define __RLEDFA_H

#include "dfa.h"

#pragma pack (1)

using namespace std;

class RleDfaSampleRange{
public:
	state_t target;
	unsigned char start;
	unsigned char end;
};

class RleDfaRleRangeItem{
public:
	state_t target;
	unsigned char end;
};

class RleDfaEdage{
public:
	RleDfaRleRangeItem *ranges;
	unsigned lenght;
	unsigned char bitmap[CHAR_BIRMAP_SIZE];
};

class RleDfa
{
	protected:
		RleDfaEdage* edges;
		linked_set **accepted_rules;
		unsigned size;
	public:
		RleDfa(DFA* dfa);
		~RleDfa();
		unsigned getSize();
		unsigned int getMemSize(); //B
		linked_set *accepts(state_t state);
		// linked_set* match(FILE *file);
		state_t getNext(state_t current, unsigned char input);
	protected:
		void forOneState(state_t s, state_t ** state_table);
};

inline unsigned RleDfa::getSize(){ return this->size;}

inline linked_set *RleDfa::accepts(state_t state) { return accepted_rules[state]; }

#endif /*__RLEDFA_H*/