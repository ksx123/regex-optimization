
#include "stdinc.h"
// #include "nfa.h"
// #include "dfa.h"
#include "hybrid_fa.h"
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

void doNFA(char const * re_file_name){
	struct timeb startTime , endTime;
	ftime(&startTime);
	NFA* nfa = get_nfa_from_file(re_file_name);
	ftime(&endTime);
	printf("time = %dms\n",(endTime.time*1000+endTime.millitm) - (startTime.time*1000 + startTime.millitm));
	printf("\n按任意键继续...");
	getchar();

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

void doDFA(char const * re_file_name){
	NFA* nfa;
	DFA* dfa;
	int edge_count=0;
	int state_size=0;

	struct timeb startTime , endTime;
	ftime(&startTime);
	FILE *regex_file=fopen(re_file_name, "r");
	fprintf(stderr,"\nParsing the regular expression file %s ...\n", re_file_name);
	regex_parser *p = new regex_parser(false, false);

	int size;
	list<NFA *>* nfa_list = p->parse_to_list(regex_file, &size);
	list<DFA *>* dfa_list = new  list<DFA*>;
	for(list<NFA *>::iterator it= nfa_list->begin(); it != nfa_list->end(); ++it){
		nfa = *it;
		nfa->remove_epsilon();
		nfa->reduce();
		dfa=nfa->nfa2dfa();
		dfa->minimize();
		printf("%lu\n", dfa->get_m_size());
		dfa_list->push_back(dfa);
		delete nfa;
		int tmp_count;
		state_size += get_dfa_count(dfa, &tmp_count);
		edge_count += tmp_count;
	}
	delete nfa_list;
	ftime(&endTime);
	printf("time = %dms\n",(endTime.time*1000+endTime.millitm) - (startTime.time*1000 + startTime.millitm));
	printf("\n按任意键继续...");
	getchar();

	printf("DFA state size:%d, edge count:%d\n", state_size, edge_count);
}

void doHFA(char const * re_file_name){
	NFA* nfa;
	int state_count = 0;
	int edge_count = 0;

	struct timeb startTime , endTime;
	ftime(&startTime);
	FILE *regex_file=fopen(re_file_name, "r");
	fprintf(stderr,"\nParsing the regular expression file %s ...\n", re_file_name);
	regex_parser *p = new regex_parser(false, false);

	int size;
	list<NFA *>* nfa_list = p->parse_to_list(regex_file, &size);
	list<HybridFA*>* hfa_list = new list<HybridFA*>;
	for(list<NFA *>::iterator it= nfa_list->begin(); it != nfa_list->end(); ++it){
		nfa = *it;
		nfa->remove_epsilon();
		nfa->reduce();
		HybridFA* hfa=new HybridFA(nfa);
		hfa_list->push_back(hfa);
		int tmp_count;
		state_count += get_dfa_count(hfa->get_head(), &tmp_count);
		edge_count += tmp_count;
	}
	delete nfa_list;
	ftime(&endTime);
	printf("time = %dms\n",(endTime.time*1000+endTime.millitm) - (startTime.time*1000 + startTime.millitm));
	printf("\n按任意键继续...");
	getchar();
}

void doMDFA(char const * re_file_name){
	NFA *nfa=NULL; 
	DFA *dfa=NULL;
	int size;
	int i = 0;
	
	struct timeb startTime , endTime;
	ftime(&startTime);
	FILE *regex_file=fopen(re_file_name, "r");
	fprintf(stderr,"\nParsing the regular expression file %s ...\n", re_file_name);
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
	printf("MDFA gSize:%d, m_siz:%lu\n", ds->size(), mdfa->get_m_size());

	free(nfas);
	free(dfas);
	fclose(regex_file);
	delete p;
	ftime(&endTime);
	printf("time = %dms\n",(endTime.time*1000+endTime.millitm) - (startTime.time*1000 + startTime.millitm));
	printf("\n按任意键继续...");
	getchar();
	delete mdfa;
}

void doGCFA(char const * re_file_name){
	NFA *nfa=NULL; 
	DFA *dfa=NULL;
	int size;
	int i = 0;
	
	struct timeb startTime , endTime;
	ftime(&startTime);
	FILE *regex_file=fopen(re_file_name, "r");
	fprintf(stderr,"\nParsing the regular expression file %s ...\n", re_file_name);
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
	ftime(&endTime);
	printf("time = %dms\n",(endTime.time*1000+endTime.millitm) - (startTime.time*1000 + startTime.millitm));
	delete nfas;
	delete dfas;
	delete nfa_list;
	fclose(regex_file);
	delete p;
	printf("\n按任意键继续...");
	getchar();

	// FILE *input=fopen("../test_data1", "r");
	// ftime(&startTime);
	// int r = mdfa->match(input);
	// ftime(&endTime);
	// printf("r = %d, time = %d\n", r, (endTime.time*1000+endTime.millitm) - (startTime.time*1000 + startTime.millitm));
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
	printf("MDFA m_siz:%lu\n", mdfa->get_m_size());

	// ftime(&startTime);
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
	printf("GCFA m_siz:%lu\n", mdfa->get_m_size());
	printf("\n按任意键继续...");
	getchar();
	// ftime(&startTime);
	// FILE *input=fopen("x1_size_s0_p0.35.trace", "r");
	// r = mdfa->match(input);
	// ftime(&endTime);
	// printf("r = %d, time = %d\n", r, (endTime.time*1000+endTime.millitm) - (startTime.time*1000 + startTime.millitm));

	delete mdfa;
	printf("\n按任意键继续...");
	getchar();
}

int main(int argc, char const *argv[])
{
	printf("%s argc=%d\n", "new main", argc);
	if(argc>2){
		printf("re file:%s\n", argv[1]);
		if(strcmp(argv[2], "nfa")==0){
			doNFA(argv[1]);
		}else if (strcmp(argv[2], "dfa")==0){
			doDFA(argv[1]);
		}else if (strcmp(argv[2], "hfa")==0){
			doHFA(argv[1]);
		}else if (strcmp(argv[2], "mdfa")==0){
			doMDFA(argv[1]);
		}else if (strcmp(argv[2], "gcfa")==0){
			doGCFA(argv[1]);
		}else {
			printf("%s\n", "UK type");
		}
	}
	// printf("\nEnd\n");
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