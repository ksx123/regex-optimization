#include "mdfa.h"

#define _MAX_SIZE 100


MDFA::MDFA(dfa_array dfas, nfa_array nfas, int size){
	unsigned int g[size][size];
	unsigned int V[size];
	for (int i = 0; i < size; ++i){
		for (int j = i + 1; j < size; ++j){
			g[i][j] = is_interaction(nfas[i], nfas[j], dfas[i]->size(), dfas[j]->size());
		}
	}
	for(int i=0;i<size;++i) V[i] = 1;
	
	while(has_items(V, size)){
		dfa_list* dl = new dfa_list();
		unsigned int in_set[size];
		for(int i=0;i<size;++i) in_set[i] = 0;
		while(!is_limited(dl)){
			int dfa_index = get_next_dfa(in_set, V, (unsigned int**)g, size);
			V[dfa_index] = 0;
			in_set[dfa_index] = 1;
			dl->push_back(dfas[dfa_index]);
		}
		dfa_groups.push_back(dl);
	}
}

int MDFA::is_interaction(NFA* a, NFA* b, int a_size, int b_size) {
	NFA * new_fa = new NFA();
	new_fa->link(a);
	new_fa->link(b);
	new_fa->remove_epsilon();
	new_fa->reduce();
	DFA* dfa = new_fa->nfa2dfa();
	dfa->minimize();
	return dfa->size() > (a_size + b_size);
}

int MDFA::has_items(unsigned int * V, int size) {
	for(int i = 0; i < size; ++i){
		if(V[i] == 1) 
			return 1;
	}
	return 0;
}

int MDFA::is_limited(dfa_list* dl) {
	int sum = 0;
	for(dfa_list::iterator it= dl->begin(); it!=dl->end();++it){
		sum += (*it)->size();
	}
	if(sum >= _MAX_SIZE) return 1;
	return 0;
}

int MDFA::get_next_dfa(unsigned int * in_set, unsigned int * V, unsigned int** E, int size){
	int min = -1;
	int min_index = -1;

	for (int i = 0; i < size; ++i){
		if(V[i]){
			int sum = 0;
			for (int j = 0; j < size; ++j){
				if(in_set[j]){
					sum += *(E+i*size+j);
				}
			}
			if(min_index < 0 || sum < min){
				min_index = i;
				min = sum;
			}
		}
	}
	return min_index;
}

