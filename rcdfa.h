
#ifndef __RCDFA_H               
#define __RCDFA_H

#include "dfa.h"

#pragma pack (1)

using namespace std;

class RangeEdge{
public:
	unsigned char start;
	unsigned char end;
	state_t target;
};

class RangeEdgeList{
public:
	RangeEdge* ranges;
	unsigned int lenght;
};

typedef list<RangeEdge*> list_re;

class RCDFA : public DFA{
protected:
	list_re** range_edges_table;
	RangeEdgeList* range_edges_lists;

public:
	RCDFA(unsigned=50);
	RCDFA(DFA* dfa);

	~RCDFA();

	unsigned int get_m_size();
	unsigned int getMemSize();
	
	void to_dot(FILE *file, const char *title);
	
	int match(FILE *file);
	int match(char * str);

	list_re** get_edges_table();
};

inline list_re** RCDFA::get_edges_table(){return range_edges_table;}

#endif /*__DFA_H*/
