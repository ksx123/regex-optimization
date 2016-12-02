
#ifndef __MDFA_H               
#define __MDFA_H

#include "dfa.h"
#include "nfa.h"


typedef DFA** dfa_array;
typedef NFA** nfa_array;
typedef list<DFA*> dfa_list;

class MDFA {
protected:
	list<dfa_list*> dfa_groups;

	
public:
	MDFA(dfa_array dfas, nfa_array nfas, int size);


private:
	int is_interaction(NFA* a, NFA* b, int a_size, int b_size);
	int has_items(unsigned int * V, int size);
	int is_limited(list<DFA*>* dl);
	int get_next_dfa(unsigned int * in_set, unsigned int * V, unsigned int** E, int size);
};




#endif /*__MDFA_H*/
