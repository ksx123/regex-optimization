
#ifndef __RCDFA_H               
#define __RCDFA_H

#include "dfa.h"

using namespace std;

class RangeEdge{
public:
	char start;
	char end;
	state_t target;
};
typedef list<RangeEdge*> list_re;

class RCDFA : public DFA{
protected:
	list_re** range_edges_table;

public:
	RCDFA(unsigned=50);
	RCDFA(DFA* dfa);

	void to_dot(FILE *file, const char *title);
};

#endif /*__DFA_H*/
