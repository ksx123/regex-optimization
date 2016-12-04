
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

int main(int argc, char const *argv[])
{
try{
	printf("%s\n", "new main");

	NFA *nfa=NULL; 
	DFA *dfa=NULL;
	struct timeb startTime , endTime;

	char * input_file = "_test/r_test";
	FILE *regex_file=fopen(input_file, "r");
	fprintf(stderr,"\nParsing the regular expression file %s ...\n", input_file);
	regex_parser *p = new regex_parser(false, false);
	int size;
	ftime(&startTime);
	list<NFA *>* nfa_list = p->parse_to_list(regex_file, &size);
	printf("size=%d\n", size);
	DFA ** dfas = (DFA **) allocate_array(size, sizeof(DFA*));
	NFA ** nfas = (NFA **) allocate_array(size, sizeof(NFA*));
	int i = 0;

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
	printf("%p\n", mdfa);
	mdfa->build();
	printf("%s\n", "build finish");
	// output_mdfa(mdfa, "_test/mdfa1");
	mdfa->toRCDFA();
	printf("%s\n", "toRCDFA finish");
	ftime(&endTime);
	printf("time = %d.%d\n",endTime.time - startTime.time, endTime.millitm - startTime.millitm);
	// output_mdfa(mdfa, "_test/mdfa2");
	printf("%s\n", "output_mdfa end");
    ftime(&startTime);
    FILE *input=fopen("xx_depth_s0_p0.35.trace", "r");
    int r = mdfa->match(input);
    ftime(&endTime);
	printf("r = %d, time = %d.%d\n",endTime.time - startTime.time, endTime.millitm - startTime.millitm);
	delete nfas;
	delete dfas;

	printf("\n按任意键继续...");
	getchar();

	delete mdfa;
}catch(double d){
	printf("%s: %lf\n", "error", d);
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

// int main(int argc, char const *argv[])
// {

// 	printf("%s\n", "new main");

// 	NFA *nfa=NULL; 
// 	DFA *dfa=NULL;

// 	char * input_file = "_test/txt_new";
// 	FILE *regex_file=fopen(input_file, "r");
// 	fprintf(stderr,"\nParsing the regular expression file %s ...\n", input_file);
// 	regex_parser *p = new regex_parser(false, false);
// 	nfa = p->parse(regex_file);
// 	if(nfa!=NULL){
// 			char string[100];
// 			char out_file[100];
// 			sprintf(out_file,"%s_nfa1", input_file);
// 			FILE *nfa_dot_file=fopen(out_file,"w");
// 			fprintf(stderr,"\nExporting to NFA DOT file %s ...\n", out_file);
// 			sprintf(string,"source: %s",input_file);
// 			nfa->to_dot(nfa_dot_file, string);
// 			fclose(nfa_dot_file);
// 	}
// 	nfa->remove_epsilon();
// 	if(nfa!=NULL){
// 			char string[100];
// 			char out_file[100];
// 			sprintf(out_file,"%s_nfa2", input_file);
// 			FILE *nfa_dot_file=fopen(out_file,"w");
// 			fprintf(stderr,"\nExporting to NFA DOT file %s ...\n", out_file);
// 			sprintf(string,"source: %s",input_file);
// 			nfa->to_dot(nfa_dot_file, string);
// 			fclose(nfa_dot_file);
// 	}
// 	nfa->reduce();
// 	if(nfa!=NULL){
// 			char string[100];
// 			char out_file[100];
// 			sprintf(out_file,"%s_nfa2", input_file);
// 			FILE *nfa_dot_file=fopen(out_file,"w");
// 			fprintf(stderr,"\nExporting to NFA DOT file %s ...\n", out_file);
// 			sprintf(string,"source: %s",input_file);
// 			nfa->to_dot(nfa_dot_file, string);
// 			fclose(nfa_dot_file);
// 	}
// 	dfa=nfa->nfa2dfa();
// 	if (dfa!=NULL){
// 		char out_file[100];
// 		sprintf(out_file,"%s_dfa", input_file);
// 		FILE *dot_file=fopen(out_file ,"w");
// 		fprintf(stderr,"\nExporting to DOT file %s ...\n", out_file);
// 		char string[100];
// 		sprintf(string,"source: %s", input_file);
// 		dfa->to_dot(dot_file, string);
// 		fclose(dot_file);
// 	}
// 	char * str = "asssssc d";
// 	dfa->minimize();
// 	if (dfa!=NULL){
// 		char out_file[100];
// 		sprintf(out_file,"%s_dfa", input_file);
// 		FILE *dot_file=fopen(out_file ,"w");
// 		fprintf(stderr,"\nExporting to DOT file %s ...\n", out_file);
// 		char string[100];
// 		sprintf(string,"source: %s", input_file);
// 		dfa->to_dot(dot_file, string);
// 		fclose(dot_file);
// 		printf("r1 = %d\n", dfa->match(str));
// 	}
// 	RCDFA * rcdfa = new RCDFA(dfa);
// 	if (rcdfa!=NULL){
// 		char out_file[100];
// 		sprintf(out_file,"%s_rcdfa", input_file);
// 		FILE *dot_file=fopen(out_file ,"w");
// 		fprintf(stderr,"\nExporting to DOT file %s ...\n", out_file);
// 		char string[100];
// 		sprintf(string,"source: %s", input_file);
// 		rcdfa->to_dot(dot_file, string);
// 		fclose(dot_file);
// 		printf("r2 = %d\n", rcdfa->match(str));
// 	}
// 	dfa_array dfas = (DFA **) allocate_array(10, sizeof(DFA*));
// 	nfa_array nfas = (NFA **) allocate_array(10, sizeof(NFA*));
// 	dfas[0] = dfa;
// 	nfas[0] = nfa;
// 	MDFA * mdfa = new MDFA(dfas, nfas, 1);
// 	printf("%p\n", mdfa);
// 	delete mdfa;
// 	delete rcdfa;
// 	delete dfa;
// 	delete nfa;
// 	return 0;
// }