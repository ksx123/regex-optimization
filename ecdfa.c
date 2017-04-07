#include "ecdfa.h"
#include "c_type.h"

EgCmpDfaBitmapRange *get_bitmap_range_from_sample_ranges(list<EgCmpDfaSampleRange*> * sample_ranges){
	// printf("start get_bitmap_range_from_sample_ranges\n");
	EgCmpDfaBitmapRange *bitmap_range = new EgCmpDfaBitmapRange();
	if(sample_ranges->empty()){
		return NULL;
	}
	// printf("get_bitmap_range_from_sample_ranges 1\n");
	bitmap_range->target = sample_ranges->front()->target;
	// printf("get_bitmap_range_from_sample_ranges 2\n");
	BitmapInit(bitmap_range->bitmap, CHAR_BIRMAP_SIZE);
	// printf("get_bitmap_range_from_sample_ranges 3\n");
	for (list<EgCmpDfaSampleRange*>::iterator it=sample_ranges->begin() ; it != sample_ranges->end(); ++it){
		// printf("get_bitmap_range_from_sample_ranges 4\n");
		for (unsigned int i = (*it)->start; i <= (*it)->end; ++i) {
			// printf("get_bitmap_range_from_sample_ranges 5: %u %u %u\n",(*it)->start, (*it)->end, i);
			BitmapSet(bitmap_range->bitmap, CHAR_BIRMAP_SIZE, i);
		}
	}
	// printf("end get_bitmap_range_from_sample_ranges\n");
	return bitmap_range;
}

EgCmpDfaBitmapRange *create_bit_range_list(list<EgCmpDfaBitmapRange*> * bitmap_list){
	EgCmpDfaBitmapRange* bit_list = new EgCmpDfaBitmapRange[bitmap_list->size()];
	int index = 0;
	for (list<EgCmpDfaBitmapRange*>::iterator it=bitmap_list->begin() ; it != bitmap_list->end(); ++it){
		bit_list[index] = *(*it);
		index++;
	}
	return bit_list;
}

EgCmpDfaSampleRange *create_sam_range_list(list<EgCmpDfaSampleRange*> * sample_list){
	EgCmpDfaSampleRange* sam_list = new EgCmpDfaSampleRange[sample_list->size()];
	int index = 0;
	for (list<EgCmpDfaSampleRange*>::iterator it=sample_list->begin() ; it != sample_list->end(); ++it){
		sam_list[index] = *(*it);
		index++;
	}
	return sam_list;
}

EgCmpDfaRleRange *create_rle_range_list(list<EgCmpDfaSampleRange*> * sample_list){
	EgCmpDfaRleRange * rle = new EgCmpDfaRleRange();
	rle->ranges = new EgCmpDfaRleRangeItem[sample_list->size()];

	BitmapInit(rle->bitmap, CHAR_BIRMAP_SIZE);
	int index = 0;
	for (list<EgCmpDfaSampleRange*>::iterator it=sample_list->begin() ; it != sample_list->end(); ++it){
		for (unsigned int i = (*it)->start; i <= (*it)->end; ++i) {
			BitmapSet(rle->bitmap, CHAR_BIRMAP_SIZE, i);
		}
		rle->ranges[index].end = (*it)->end;
		rle->ranges[index].target = (*it)->target;
		index++;
	}
	return rle;
}

bool CompareRanges(EgCmpDfaSampleRange* _X,  EgCmpDfaSampleRange* _Y) {
	return _X->start < _Y->start;
}

EgCmpDfa::EgCmpDfa(DFA* dfa){
	int _size = this->size = dfa->size();

	linked_set** o_accepted_rules = dfa-> get_accepted_rules();
	state_t ** state_table = dfa->get_state_table();

	// this->accepted_rules = (linked_set **)allocate_array(_size,sizeof(linked_set *));
	this->accepted_rules = (linked_set **)allocate_array(_size, sizeof(linked_set *));
	this->edges = (EgCmpDfaEdage *)allocate_array(_size, sizeof(EgCmpDfaEdage));

	for(state_t s=0;s<_size;++s){
		this->accepted_rules[s] = new linked_set();
		this->accepted_rules[s]->add(o_accepted_rules[s]);
	}

	for (state_t s=0;s<_size;s++){
		this->forOneState(s, state_table);
	}
}


