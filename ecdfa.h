#ifndef __ECDFA_H               
#define __ECDFA_H

#include "dfa.h"
#include <list>

#define CHAR_BIRMAP_SIZE 32

#pragma pack (1)

using namespace std;

class EgCmpDfaEdage{
public:
	void *data;
	unsigned lenght;
	char type;
};

class EgCmpDfaSampleRange{
public:
	state_t target;
	char start;
	char end;
};

class EgCmpDfaBitmapRange{
public:
	state_t target;
	char bitmap[CHAR_BIRMAP_SIZE];
};

class EgCmpDfaRleRangeItem{
public:
	state_t target;
	char end;
};

class EgCmpDfaRleRange{
public:
	char bitmap[CHAR_BIRMAP_SIZE];
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
		linked_set* match(FILE *file);
	protected:
		void forOneState(state_t s, state_t ** state_table);
		state_t getNext(state_t current, char input);
};

#endif /*__ECDFA_H*/