#ifndef COMMONH
#define COMMONH

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;

typedef signed int   s32;
typedef signed short s16;
typedef signed char  s8;

typedef struct {
	float *data;
	int x, y, z;
	int rs;
	float min, max;
}tVolumen;

void RenderAll();

void PrintStatus(char *str, ...);

#include <windows.h>

#endif

