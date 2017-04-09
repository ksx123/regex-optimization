#include "nmdfa.h"
#include "nfa.h"
#include "dfa.h"
#include "parser.h"

int **buildRulesMatrix(FILE *regex_file, regex_parser * parse, unsigned int size){
	int **rulesMat = (int **)allocate_array(size, sizeof(int*));
	NFA *nfa = NULL;
	DFA *dfa = NULL;
	int_set* line_nums = new int_set(size);
	for (int i = 0; i < size; ++i){
		rulesMat[i] = (int *)allocate_array(size, sizeof(int));
		for (int j = 0; j < i; ++j){
			rulesMat[i][j] = rulesMat[j][i];
		}
		for (int j = i+1; j < size; ++j){
			line_nums->clear();
			line_nums->insert(i);
			line_nums->insert(j);
			nfa = parse->parse_from_list(regex_file, line_nums);
			nfa->remove_epsilon();
			nfa->reduce();
			dfa=nfa->nfa2dfa();
			if (dfa==NULL) {
				printf("Max DFA size %ld exceeded during creation: the DFA was not generated\n",MAX_DFA_SIZE);
			}
			else {
				dfa->minimize();
				rulesMat[i][j] = dfa->size();
				delete dfa;
			}
			delete nfa;
		}
	}
	delete line_nums;

	return rulesMat;
}

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


NMDfaGroup* get_init_groups(int **rulesMat, unsigned int rule_size, int_set* processed_set, unsigned char cpu_num, unsigned int *group_size){
	NMDfaGroup* groups = new NMDfaGroup[cpu_num];
	*group_size = 0;
	for (int i = 0; i < cpu_num; ++i) {
		groups[i].isFull = false;
		groups[i].data = new int_set(rule_size);
		int min = -1;
		int min_index = -1;

		for (unsigned int i = 0; i < rule_size; ++i){
			if(!processed_set->mbr(i)){
				int sum = 0;
				for (int j = 0; j < rule_size; ++j){
					if(processed_set->mbr(j) && i != j){
						sum += rulesMat[i][j];
					}
				}
				if(min_index < 0 || sum < min){
					min_index = i;
					min = sum;
				}
			}
		}
		if(min_index==-1){
			break;
		}
		fprintf(stderr, "groups#%d:%d\n", i, min_index);
		groups[i].data->insert(min_index);
		processed_set->insert(min_index);
		(*group_size) = (*group_size) + 1;
	}

	return groups;
}

int get_next_rule(int ** rulesMat, unsigned int rule_size, int_set* processed_set, int_set* curret_group){
	int min = -1;
	int min_index = -1;

	for (unsigned int i = 0; i < rule_size; ++i){
		if(!processed_set->mbr(i)){
			int sum = 0;
			for (int j = 0; j < rule_size; ++j){
				if(curret_group->mbr(j)){
					sum += rulesMat[i][j];
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

bool checkCanAdd(FILE *regex_file, regex_parser * parse, NMDfaGroup * curret_group){
	NFA *nfa = parse->parse_from_list(regex_file, curret_group->data);
	nfa->remove_epsilon();
	nfa->reduce();
	DFA *dfa=nfa->nfa2dfa();
	bool canAdd = false;
	if (dfa==NULL) {
		printf("Max DFA size %ld exceeded during creation: the DFA was not generated\n",MAX_DFA_SIZE);
	}
	else {
		dfa->minimize();
		canAdd = dfa->size() < 200;
		delete dfa;
	}
	delete nfa;
	return canAdd;
}

NMDFA::NMDFA(FILE *file, regex_parser *parse, unsigned char cpu_num){
	unsigned int rule_size = getRuleSizeFromFile(file);
	int ** rulesMat = buildRulesMatrix(file, parse, rule_size);

	list<int_set*>* group_list = new list<int_set*>();

	int_set* processed_set = new int_set(rule_size);
	unsigned int curret_group_size;
	NMDfaGroup* groups = get_init_groups(rulesMat, rule_size, processed_set, cpu_num, &curret_group_size);

	while(processed_set->size() != rule_size){

		for(int groups_index = 0; groups_index < curret_group_size; groups_index++){
			if(!groups[groups_index].isFull){
				int next_rule = get_next_rule(rulesMat, rule_size, processed_set, groups[groups_index].data);
				if(checkCanAdd(file, parse, &groups[groups_index])){
					groups[groups_index].data->insert(next_rule);
					processed_set->insert(next_rule);
				}else{
					groups[groups_index].isFull = true;
				}
			}

			if (processed_set->size() == rule_size){
				break;
			}
		}

		bool allFinish = true;
		for(int groups_index = 0; groups_index < curret_group_size; groups_index++){
			if(!groups[groups_index].isFull){
				allFinish = false;
				break;
			}
		}

		if(allFinish){
			printf("allFinish\n");
			for(int groups_index = 0; groups_index < curret_group_size; groups_index++){
				group_list->push_back(groups[groups_index].data);
			}
			delete[] groups;
			groups = NULL;
			if(processed_set->size() != rule_size){
				groups = get_init_groups(rulesMat, rule_size, processed_set, cpu_num, &curret_group_size);
			}
			
		}
	}

	if(groups!=NULL){
		printf("deal rest\n");
		for(int groups_index = 0; groups_index < curret_group_size; groups_index++){
			group_list->push_back(groups[groups_index].data);
		}
		delete[] groups;
	}

	buildDfaList(group_list, file, parse);
}

void NMDFA::buildDfaList(list<int_set*>* group_list, FILE *regex_file, regex_parser *parse){
	DFA** dfa_list = new DFA*[group_list->size()];
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
			dfa_list[i] = dfa;
		}
		i++;
		delete nfa;
	}
	this->type = 1;
	this->group_size = group_list->size();
	this->data = dfa_list;
}


NMDFA::~NMDFA(){

}

unsigned int NMDFA::getSize() {
	unsigned int sum = 0;
	if(this->type==1){
		DFA **dfas = (DFA **)this->data;
		for (int i = 0; i < this->group_size; ++i) {
			sum += dfas[i]->size();
		}
	}
	
	return sum;
}

unsigned int NMDFA::getMemSize(){
	unsigned int sum = 0;
	if(this->type==1){
		DFA **dfas = (DFA **)this->data;
		for (int i = 0; i < this->group_size; ++i) {
			sum += dfas[i]->get_m_size();
		}
	}
	
	return sum;
}