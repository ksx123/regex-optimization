#include "mdfa.h"
#include "rcdfa.h"

#define _MAX_SIZE 300


MDFA::MDFA(dfa_array dfas, nfa_array nfas, int size){
	unsigned int g[size][size];
	unsigned int V[size];
	for (int i = 0; i < size; ++i){
		g[i][i] = 0;
		for (int j = i + 1; j < size; ++j){
			g[i][j] = is_interaction(nfas[i], nfas[j], dfas[i]->size(), dfas[j]->size());
			g[j][i] = g[i][j];
		}
	}
	// for (int i = 0; i < size; ++i){
	// 	for (int j = 0; j < size; ++j){
	// 		printf("%d ", g[i][j]);
	// 	}
	// 	printf("\n");
	// }
	for(int i=0;i<size;++i) V[i] = 1;
	
	dfa_groups = new list<dfa_nfa_list*>();
	while(has_items(V, size)){
		dfa_nfa_list* dnl = new dfa_nfa_list();
		unsigned int in_set[size];
		for(int i=0;i<size;++i) in_set[i] = 0;
		int isFirst = 1;
		while(!is_limited(dnl) && has_items(V, size)){
			int dfa_index = get_next_dfa(in_set, V, (unsigned int*)g, size);
			if(isFirst){
			   dfa_index = get_init_dfa(V, (unsigned int*)g, size);
			   isFirst = 0;
			}
			// printf("dfa_index:%d\n", dfa_index);
			V[dfa_index] = 0;
			in_set[dfa_index] = 1;
			dfa_nfa *p = new dfa_nfa();
			p->first = dfas[dfa_index];
			p->second = nfas[dfa_index];
			dnl->push_back(p);
		}
		// printf("-----------------------------\n");
		dfa_groups->push_back(dnl);
	}
	this->dfas = NULL;
}
MDFA::MDFA(){
	dfa_groups = NULL;
	dfas = NULL;
}

MDFA::MDFA(dfa_list* dfas){
	this->dfa_groups = NULL;
	this->dfas = dfas;
}

MDFA::~MDFA() {
	if(dfa_groups!=NULL){
		delete dfa_groups;
	}
	if(dfas != NULL){
		for (dfa_list::iterator i = dfas->begin(); i != dfas->end(); ++i) {
				delete *i;
		}
		delete dfas;
	}
}

unsigned int MDFA::get_m_size(){
	unsigned int size_count = 0;
	if(dfas != NULL){
		for (dfa_list::iterator i = dfas->begin(); i != dfas->end(); ++i) {
				size_count += (*i)->get_m_size();
		}
		size_count += sizeof(dfas) + dfas->size() * (16 + sizeof(DFA*)) + 24;
	}
	return size_count;
}

void MDFA::build() {
	printf("_MAX_SIZE:%d\n", _MAX_SIZE);
	dfas = new dfa_list();
	unsigned int index = 0;
	for(list<dfa_nfa_list*>::iterator it=dfa_groups->begin(); it!=dfa_groups->end(); ++it){
		dfa_nfa_list* ds = (*it);
		printf("#group%d: %d\n", index++, ds->size());
		NFA * new_fa = NULL;
		for(dfa_nfa_list::iterator iit=ds->begin(); iit!=ds->end(); ++iit){
			if(new_fa == NULL){
				new_fa = (*iit)->second;
			}else{
				new_fa = new_fa->make_or((*iit)->second);
			}
			delete (*iit)->first;
		}
		new_fa = new_fa->get_first();
		new_fa->remove_epsilon();
		new_fa->reduce();
		DFA* dfa = new_fa->nfa2dfa();
		dfa->minimize();
		dfas->push_back(dfa);
		delete new_fa;
	}
	delete dfa_groups;
	dfa_groups = NULL;
}

void MDFA::toRCDFA() {
	dfa_list * ds = new dfa_list();
	for(dfa_list::iterator i = dfas->begin(); i != dfas->end(); ++i){
		RCDFA * rcdfa = new RCDFA(*i);
		ds->push_back(rcdfa);
		delete *i;
	}
	delete dfas;
	dfas = ds;
}

int MDFA::match(FILE *file){
	int sum = 0;
	for(dfa_list::iterator i = dfas->begin(); i != dfas->end(); ++i){
		sum += (*i)->match(file);
	}
	return sum;
}

int MDFA::match(char * str){
	int sum = 0;
	for(dfa_list::iterator i = dfas->begin(); i != dfas->end(); ++i){
		sum += (*i)->match(str);
	}
	return sum;
}

int MDFA::is_interaction(NFA* a, NFA* b, int a_size, int b_size) {
	NFA* a_c = a->clone();
	NFA* b_c = b->clone();
	NFA * new_fa = a_c->make_or(b_c);
	new_fa = new_fa->get_first();
	new_fa->remove_epsilon();
	new_fa->reduce();
	DFA* dfa = new_fa->nfa2dfa();
	dfa->minimize();
	int r = dfa->size() > (a_size + b_size);
	delete new_fa;
	delete dfa;
	return r;
}

int MDFA::has_items(unsigned int * V, int size) {
	for(int i = 0; i < size; ++i){
		if(V[i] == 1) 
			return 1;
	}
	return 0;
}

int MDFA::is_limited(dfa_nfa_list* dnl) {
	int sum = 0;
	for(dfa_nfa_list::iterator it=dnl->begin(); it!=dnl->end();++it){
		sum += (*it)->first->size();
	}
	if(sum >= _MAX_SIZE) return 1;
	return 0;
}

int MDFA::get_next_dfa(unsigned int * in_set, unsigned int * V, unsigned int* E, int size){
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
			// printf("\t%d sum:%d\n", i, sum);
			if(min_index < 0 || sum < min){
				min_index = i;
				min = sum;
			}
		}
	}
	return min_index;
}

int MDFA::get_init_dfa(unsigned int * V, unsigned int* E, int size){
	int min = -1;
	int min_index = -1;

	for (int i = 0; i < size; ++i){
		if(V[i]){
			int sum = 0;
			for (int j = 0; j < size; ++j){
				if(V[j]){
					sum += *(E+i*size+j);
				}
			}
			// printf("get_init_dfa:%d sum:%d\n", i, sum);
			if(min_index < 0 || sum < min){
				min_index = i;
				min = sum;
			}
		}
	}
	return min_index;
}

