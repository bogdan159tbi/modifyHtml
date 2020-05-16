#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef COADA
#define COADA

typedef void (*TFreeQ)(void *info);
typedef int (*TFCmp)(void *x,void *y);
typedef void (*TFAfisQ)(void *info,FILE *out);
typedef struct celQ
{
	struct celQ *urm;
	void *info;
}TCelQ,*ACelQ;

typedef struct coada
{
	size_t dime;
	ACelQ ic,sc;
}TCoada,*AQ;

int EMPTYQ(void *a);

void *InitQ(size_t d);
int IntrQ(void *a,void *ae);
int ExtrQ(void *a,void *ae);//,TFreeQ freeEl);
void DistrQ(void **a,TFreeQ freeEl);
void DistrQN(void **a,TFreeQ freeEl,int nr);//pt history sa elim stringurile in functie de nr entries
void IntrQSorted(void **q,void *ae,TFCmp cmp,TFreeQ fEL);
void AfisQ(void *q,TFAfisQ afis,TFreeQ freeEl,FILE *out);

#endif