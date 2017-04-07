#include "rledfa.h"
#include "c_type.h"

void set_rle_range(list<RleDfaSampleRange*> * sample_list, RleDfaEdage* rle){
	rle->lenght = sample_list->size();
	rle->ranges = new RleDfaRleRangeItem[sample_list->size()];

	BitmapInit(rle->bitmap, CHAR_BIRMAP_SIZE);
	int index = 0;
	for (list<RleDfaSampleRange*>::iterator it=sample_list->begin() ; it != sample_list->end(); ++it){
		for (unsigned int i = (*it)->start; i <= (*it)->end; ++i) {
			BitmapSet(rle->bitmap, CHAR_BIRMAP_SIZE, i);
		}
		rle->ranges[index].end = (*it)->end;
		rle->ranges[index].target = (*it)->target;
		index++;
	}
}

RleDfa::RleDfa(DFA* dfa){
	int _size = this->size = dfa->size();

	linked_set** o_accepted_rules = dfa-> get_accepted_rules();
	state_t ** state_table = dfa->get_state_table();

	this->accepted_rules = (linked_set **)allocate_array(_size, sizeof(linked_set *));
	this->edges = (RleDfaEdage *)allocate_array(_size, sizeof(RleDfaEdage));

	for(state_t s=0;s<_size;++s){
		this->accepted_rules[s] = new linked_set();
		this->accepted_rules[s]->add(o_accepted_rules[s]);
	}

	for (state_t s=0;s<_size;s++){
		this->forOneState(s, state_table);
	}
}

bool CompareRanges(RleDfaSampleRange* _X,  RleDfaSampleRange* _Y) {
	return _X->start < _Y->start;
}

void RleDfa::forOneState(state_t s, state_t ** state_table){
	int *mark = allocate_int_array(CSIZE);

	for(int i=0;i<CSIZE;i++) {
		mark[i]=0;
	}

	// list<BitCmpDfaBitmapRange*> * bitmap_list = new list<BitCmpDfaBitmapRange*>();
	list<RleDfaSampleRange*> * re_list = new list<RleDfaSampleRange*>();
	
	RleDfaSampleRange* r_edge = NULL;
	state_t target=NO_STATE;
	for (int c=0;c<CSIZE;c++){
		if (!mark[c]){
			mark[c]=1;
			if (state_table[s][c]!=0){
				target=state_table[s][c];
				r_edge = new RleDfaSampleRange();
				r_edge->start = c;
				r_edge->end = c;
				r_edge->target = target;

				bool range=true;
				for(int d=c+1;d<CSIZE;d++){
					if(state_table[s][d]==target){
						mark[d] = 1;
						if(!range){
							range = true;
							r_edge = new RleDfaSampleRange();
							r_edge->start = d;
							r_edge->end = d;
							r_edge->target = target;
						}
					}

					if(range && (state_table[s][d]!=target || d==CSIZE-1)){
						range = false;
						if(state_table[s][d]!=target){
							r_edge->end = d-1;
						}else{
							r_edge->end = d;
						}
						re_list->push_back(r_edge);
					}
				}
			}
		}
	}
	// delete same_target;

	re_list->sort(CompareRanges);

	set_rle_range(re_list, &this->edges[s]);

	for (list<RleDfaSampleRange*>::iterator it=re_list->begin() ; it != re_list->end(); ++it){
		delete *it;
	}
	delete re_list;

	free(mark);
}

RleDfa::~RleDfa(){

	for(state_t s=0;s<this->size;++s){
		delete this->accepted_rules[s];
	}
	free(this->accepted_rules);

	for(state_t s=0;s<this->size;++s){
		RleDfaRleRangeItem* ranges = this->edges[s].ranges;
		delete[] ranges;
	}
	free(this->edges);
}

unsigned int RleDfa::getMemSize(){
	unsigned int sum = 0;
	sum += sizeof(RleDfaEdage) * this->size;

	for(state_t s=0;s<this->size;++s){
		sum += sizeof(RleDfaRleRangeItem) * this->edges[s].lenght;
		// printf("%d %d\n", sizeof(RleDfaRleRangeItem), this->edges[s].lenght);
	}

	return sum;
}

state_t RleDfa::getNext(state_t current, unsigned char input) {
	if (BitmapCheck(this->edges[current].bitmap, CHAR_BIRMAP_SIZE, input)){
		for(int i = 0; i < this->edges[current].lenght; ++i ) {
			if(input <= this->edges[current].ranges[i].end) {
				return this->edges[current].ranges[i].target;
			}
		}
	}
	return 0;
}