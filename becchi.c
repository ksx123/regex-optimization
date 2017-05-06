#include "becchi.h"
#include "nfa.h"
#include "dfa.h"
#include "ecdfa.h"
#include "parser.h"


unsigned int getRuleSizeFromFile(FILE *file){
	rewind(file);
	char *re = allocate_char_array(1000);
	int i = 0;
	int rule_size = 0;
	unsigned int c = fgetc(file);

	while(c != EOF){
		if (c == '\n' || c == '\r'){
			if(i != 0){
				re[i] = '\0';
				if (re[0] != '#'){
					rule_size++;
				} 
				i = 0;
				free(re);
				re=allocate_char_array(1000);
			}
		}else{
			re[i++] = c;
		}	
		c = fgetc(file);
	} //end while
	
	if(i != 0){
		re[i] = '\0';
		if (re[0] != '#'){
			rule_size++;
		}
	}
	free(re);

	return rule_size;
}

unsigned int getSingleSize(FILE *regex_file, regex_parser * parse, unsigned int index, unsigned int *single_sizes){
	if (!single_sizes[index]) {
		NFA *nfa = parse->parse(regex_file, index, index);
		nfa->remove_epsilon();
		nfa->reduce();
		DFA *dfa=nfa->nfa2dfa();
		if (dfa==NULL) {
			printf("Max DFA size %ld exceeded during creation: the DFA was not generated\n",MAX_DFA_SIZE);
		}
		else {
			dfa->minimize();
			single_sizes[index] = dfa->size();	
			delete dfa;
		}
		delete nfa;
	}
	return single_sizes[index];
}

bool checkCanAdd(FILE *regex_file, regex_parser * parse, int_set* group, unsigned int index, double ne, unsigned int *current_size, unsigned int *single_sizes){
	if(group->size()==0) {
		*current_size = getSingleSize(regex_file, parse, index, single_sizes);
		if(VERBOSE) printf("true:%2d %3d\n", index, *current_size);
		return true;
	}

	group->insert(index);
	NFA *nfa = parse->parse_from_list(regex_file, group);
	nfa->remove_epsilon();
	nfa->reduce();
	DFA *dfa=nfa->nfa2dfa();
	bool canAdd = false;
	if (dfa==NULL) {
		printf("Max DFA size %ld exceeded during creation: the DFA was not generated\n",MAX_DFA_SIZE);
	}
	else {
		dfa->minimize();
		if(dfa->size() <= ne * (*current_size + getSingleSize(regex_file, parse, index, single_sizes))){
			if(VERBOSE) printf("true:%2d %3d<=%lf*(%d+%d)=%lf\n", index, dfa->size(), ne, *current_size, getSingleSize(regex_file, parse, index, single_sizes), ne * (*current_size + getSingleSize(regex_file, parse, index, single_sizes)));
		
			canAdd = true;
			*current_size = dfa->size();
		}else{
			if(VERBOSE) printf("false:%2d %3d<=%lf*(%d+%d)=%lf\n", index, dfa->size(), ne, *current_size, getSingleSize(regex_file, parse, index, single_sizes), ne * (*current_size + getSingleSize(regex_file, parse, index, single_sizes)));
		}
		delete dfa;
	}
	delete nfa;
	group->remove(index);
	return canAdd;
}

Becchi::Becchi(FILE *file, regex_parser *parse, double ne, unsigned char type){
	unsigned int rule_size = getRuleSizeFromFile(file);
	unsigned int current_size;
	unsigned int single_sizes[1000];
	for (int i = 0; i < 1000; ++i) {
		single_sizes[i] = 0;
	}

	list<int_set*>* group_list = new list<int_set*>();
	int_set* processed_set = new int_set(rule_size);

	while(processed_set->size() != rule_size){
		int_set* group = new int_set(rule_size);
		current_size = 0;

		for(unsigned int rule_index = 1; rule_index <= rule_size; rule_index++){
			if(!processed_set->mbr(rule_index)){
				if(checkCanAdd(file, parse, group, rule_index, ne, &current_size, single_sizes)) {
					group->insert(rule_index);
					processed_set->insert(rule_index);
					if(VERBOSE) group->print();
				}
			}

			if (processed_set->size() == rule_size){
				break;
			}
		}
		group_list->push_back(group);
	}

	if(type==1){
		buildDfaList(group_list, file, parse);
	}else if(type==2){
		buildEcDfaList(group_list, file, parse);
	}
}


void Becchi::buildDfaList(list<int_set*>* group_list, FILE *regex_file, regex_parser *parse){
	DFA** dfa_list = new DFA*[group_list->size()];
	int i = 0;
	for(list<int_set*>::iterator it=group_list->begin(); it != group_list->end(); ++it) {
		if(VERBOSE) (*it)->print();
		NFA *nfa = parse->parse_from_list(regex_file, *it);
		nfa->remove_epsilon();
		nfa->reduce();
		DFA *dfa=nfa->nfa2dfa();
		if (dfa==NULL) {
			printf("Max DFA size %ld exceeded during creation: the DFA was not generated\n",MAX_DFA_SIZE);
		}
		else {
			dfa->minimize();
			dfa_list[i] = dfa;
		}
		i++;
		delete nfa;
	}
	this->type = 1;
	this->group_size = group_list->size();
	this->data = dfa_list;
}


void Becchi::buildEcDfaList(list<int_set*>* group_list, FILE *regex_file, regex_parser *parse){
	EgCmpDfa** dfa_list = new EgCmpDfa*[group_list->size()];
	int i = 0;
	for(list<int_set*>::iterator it=group_list->begin(); it != group_list->end(); ++it) {
		(*it)->print();
		NFA *nfa = parse->parse_from_list(regex_file, *it);
		nfa->remove_epsilon();
		nfa->reduce();
		DFA *dfa=nfa->nfa2dfa();
		if (dfa==NULL) {
			printf("Max DFA size %ld exceeded during creation: the DFA was not generated\n",MAX_DFA_SIZE);
		}
		else {
			dfa->minimize();
			dfa_list[i] = new EgCmpDfa(dfa);
			delete dfa;
		}
		i++;
		delete nfa;
	}
	this->type = 2;
	this->group_size = group_list->size();
	this->data = dfa_list;
}

Becchi::~Becchi(){

}

unsigned int Becchi::getSize() {
	unsigned int sum = 0;
	if(this->type==1){
		DFA **dfas = (DFA **)this->data;
		for (int i = 0; i < this->group_size; ++i) {
			sum += dfas[i]->size();
		}
	}else if(this->type==2){
		EgCmpDfa **dfas = (EgCmpDfa **)this->data;
		for (int i = 0; i < this->group_size; ++i) {
			sum += dfas[i]->getSize();
		}
	}
	
	return sum;
}

unsigned int Becchi::getMemSize(){
	unsigned int sum = 0;
	if(this->type==1){
		DFA **dfas = (DFA **)this->data;
		for (int i = 0; i < this->group_size; ++i) {
			sum += dfas[i]->get_m_size();
		}
	}else if(this->type==2){
		EgCmpDfa **dfas = (EgCmpDfa **)this->data;
		for (int i = 0; i < this->group_size; ++i) {
			sum += dfas[i]->getMemSize();
		}
	}
	return sum;
}