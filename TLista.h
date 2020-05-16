#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef LISTA
#define LISTA

typedef void (*TFreeL)(void *info);
typedef void (*afiElL)(void *info,FILE *out);

typedef struct celula
{
	void *info;
	struct celula *urm;
}TCelula,*TLista;

TLista InitLista();
int Inserare(TLista *l,void *ae);
void DistrL(TLista *al,TFreeL felib);
void afisareLista(TLista l,afiElL fafis,FILE *out);

#endif