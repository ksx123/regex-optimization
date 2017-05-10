#ifndef __ECDFAB_H               
#define __ECDFAB_H

#include "ecdfa.h"

#pragma pack (1)

using namespace std;

class EgCmpDfaB
{
	protected:
		void **edges;
		unsigned char *types;
		linked_set **accepted_rules;
		unsigned size;
	public:
		EgCmpDfaB(DFA* dfa);
		~EgCmpDfaB();
		unsigned getSize();
		unsigned int getMemSize(); //B
		linked_set *accepts(state_t state);
		// linked_set* match(FILE *file);
		// state_t getNext(state_t current, unsigned char input);
	protected:
		void forOneState(state_t s, state_t ** state_table);
};

inline unsigned EgCmpDfaB::getSize(){ return this->size;}

inline linked_set *EgCmpDfaB::accepts(state_t state) { return accepted_rules[state]; }

#endif /*__ECDFAB_H*/