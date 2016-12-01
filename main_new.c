
#include "stdinc.h"
// #include "nfa.h"
// #include "dfa.h"
// #include "hybrid_fa.h"
#include "parser.h"
// #include "trace.h"
#include "rcdfa.h"
#include "mdfa.h"

int VERBOSE;
int DEBUG = 1;

int main(int argc, char const *argv[])
{

	printf("%s\n", "new main");

	NFA *nfa=NULL; 
	DFA *dfa=NULL;

	char * input_file = "_test/txt_new";
	FILE *regex_file=fopen(input_file, "r");
	fprintf(stderr,"\nParsing the regular expression file %s ...\n", input_file);
	regex_parser *p = new regex_parser(false, false);
	nfa = p->parse(regex_file);
	if(nfa!=NULL){
			char string[100];
			char out_file[100];
			sprintf(out_file,"%s_nfa1", input_file);
			FILE *nfa_dot_file=fopen(out_file,"w");
			fprintf(stderr,"\nExporting to NFA DOT file %s ...\n", out_file);
			sprintf(string,"source: %s",input_file);
			nfa->to_dot(nfa_dot_file, string);
			fclose(nfa_dot_file);
	}
	nfa->remove_epsilon();
	if(nfa!=NULL){
			char string[100];
			char out_file[100];
			sprintf(out_file,"%s_nfa2", input_file);
			FILE *nfa_dot_file=fopen(out_file,"w");
			fprintf(stderr,"\nExporting to NFA DOT file %s ...\n", out_file);
			sprintf(string,"source: %s",input_file);
			nfa->to_dot(nfa_dot_file, string);
			fclose(nfa_dot_file);
	}
	nfa->reduce();
	if(nfa!=NULL){
			char string[100];
			char out_file[100];
			sprintf(out_file,"%s_nfa2", input_file);
			FILE *nfa_dot_file=fopen(out_file,"w");
			fprintf(stderr,"\nExporting to NFA DOT file %s ...\n", out_file);
			sprintf(string,"source: %s",input_file);
			nfa->to_dot(nfa_dot_file, string);
			fclose(nfa_dot_file);
	}
	char * str = "asssssc d";
	dfa=nfa->nfa2dfa();
	if (dfa!=NULL){
		char out_file[100];
		sprintf(out_file,"%s_dfa", input_file);
		FILE *dot_file=fopen(out_file ,"w");
		fprintf(stderr,"\nExporting to DOT file %s ...\n", out_file);
		char string[100];
		sprintf(string,"source: %s", input_file);
		dfa->to_dot(dot_file, string);
		fclose(dot_file);
		printf("r1 = %d\n", dfa->match(str));
	}
	RCDFA * rcdfa = new RCDFA(dfa);
	if (rcdfa!=NULL){
		char out_file[100];
		sprintf(out_file,"%s_rcdfa", input_file);
		FILE *dot_file=fopen(out_file ,"w");
		fprintf(stderr,"\nExporting to DOT file %s ...\n", out_file);
		char string[100];
		sprintf(string,"source: %s", input_file);
		rcdfa->to_dot(dot_file, string);
		fclose(dot_file);
		printf("r2 = %d\n", rcdfa->match(str));
	}
	DFA ** dfas = (DFA **) allocate_array(10, sizeof(DFA*));
	dfas[0] = dfa;
	MDFA * mdfa = new MDFA(dfas, 1);
	printf("%p\n", mdfa);
	delete mdfa;
	delete rcdfa;
	delete dfa;
	delete nfa;
	return 0;
}