#include "c_type.h"
#include <memory.h>

unsigned char BITMAP_PRE[8] = {0x01<<0, 0x01<<1, 0x01<<2, 0x01<<3, 0x01<<4, 0x01<<5, 0x01<<6, 0x01<<7};
 

void SetCharArrayWithVoidPointer(char char_array[POINTER_SIZE], void* p){
	long *temp_p = (long *)&char_array[0];
	*temp_p = (long)p;
}

void* GetVoidPointerFromCharArray(char char_array[POINTER_SIZE]){
	long *temp_p = (long *)&char_array[0];
	return (void*) *temp_p;
}


void BitmapInit(unsigned char* buf, unsigned int n){
    memset(buf, 0, sizeof(char)*n);
}
 
void BitmapSet(unsigned char* buf, unsigned int n, unsigned int i){
    unsigned int add = i/BYTESIZE;
    unsigned int pos = i%BYTESIZE;
    if(add>=n) {
        return ;
    }
    buf += add;
    *buf |= BITMAP_PRE[pos]; //*buf |= (0x01<<(i % BYTESIZE));
}

int BitmapCheck(unsigned char* buf, unsigned int n, unsigned int i){
    unsigned int add = i/BYTESIZE;
    unsigned int pos = i%BYTESIZE;
 
    if(add>=n) {
        return 0;
    }
    buf += add;
    return (*buf&BITMAP_PRE[pos])==BITMAP_PRE[pos]; // return (*ptr&0x01<<(i%BYTESIZE))==0x01<<(i%BYTESIZE);
}