#include <stdio.h>
#include "parser.h"
#include "nmdfa.h"


#ifndef CUR_VER
#define CUR_VER		"SHMTU 1.5.0"
#endif

int VERBOSE;
int DEBUG;
int GROUP_LIMIT = 100;

/*
 * Returns the current version string
 */
void version(){
    printf("version:: %s\n", CUR_VER);
}

/* usage */
static void usage() 
{
	fprintf(stderr,"\n");
    fprintf(stderr, "Usage: regex [options]\n"); 
    fprintf(stderr, "             [--parse|-p <regex_file> [--m|--i] | --import|-i <in_file> ]\n");
    fprintf(stderr, "             [--export|-e  <out_file>][--graph|-g <dot_file>]\n");
    fprintf(stderr, "             [--trace|-t <trace_file>]\n");
    fprintf(stderr, "             [--hfa]\n\n");
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, "    --help,-h       print this message\n");
    fprintf(stderr, "    --version,-r    print version number\n");				    
    fprintf(stderr, "    --verbose,-v    basic verbosity level \n");
    fprintf(stderr, "    --debug,  -d    enhanced verbosity level \n");
	fprintf(stderr, "\nOther:\n");
	fprintf(stderr, "    --parse,-p <regex_file>  process regex file\n");
	fprintf(stderr, "    --m,--i  m modifier, ignore case\n");
    fprintf(stderr, "    --trace,-t <trace_file>  trace file to be processed\n");
    fprintf(stderr, "\n");
    exit(0);
}

/* configuration */
static struct conf {
	char *regex_file;
	char *trace_file;
	bool i_mod;
	bool m_mod;
	bool verbose;
	bool debug;
    int group_limit;
    int cpu_num;
    bool use_ecdfa;
} config;


/* initialize the configuration */
void init_conf(){
	config.regex_file=NULL;
	config.trace_file=NULL;
	config.i_mod=false;
	config.m_mod=false;
	config.debug=false;
	config.group_limit=0;
    config.cpu_num=4;
    config.use_ecdfa=false;
}

/* print the configuration */
void print_conf(){
	fprintf(stderr,"\nCONFIGURATION: \n");
	if (config.regex_file) fprintf(stderr, "- RegEx file: %s\n",config.regex_file);
	if (config.trace_file) fprintf(stderr,"- Trace file: %s\n",config.trace_file);
	if (config.i_mod) fprintf(stderr,"- ignore case selected\n");
	if (config.m_mod) fprintf(stderr,"- m modifier selected\n");
	if (config.verbose && !config.debug) fprintf(stderr,"- verbose mode\n");
	if (config.debug) fprintf(stderr,"- debug mode\n");
    if (config.group_limit) fprintf(stderr, "- Group limit: %d\n",config.group_limit);
    if (config.cpu_num) fprintf(stderr,"- Cpu number(default:4): %d\n",config.cpu_num);
    if (config.use_ecdfa) fprintf(stderr,"- use ecdfa\n");
}

/* parse the main call parameters */
static int parse_arguments(int argc, char **argv)
{
	int i=1;
    if (argc < 2) {
        usage();
		return 0;
    }
    while(i<argc){
    	if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
    		usage();
    		return 0;
    	}else if(strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--version") == 0){
    		version();
    		return 0;
    	}else if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0){
    		config.verbose=1;
    	}else if(strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0){
    		config.debug=1;
    	}else if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--parse") == 0){
    		i++;
    		if(i==argc){
    			fprintf(stderr,"Regular expression file name missing.\n");
    			return 0;
    		}
    		config.regex_file=argv[i];
    	}else if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--trace") == 0){
    		i++;
    		if(i==argc){
    			fprintf(stderr,"Trace file name missing.\n");
    			return 0;
    		}
    		config.trace_file=argv[i];
        }else if(strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--cpu") == 0){
            i++;
            if(i==argc){
                fprintf(stderr,"Cpu number value missing.\n");
                return 0;
            }
            config.cpu_num=atoi(argv[i]);		
        }else if(strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--limit") == 0){
            i++;
            if(i==argc){
                fprintf(stderr,"Group limit value missing.\n");
                return 0;
            }
            config.group_limit=atoi(argv[i]);  
    	}else if(strcmp(argv[i], "--m") == 0){
			config.m_mod=true;
		}else if(strcmp(argv[i], "--i") == 0){
			config.i_mod=true;
        }else if(strcmp(argv[i], "--e") == 0){
            config.use_ecdfa=true;  	    		
    	}else{
    		fprintf(stderr,"Ignoring invalid option %s\n",argv[i]);
    	}
    	i++;
    }
	return 1;
}

int main(int argc, char **argv){
	//read configuration
	init_conf();
	while(!parse_arguments(argc,argv)) usage();
	print_conf();
	VERBOSE=config.verbose;
	DEBUG=config.debug;
	if (DEBUG) {
		VERBOSE=1;
	} 

    if(config.group_limit!=0){
        GROUP_LIMIT = config.group_limit;
    }

	if(config.regex_file!=NULL){
		regex_parser *parse=new regex_parser(config.i_mod,config.m_mod);
		FILE *re_file=fopen(config.regex_file,"r");
		NMDFA* nmdfa = new NMDFA(re_file, parse, config.cpu_num, config.use_ecdfa? 2: 1);
		fclose(re_file);
		printf("nmdfa: gSize=%d, size=%d, memSize=%d\n", nmdfa->getGroupSize(), nmdfa->getSize(), nmdfa->getMemSize());
	}



	return 0;
}