void EgCmpDfa::forOneState(state_t s, state_t ** state_table){
	int *mark = allocate_int_array(CSIZE);

	for(int i=0;i<CSIZE;i++) {
		mark[i]=0;
	}

	list<EgCmpDfaSampleRange*> * re_list = new list<EgCmpDfaSampleRange*>();
	list<EgCmpDfaBitmapRange*> * bitmap_list = new list<EgCmpDfaBitmapRange*>();
	list<EgCmpDfaSampleRange*> * same_target = new list<EgCmpDfaSampleRange*>();
	EgCmpDfaSampleRange* r_edge = NULL;
	state_t target=NO_STATE;
	for (int c=0;c<CSIZE;c++){
		if (!mark[c]){
			mark[c]=1;
			if (state_table[s][c]!=0){
				target=state_table[s][c];
				r_edge = new EgCmpDfaSampleRange();
				r_edge->start = c;
				r_edge->end = c;
				r_edge->target = target;

				bool range=true;
				same_target->clear();
				for(int d=c+1;d<CSIZE;d++){
					if(state_table[s][d]==target){
						// printf("\t%d -> %d = %d\n", s, d, target);
						mark[d] = 1;
						if(!range){
							range = true;
							r_edge = new EgCmpDfaSampleRange();
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
				if(same_target->size()>12){
					bitmap_list->push_back(get_bitmap_range_from_sample_ranges(same_target));
					//free megered sample range
					for (list<EgCmpDfaSampleRange*>::iterator it=same_target->begin() ; it != same_target->end(); ++it){
						delete *it;
					}
				}else{
					for (list<EgCmpDfaSampleRange*>::iterator it=same_target->begin() ; it != same_target->end(); ++it){
						re_list->push_back(*it);
					}
				}
			}
		}
	}
	delete same_target;

	if (bitmap_list->empty() && re_list->size()==1 && re_list->front()->start == re_list->front()->end) { // 只有一个
		this->edges[s].type = 1;
		EgCmpDfaRleRangeItem *data = new EgCmpDfaRleRangeItem();
		data->target = re_list->front()->target;
		data->end = re_list->front()->end;
		this->edges[s].data = data;
	}else{
		unsigned int bit_size = bitmap_list->size();
		unsigned int re_size = re_list->size();

		EgCmpDfaBitmapRange *bit_range_list = NULL;
		EgCmpDfaSampleRange *sam_range_list = NULL;
		EgCmpDfaRleRange * rle_range_list = NULL;
		if(!bitmap_list->empty()){
			bit_range_list = create_bit_range_list(bitmap_list);
		}

		if(!re_list->empty()){
			// TODO sort re_list
			re_list->sort(CompareRanges);
			if(re_list->size()>CHAR_BIRMAP_SIZE){
				rle_range_list = create_rle_range_list(re_list);
			}else{
				sam_range_list = create_sam_range_list(re_list);
			}
		}

		if(bit_range_list!=NULL && sam_range_list!=NULL){
			this->edges[s].type = 2;
			this->edges[s].lenght = bit_size;
			EgCmpDfaHasBitEdage *data = new EgCmpDfaHasBitEdage();
			data->bit_ranges = bit_range_list;
			data->lenght = re_size;
			data->other_ranges = sam_range_list;
			this->edges[s].data = data;
		}

		if(bit_range_list!=NULL && rle_range_list!=NULL){
			this->edges[s].type = 3;
			this->edges[s].lenght = bit_size;
			EgCmpDfaHasBitEdage * data = new EgCmpDfaHasBitEdage();
			data->bit_ranges = bit_range_list;
			data->lenght = re_size;
			data->other_ranges = rle_range_list;
			this->edges[s].data = data;
		}

		if(bit_range_list==NULL && sam_range_list!=NULL){
			this->edges[s].type = 4;
			this->edges[s].lenght = re_size;
			this->edges[s].data = sam_range_list;
		}

		if(bit_range_list==NULL && rle_range_list!=NULL){
			this->edges[s].type = 5;
			this->edges[s].lenght = re_size;
			this->edges[s].data = rle_range_list;
		}

		if(bit_range_list!=NULL && sam_range_list==NULL && rle_range_list==NULL){
			this->edges[s].type = 6;
			this->edges[s].lenght = bit_size;
			this->edges[s].data = bit_range_list;
		}
	}

	for (list<EgCmpDfaSampleRange*>::iterator it=re_list->begin() ; it != re_list->end(); ++it){
		delete *it;
	}
	delete re_list;

	for (list<EgCmpDfaBitmapRange*>::iterator it=bitmap_list->begin() ; it != bitmap_list->end(); ++it){
		delete *it;
	}
	delete bitmap_list;

	free(mark);
}

EgCmpDfa::~EgCmpDfa(){

	for(state_t s=0;s<this->size;++s){
		delete this->accepted_rules[s];
	}
	free(this->accepted_rules);

	for(state_t s=0;s<this->size;++s){
		EgCmpDfaRleRangeItem* data1 = NULL;
		EgCmpDfaHasBitEdage * data23 = NULL;
		EgCmpDfaSampleRange* data4 = NULL;
		EgCmpDfaRleRange* data5 = NULL;
		EgCmpDfaBitmapRange* data6 = NULL;
		switch(this->edges[s].type){
			case 1:
				data1 = (EgCmpDfaRleRangeItem*)this->edges[s].data;
				delete data1;
				break;
			case 2:
				data23 = (EgCmpDfaHasBitEdage*)this->edges[s].data;
				delete[] data23->bit_ranges;
				data4 = (EgCmpDfaSampleRange*)data23->other_ranges;
				delete[] data4;
				delete data23;
				break;
			case 3:
				data23 = (EgCmpDfaHasBitEdage*)this->edges[s].data;
				delete[] data23->bit_ranges;
				data5 = (EgCmpDfaRleRange*)data23->other_ranges;
				delete[] data5->ranges;
				delete data5;
				delete data23;
				break;
			case 4:
				data4 = (EgCmpDfaSampleRange*)this->edges[s].data;
				delete[] data4;
				break;
			case 5:
				data5 = (EgCmpDfaRleRange*)this->edges[s].data;
				delete[] data5->ranges;
				delete data5;
				break;
			case 6:
				data6 = (EgCmpDfaBitmapRange*)this->edges[s].data;
				delete[] data6;
				break;
		}
	}
	free(this->edges);
}

unsigned int EgCmpDfa::getMemSize(){
	unsigned int sum = 0;
	sum += sizeof(EgCmpDfaEdage) * this->size;
	// printf("%d %d\n", sizeof(EgCmpDfaEdage), sizeof(state_t));
	for(state_t s=0;s<this->size;++s){
		EgCmpDfaRleRangeItem* data1 = NULL;
		EgCmpDfaHasBitEdage * data23 = NULL;
		EgCmpDfaSampleRange* data4 = NULL;
		EgCmpDfaRleRange* data5 = NULL;
		EgCmpDfaBitmapRange* data6 = NULL;
		switch(this->edges[s].type){
			case 1:
				data1 = (EgCmpDfaRleRangeItem*)this->edges[s].data;
				sum += sizeof(EgCmpDfaRleRangeItem);
				break;
			case 2:
				data23 = (EgCmpDfaHasBitEdage*)this->edges[s].data;
				sum += sizeof(EgCmpDfaHasBitEdage);
				sum += sizeof(EgCmpDfaBitmapRange) * this->edges[s].lenght;
				sum += sizeof(EgCmpDfaSampleRange) * data23->lenght;
				break;
			case 3:
				data23 = (EgCmpDfaHasBitEdage*)this->edges[s].data;
				sum += sizeof(EgCmpDfaHasBitEdage);
				sum += sizeof(EgCmpDfaBitmapRange) * this->edges[s].lenght;
				sum += sizeof(EgCmpDfaRleRange);
				sum += sizeof(EgCmpDfaRleRangeItem) * data23->lenght;
				break;
			case 4:
				sum += sizeof(EgCmpDfaSampleRange) * this->edges[s].lenght;
				break;
			case 5:
				sum += sizeof(EgCmpDfaRleRange);
				sum += sizeof(EgCmpDfaRleRangeItem) * this->edges[s].lenght;
				break;
			case 6:
				sum += sizeof(EgCmpDfaBitmapRange) * this->edges[s].lenght;
				break;
		}
	}
	return sum;
}

state_t EgCmpDfa::getNext(state_t current, unsigned char input) {
	EgCmpDfaRleRangeItem* data1 = NULL;
	EgCmpDfaHasBitEdage * data23 = NULL;
	EgCmpDfaSampleRange* data4 = NULL;
	EgCmpDfaRleRange* data5 = NULL;
	EgCmpDfaBitmapRange* data6 = NULL;
	unsigned int i = 0;
	switch(this->edges[current].type) {
		case 1:
			data1 = (EgCmpDfaRleRangeItem*)this->edges[current].data;
			if(data1->end==input) {
				return data1->target;
			}
			break;
		case 2:
			data23 = (EgCmpDfaHasBitEdage*)this->edges[current].data;
			for(; i < this->edges[current].lenght; ++i ) {
				if(BitmapCheck(data23->bit_ranges[i].bitmap, CHAR_BIRMAP_SIZE, input)){
					return data23->bit_ranges[i].target;
				}
			}
			data4 = (EgCmpDfaSampleRange*)data23->other_ranges;
			for(i = 0; i < data23->lenght; ++i ) {
				if(input >= data4[i].start && input <= data4[i].end) {
					return data4[i].target;
				}
			}
			break;
		case 3:
			data23 = (EgCmpDfaHasBitEdage*)this->edges[current].data;
			for(; i < this->edges[current].lenght; ++i ) {
				if(BitmapCheck(data23->bit_ranges[i].bitmap, CHAR_BIRMAP_SIZE, input)){
					return data23->bit_ranges[i].target;
				}
			}
			data5 = (EgCmpDfaRleRange*)data23->other_ranges;
			if (BitmapCheck(data5->bitmap,CHAR_BIRMAP_SIZE, input)){
				for(; i < data23->lenght; ++i ) {
					if(input <= data5->ranges[i].end) {
						return data5->ranges[i].target;
					}
				}
			}
			break;
		case 4:
			data4 = (EgCmpDfaSampleRange*)this->edges[current].data;
			for(; i < this->edges[current].lenght; ++i ) {
				if(input >= data4[i].start && input <= data4[i].end) {
					return data4[i].target;
				}
			}
			break;
		case 5:
			data5 = (EgCmpDfaRleRange*)this->edges[current].data;
			if (BitmapCheck(data5->bitmap,CHAR_BIRMAP_SIZE, input)){
				for(; i < this->edges[current].lenght; ++i ) {
					if(input <= data5->ranges[i].end) {
						return data5->ranges[i].target;
					}
				}
			}
			break;
		case 6:
			data6 = (EgCmpDfaBitmapRange*)this->edges[current].data;
			for(; i < this->edges[current].lenght; ++i ) {
				if(BitmapCheck(data6[i].bitmap, CHAR_BIRMAP_SIZE, input)){
					return data6[i].target;
				}
			}
			break;
	}
	return 0;
}

linked_set* EgCmpDfa::match(FILE *file){
  rewind(file);
  linked_set *result = new linked_set();
  state_t current = 0;
  unsigned char c = fgetc(file); 
  while(c!=EOF){
  	current = getNext(current, c);

    if(!this->accepted_rules[current]->empty()){
      result->add(this->accepted_rules[current]);
      break;
    }
    c = fgetc(file);
  }
  return result;
}