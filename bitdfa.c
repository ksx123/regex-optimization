#include "bitdfa.h"
#include "c_type.h"

BitCmpDfaBitmapRange *get_bitmap_from_sample_ranges(list<BitCmpDfSampleRange*> * sample_ranges){
	BitCmpDfaBitmapRange *bitmap_range = new BitCmpDfaBitmapRange();
	if(sample_ranges->empty()){
		return NULL;
	}

	bitmap_range->target = sample_ranges->front()->target;
	BitmapInit(bitmap_range->bitmap, CHAR_BIRMAP_SIZE);
	for (list<BitCmpDfSampleRange*>::iterator it=sample_ranges->begin() ; it != sample_ranges->end(); ++it){
		for (unsigned int i = (*it)->start; i <= (*it)->end; ++i) {
			BitmapSet(bitmap_range->bitmap, CHAR_BIRMAP_SIZE, i);
		}
	}
	return bitmap_range;
}

BitCmpDfaBitmapRange *create_bit_range_list(list<BitCmpDfaBitmapRange*> * bitmap_list){
	BitCmpDfaBitmapRange* bit_list = new BitCmpDfaBitmapRange[bitmap_list->size()];
	int index = 0;
	for (list<BitCmpDfaBitmapRange*>::iterator it=bitmap_list->begin() ; it != bitmap_list->end(); ++it){
		bit_list[index] = *(*it);
		index++;
	}
	return bit_list;
}

BitCmpDfa::BitCmpDfa(DFA* dfa){
	int _size = this->size = dfa->size();

	linked_set** o_accepted_rules = dfa-> get_accepted_rules();
	state_t ** state_table = dfa->get_state_table();

	this->accepted_rules = (linked_set **)allocate_array(_size, sizeof(linked_set *));
	this->edges = (BitCmpDfaEdage *)allocate_array(_size, sizeof(BitCmpDfaEdage));

	for(state_t s=0;s<_size;++s){
		this->accepted_rules[s] = new linked_set();
		this->accepted_rules[s]->add(o_accepted_rules[s]);
	}

	for (state_t s=0;s<_size;s++){
		this->forOneState(s, state_table);
	}
}


void BitCmpDfa::forOneState(state_t s, state_t ** state_table){
	int *mark = allocate_int_array(CSIZE);

	for(int i=0;i<CSIZE;i++) {
		mark[i]=0;
	}

	list<BitCmpDfaBitmapRange*> * bitmap_list = new list<BitCmpDfaBitmapRange*>();
	list<BitCmpDfSampleRange*> * same_target = new list<BitCmpDfSampleRange*>();
	
	BitCmpDfSampleRange* r_edge = NULL;
	state_t target=NO_STATE;
	for (int c=0;c<CSIZE;c++){
		if (!mark[c]){
			mark[c]=1;
			if (state_table[s][c]!=0){
				target=state_table[s][c];
				r_edge = new BitCmpDfSampleRange();
				r_edge->start = c;
				r_edge->end = c;
				r_edge->target = target;

				bool range=true;
				same_target->clear();
				for(int d=c+1;d<CSIZE;d++){
					if(state_table[s][d]==target){
						mark[d] = 1;
						if(!range){
							range = true;
							r_edge = new BitCmpDfSampleRange();
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
						same_target->push_back(r_edge);
					}
				}

				bitmap_list->push_back(get_bitmap_from_sample_ranges(same_target));
				//free megered sample range
				for (list<BitCmpDfSampleRange*>::iterator it=same_target->begin() ; it != same_target->end(); ++it){
					delete *it;
				}
			}
		}
	}
	delete same_target;

	this->edges[s].lenght = bitmap_list->size();
	this->edges[s].data = create_bit_range_list(bitmap_list);

	for (list<BitCmpDfaBitmapRange*>::iterator it=bitmap_list->begin() ; it != bitmap_list->end(); ++it){
		delete *it;
	}
	delete bitmap_list;

	free(mark);
}

BitCmpDfa::~BitCmpDfa(){

	for(state_t s=0;s<this->size;++s){
		delete this->accepted_rules[s];
	}
	free(this->accepted_rules);

	for(state_t s=0;s<this->size;++s){
		BitCmpDfaBitmapRange* data = this->edges[s].data;
		delete[] data;
	}
	free(this->edges);
}

unsigned int BitCmpDfa::getMemSize(){
	unsigned int sum = 0;
	sum += sizeof(BitCmpDfaEdage) * this->size;

	for(state_t s=0;s<this->size;++s){
		sum += sizeof(BitCmpDfaBitmapRange) * this->edges[s].lenght;
	}

	return sum;
}

state_t BitCmpDfa::getNext(state_t current, unsigned char input) {
	BitCmpDfaBitmapRange* data = this->edges[current].data;
	for(int i = 0; i < this->edges[current].lenght; ++i ) {
		if(BitmapCheck(data[i].bitmap, CHAR_BIRMAP_SIZE, input)){
			return data[i].target;
		}
	}
	return 0;
}
