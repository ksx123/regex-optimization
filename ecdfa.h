#ifndef __ECDFA_H               
#define __ECDFA_H

#include "dfa.h"
#include <list>

#pragma pack (1)

using namespace std;

class EgCmpDfaEdage{
public:
	void *data;
	unsigned lenght;
	unsigned char type;
};

class EgCmpDfaSampleRange{
public:
	state_t target;
	unsigned char start;
	unsigned char end;
};

class EgCmpDfaBitmapRange{
public:
	state_t target;
	unsigned char bitmap[CHAR_BIRMAP_SIZE];
};

class EgCmpDfaRleRangeItem{
public:
	state_t target;
	unsigned char end;
};

class EgCmpDfaRleRange{
public:
	unsigned char bitmap[CHAR_BIRMAP_SIZE];
	EgCmpDfaRleRangeItem * ranges;
};

class EgCmpDfaHasBitEdage{
public:
	EgCmpDfaBitmapRange * bit_ranges;
	unsigned lenght;
	void *other_ranges;
};

class EgCmpDfa
{
	protected:
		EgCmpDfaEdage* edges;
		linked_set **accepted_rules;
		unsigned size;
	public:
		EgCmpDfa(DFA* dfa);
		~EgCmpDfa();
		unsigned getSize();
		unsigned int getMemSize(); //B
		linked_set *accepts(state_t state);
		linked_set* match(FILE *file);
		state_t getNext(state_t current, unsigned char input);
	protected:
		void forOneState(state_t s, state_t ** state_table);
};

inline unsigned EgCmpDfa::getSize(){ return this->size;}

inline linked_set *EgCmpDfa::accepts(state_t state) { return accepted_rules[state]; }

#endif /*__ECDFA_H*/