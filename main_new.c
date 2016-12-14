
#include "stdinc.h"
// #include "nfa.h"
// #include "dfa.h"
// #include "hybrid_fa.h"
#include "parser.h"
// #include "trace.h"
#include "rcdfa.h"
#include "mdfa.h"

#include <sys/timeb.h>

int VERBOSE;
int DEBUG = 0;

void output_nfa(NFA* nfa, char * prefix, int i);
void output_dfa(DFA* dfa, char * prefix, int i);
void output_mdfa(MDFA* mdfa, char * prefix);

NFA* get_nfa_from_file(char const* file_name){
	FILE *regex_file=fopen(file_name, "r");
	regex_parser *parse=new regex_parser(false, false);
	NFA* nfa = parse->parse(regex_file);
	nfa->remove_epsilon();
 	nfa->reduce();
 	fclose(regex_file);
 	delete parse;
 	return nfa;
}

void doNFA(){
	NFA* nfa = get_nfa_from_file("_test/r_test");
	int state_size = nfa->size();

	int edge_count = 0;
	nfa_list *queue=new nfa_list();
	nfa->traverse(queue);
	FOREACH_LIST(queue, nit){
		edge_count += (*nit)->get_epsilon()->size();
		edge_count += (*nit)->get_transitions()->size();
	}
	delete queue;
	free(nfa);

	printf("NFA state size:%d, edge count:%d\n", state_size, edge_count);
}

int get_dfa_count(DFA* dfa, int * edge_count){
	int s_count = dfa->size();
	int e_count = 0;
	state_t ** s_table = dfa->get_state_table();
	for (int s = 0; s < s_count; ++s) {
		for (int c = 0; c < CSIZE; ++c){
			if(s_table[s][c]!=0){
				e_count++;
			}
		}
	}
	*edge_count = e_count;
	return s_count;
}

void doDFA(){
	NFA* nfa = get_nfa_from_file("_test/r_test");
	DFA* dfa = nfa->nfa2dfa();
	dfa->minimize();
	int edge_count;
	int state_size = get_dfa_count(dfa, &edge_count);

	printf("DFA state size:%d, edge count:%d\n", state_size, edge_count);
}

void doMDFA(){
	NFA *nfa=NULL; 
	DFA *dfa=NULL;
	int size;
	int i = 0;
	
	char * input_file = "_test/r_test";
	FILE *regex_file=fopen(input_file, "r");
	fprintf(stderr,"\nParsing the regular expression file %s ...\n", input_file);
	regex_parser *p = new regex_parser(false, false);

	list<NFA *>* nfa_list = p->parse_to_list(regex_file, &size);

	printf("size=%d\n", size);
	DFA ** dfas = (DFA **) allocate_array(size, sizeof(DFA*));
	NFA ** nfas = (NFA **) allocate_array(size, sizeof(NFA*));

	for(list<NFA *>::iterator it= nfa_list->begin(); it != nfa_list->end(); ++it){
		nfa = *it;
		nfa->remove_epsilon();
		nfa->reduce();
		dfa=nfa->nfa2dfa();
		dfa->minimize();
		dfas[i] = dfa;
		nfas[i] = nfa;
		i++;
	}

	MDFA * mdfa = new MDFA(dfas, nfas, size);
	mdfa->build();
	printf("%s\n", "build finish");
	int state_size = 0;
	int edge_count = 0;
	
	dfa_list* ds = mdfa->get_dfas();
	for (dfa_list::iterator i = ds->begin(); i != ds->end(); ++i) {
		DFA* dfa = *i;
		int tmp_e;
		state_size += get_dfa_count(dfa, &tmp_e);;
		edge_count += tmp_e;
	}

	printf("MDFA state size:%d, edge count:%d\n", state_size, edge_count);
	delete nfas;
	delete dfas;
	fclose(regex_file);
	delete p;
	delete mdfa;
}

