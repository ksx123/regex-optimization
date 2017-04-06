#ifndef __C_TYPE_H               
#define __C_TYPE_H

#define POINTER_SIZE 8
#define POINTER_BIT_SIZE POINTER_SIZE * 8

#define BYTESIZE 8
unsigned char BITMAP_PRE[8] = {0x01<<0, 0x01<<1, 0x01<<2, 0x01<<3, 0x01<<4, 0x01<<5, 0x01<<6, 0x01<<7};
 

inline void SetCharArrayWithVoidPointer(char char_array[POINTER_SIZE], void* p){
	long *temp_p = (long *)&char_array[0];
	*temp_p = (long)p;
}

inline void* GetVoidPointerFromCharArray(char char_array[POINTER_SIZE]){
	long *temp_p = (long *)&char_array[0];
	return (void*) *temp_p;
}


inline void BitmapInit(unsigned char* buf, int n)
{
    memset(buf, 0, sizeof(char)*n);
}
 
inline void BitmapSet(unsigned char* buf, int n, unsigned char i){
    int add = i/BYTESIZE;
    int pos = i%BYTESIZE;
    if(add>=n) {
        return ;
    }
    buf += add;
    *buf |= BITMAP_PRE[pos]; //*buf |= (0x01<<(i % BYTESIZE));
}
 
inline int BitmapCheck(unsigned char* buf, int n, unsigned char i){
    int add = i/BYTESIZE;
    int pos = i%BYTESIZE;
 
    if(add>=n) {
        return 0;
    }
    buf += add;
    return (*buf&BITMAP_PRE[pos])==BITMAP_PRE[pos]; // return (*ptr&0x01<<(i%BYTESIZE))==0x01<<(i%BYTESIZE);
}




#endif /*__C_TYPE_H*/