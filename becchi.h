#ifndef __BECCHI_H               
#define __BECCHI_H

#include <stdio.h>
#include "parser.h"

// unsigned int GROUP_LIMIT = 100;
extern int GROUP_LIMIT;

class BecchiDfaGroup{
public:
	int_set* data;
	bool isFull;
};

class Becchi {
protected:
	unsigned int group_size;
	void* data;
	unsigned char type;
public:
	Becchi(FILE *file, regex_parser *parse, double ne, unsigned char type);
	~Becchi();


	unsigned int getGroupSize();
	unsigned getSize();
	unsigned int getMemSize(); //B

private:
	void buildDfaList(list<int_set*>* group_list, FILE *regex_file, regex_parser *parse);
	void buildEcDfaList(list<int_set*>* group_list, FILE *regex_file, regex_parser *parse);
};

inline unsigned int Becchi::getGroupSize() {return this->group_size;}

#endif /*__BECCHI_H*/
