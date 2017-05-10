#ifndef __C_TYPE_H               
#define __C_TYPE_H

#define POINTER_SIZE 8
#define POINTER_BIT_SIZE POINTER_SIZE * 8

#define BYTESIZE 8

void SetCharArrayWithVoidPointer(char char_array[POINTER_SIZE], void* p);
void* GetVoidPointerFromCharArray(char char_array[POINTER_SIZE]);


void BitmapInit(unsigned char* buf, unsigned int n);
void BitmapSet(unsigned char* buf, unsigned int n, unsigned int i);
int BitmapCheck(unsigned char* buf, unsigned int n, unsigned int i);

void BitmapCopy(unsigned char* dst, unsigned char* src, unsigned int n);

#endif /*__C_TYPE_H*/