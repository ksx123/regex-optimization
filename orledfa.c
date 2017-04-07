#include "orledfa.h"
#include "c_type.h"

void set_rle_range(list<ORleDfaSampleRange*> * sample_list, ORleDfaEdage* rle){

	list<ORleDfaRleRangeItem*>* rle_list = new list<ORleDfaRleRangeItem*>();
	unsigned char lastEnd = 0;
	bool isFirst = true;
	for (list<ORleDfaSampleRange*>::iterator it=sample_list->begin() ; it != sample_list->end(); ++it){
		if(isFirst && (*it)->start > 0){
			// printf("push_back: %d,%d\n", (*it)->start, isFirst);
			ORleDfaRleRangeItem* tmpRangeItem = new ORleDfaRleRangeItem();
			tmpRangeItem->target = 0;
			tmpRangeItem->lenght = (*it)->start;
			rle_list->push_back(tmpRangeItem);
		}else if((*it)->start - lastEnd > 1){
			ORleDfaRleRangeItem* tmpRangeItem = new ORleDfaRleRangeItem();
			tmpRangeItem->target = 0;
			tmpRangeItem->lenght = (*it)->start - lastEnd - 2;
			// printf("push_back: %d-%d\n", (*it)->start, lastEnd);
			rle_list->push_back(tmpRangeItem);
		}
		isFirst = false;

		// printf("s:%d, e:%d, t:%d\n", (*it)->start, (*it)->end, (*it)->target);

		unsigned char lenght = (*it)->end - (*it)->start; // need +1, 0 is 1, 255 is 256
		ORleDfaRleRangeItem* tmpRangeItem = new ORleDfaRleRangeItem();
		tmpRangeItem->target = (*it)->target;
		tmpRangeItem->lenght = lenght;
		rle_list->push_back(tmpRangeItem);
		lastEnd = (*it)->end;
	}
	// printf("sample_list=%d, rle_list=%d\n", sample_list->size(),  rle_list->size());
	rle->lenght = rle_list->size();
	rle->ranges = new ORleDfaRleRangeItem[rle_list->size()];

	int index = 0;
	for (list<ORleDfaRleRangeItem*>::iterator iit=rle_list->begin() ; iit != rle_list->end(); ++iit){
		rle->ranges[index] = *(*iit);
		delete *iit;
		index++;
	}
	delete rle_list;
}

ORleDfa::ORleDfa(DFA* dfa){
	int _size = this->size = dfa->size();

	linked_set** o_accepted_rules = dfa-> get_accepted_rules();
	state_t ** state_table = dfa->get_state_table();

	this->accepted_rules = (linked_set **)allocate_array(_size, sizeof(linked_set *));
	this->edges = (ORleDfaEdage *)allocate_array(_size, sizeof(ORleDfaEdage));

	for(state_t s=0;s<_size;++s){
		this->accepted_rules[s] = new linked_set();
		this->accepted_rules[s]->add(o_accepted_rules[s]);
	}

	for (state_t s=0;s<_size;s++){
		this->forOneState(s, state_table);
	}
}

bool CompareRanges(ORleDfaSampleRange* _X,  ORleDfaSampleRange* _Y) {
	return _X->start < _Y->start;
}

void ORleDfa::forOneState(state_t s, state_t ** state_table){
	int *mark = allocate_int_array(CSIZE);

	for(int i=0;i<CSIZE;i++) {
		mark[i]=0;
	}

	// list<BitCmpDfaBitmapRange*> * bitmap_list = new list<BitCmpDfaBitmapRange*>();
	list<ORleDfaSampleRange*> * re_list = new list<ORleDfaSampleRange*>();
	
	ORleDfaSampleRange* r_edge = NULL;
	state_t target=NO_STATE;
	for (int c=0;c<CSIZE;c++){
		if (!mark[c]){
			mark[c]=1;
			if (state_table[s][c]!=0){
				target=state_table[s][c];
				r_edge = new ORleDfaSampleRange();
				r_edge->start = c;
				r_edge->end = c;
				r_edge->target = target;

				bool range=true;
				for(int d=c+1;d<CSIZE;d++){
					if(state_table[s][d]==target){
						mark[d] = 1;
						if(!range){
							range = true;
							r_edge = new ORleDfaSampleRange();
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

	for (list<ORleDfaSampleRange*>::iterator it=re_list->begin() ; it != re_list->end(); ++it){
		delete *it;
	}
	delete re_list;

	free(mark);
}

ORleDfa::~ORleDfa(){

	for(state_t s=0;s<this->size;++s){
		delete this->accepted_rules[s];
	}
	free(this->accepted_rules);

	for(state_t s=0;s<this->size;++s){
		ORleDfaRleRangeItem* ranges = this->edges[s].ranges;
		delete[] ranges;
	}
	free(this->edges);
}

unsigned int ORleDfa::getMemSize(){
	unsigned int sum = 0;
	sum += sizeof(ORleDfaEdage) * this->size;
	for(state_t s=0;s<this->size;++s){
		sum += sizeof(ORleDfaRleRangeItem) * this->edges[s].lenght;
	}

	return sum;
}

state_t ORleDfa::getNext(state_t current, unsigned char input) {
	int lastEnd = -1;
	for(int i = 0; i < this->edges[current].lenght; ++i ) {
		lastEnd += this->edges[current].ranges[i].lenght;
		if(input <= lastEnd) {
			return this->edges[current].ranges[i].target;
		}
	}
	return 0;
}