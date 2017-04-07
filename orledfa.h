#ifndef __ORLEDFA_H               
#define __ORLEDFA_H

#include "dfa.h"

#pragma pack (1)

using namespace std;

class ORleDfaSampleRange{
public:
	state_t target;
	unsigned char start;
	unsigned char end;
};

class ORleDfaRleRangeItem{
public:
	state_t target;
	unsigned char lenght;
};

class ORleDfaEdage{
public:
	ORleDfaRleRangeItem *ranges;
	unsigned lenght;
};

class ORleDfa
{
	protected:
		ORleDfaEdage* edges;
		linked_set **accepted_rules;
		unsigned size;
	public:
		ORleDfa(DFA* dfa);
		~ORleDfa();
		unsigned getSize();
		unsigned int getMemSize(); //B
		linked_set *accepts(state_t state);
		// linked_set* match(FILE *file);
		state_t getNext(state_t current, unsigned char input);
	protected:
		void forOneState(state_t s, state_t ** state_table);
};

inline unsigned ORleDfa::getSize(){ return this->size;}

inline linked_set *ORleDfa::accepts(state_t state) { return accepted_rules[state]; }

#endif /*__ORLEDFA_H*/