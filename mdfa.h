
#ifndef __MDFA_H               
#define __MDFA_H

#include "dfa.h"
#include "nfa.h"


typedef DFA** dfa_array;
typedef NFA** nfa_array;
typedef list<DFA*> dfa_list;

typedef pair<DFA*, NFA*> dfa_nfa;
typedef list<dfa_nfa*> dfa_nfa_list;

class MDFA {
protected:
	list<dfa_nfa_list*>* dfa_groups;
	dfa_list* dfas;
	
public:
	MDFA();
	MDFA(dfa_array dfas, nfa_array nfas, int size);
	MDFA(dfa_list* dfas);
	~MDFA();

	unsigned int get_m_size();

	void build();
	void toRCDFA();

	int match(char * str);
	int match(FILE *file);

	dfa_list* get_dfas();
private:
	int is_interaction(NFA* a, NFA* b, int a_size, int b_size);
	int has_items(unsigned int * V, int size);
	int is_limited(dfa_nfa_list* dnl);
	int get_next_dfa(unsigned int * in_set, unsigned int * V, unsigned int* E, int size);
};

inline dfa_list* MDFA::get_dfas() {return dfas;}



#endif /*__MDFA_H*/