void doGCFA(){
	struct timeb startTime , endTime;
	NFA *nfa=NULL; 
	DFA *dfa=NULL;
	int size;
	int i = 0;
	
	char * input_file = "_test/r_test";
	FILE *regex_file=fopen(input_file, "r");
	fprintf(stderr,"\nParsing the regular expression file %s ...\n", input_file);
	regex_parser *p = new regex_parser(false, false);
	// ftime(&startTime);
	list<NFA *>* nfa_list = p->parse_to_list(regex_file, &size);
	printf("size=%d\n", size);
	DFA ** dfas = (DFA **) allocate_array(size, sizeof(DFA*));
	NFA ** nfas = (NFA **) allocate_array(size, sizeof(NFA*));

	for(list<NFA *>::iterator it= nfa_list->begin(); it != nfa_list->end(); ++it){
		nfa = *it;
		nfa->remove_epsilon();
		nfa->reduce();
		dfa=nfa->nfa2dfa();
		dfa->minimize();
		dfas[i] = dfa;
		nfas[i] = nfa;
		// output_nfa(nfa, "_test/nfa", i);
		// output_dfa(dfa, "_test/dfa", i);
		i++;
	}

	MDFA * mdfa = new MDFA(dfas, nfas, size);
	mdfa->build();
	printf("%s\n", "build finish");
	
	int state_size = 0;
	int edge_count = 0;
	dfa_list* ds = mdfa->get_dfas();
	for (dfa_list::iterator i = ds->begin(); i != ds->end(); ++i) {
		DFA* dfa = *i;
		int tmp_e;
		state_size += get_dfa_count(dfa, &tmp_e);;
		edge_count += tmp_e;
	}
	printf("MDFA state size:%d, edge count:%d\n", state_size, edge_count);

	mdfa->toRCDFA();
	printf("%s\n", "toRCDFA finish");
	// ftime(&endTime);
	// printf("time = %d.%d\n",endTime.time - startTime.time, endTime.millitm - startTime.millitm);

	state_size = 0;
	edge_count = 0;
	ds = mdfa->get_dfas();
	for (dfa_list::iterator i = ds->begin(); i != ds->end(); ++i) {
		RCDFA* rcdfa = (RCDFA*)*i;
		state_size += rcdfa->size();

		list_re**  et = rcdfa->get_edges_table();
		for (int i = 0, ln = rcdfa->size(); i < ln; ++i)
		{
			edge_count += et[i]->size();
		}
	}
	printf("GCFA state size:%d, edge count:%d\n", state_size, edge_count);
	
	delete nfas;
	delete dfas;
	fclose(regex_file);
	delete p;

	// printf("\n按任意键继续...");
	// getchar();
	// ftime(&startTime);
	// FILE *input=fopen("x1_size_s0_p0.35.trace", "r");
	// int r = mdfa->match(input);
	// ftime(&endTime);
	// printf("r = %d, time = %d.%d\n",endTime.time - startTime.time, endTime.millitm - startTime.millitm);

	delete mdfa;
}

int main(int argc, char const *argv[])
{
	printf("%s argc=%d\n", "new main", argc);
	if(argc>1){
		if(strcmp(argv[1], "nfa")==0){
			doNFA();
		}else if (strcmp(argv[1], "dfa")==0){
			doDFA();
		}else if (strcmp(argv[1], "mdfa")==0){
			doMDFA();
		}else if (strcmp(argv[1], "gcfa")==0){
			doGCFA();
		}else {
			printf("%s\n", "UK type");
		}
	}
	return 0;
}

void output_nfa(NFA* nfa, char * prefix, int i){
	if(nfa!=NULL){
		char string[100];
		char out_file[100];
		sprintf(out_file,"%s_nfa%d", prefix, i);
		FILE *nfa_dot_file=fopen(out_file,"w");
		if(DEBUG) fprintf(stderr,"\nExporting to NFA DOT file %s ...\n", out_file);
		sprintf(string,"source: %s",prefix);
		nfa->to_dot(nfa_dot_file, string);
		fclose(nfa_dot_file);
	}
}

void output_dfa(DFA* dfa, char * prefix, int i){
	if (dfa!=NULL){
		char out_file[100];
		sprintf(out_file,"%s_dfa%d", prefix, i);
		FILE *dot_file=fopen(out_file ,"w");
		if(DEBUG) fprintf(stderr,"\nExporting to DOT file %s ...\n", out_file);
		char string[100];
		sprintf(string,"source: %s", prefix);
		dfa->to_dot(dot_file, string);
		fclose(dot_file);
	}
}

void output_mdfa(MDFA* mdfa, char * prefix){
	dfa_list* ds = mdfa->get_dfas();
	int ii = 0;
	for (dfa_list::iterator i = ds->begin(); i != ds->end(); ++i) {
		printf("dfa_list::iterator %d\n", ii);
		DFA* dfa = *i;
		output_dfa(dfa, prefix, ii++);
	}
}