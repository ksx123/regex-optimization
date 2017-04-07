
#ifndef __RCDFA_H               
#define __RCDFA_H

#include "dfa.h"

using namespace std;

class RangeEdge{
public:
	unsigned char start;
	unsigned char end;
	state_t target;
};
typedef list<RangeEdge*> list_re;

class RCDFA : public DFA{
protected:
	list_re** range_edges_table;

public:
	RCDFA(unsigned=50);
	RCDFA(DFA* dfa);

	~RCDFA();

	unsigned int get_m_size();
	
	void to_dot(FILE *file, const char *title);
	
	int match(FILE *file);
	int match(char * str);

	list_re** get_edges_table();
};

inline list_re** RCDFA::get_edges_table(){return range_edges_table;}

#endif /*__DFA_H*/
