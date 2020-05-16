#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef STIVA
#define STIVA


typedef struct celSt
{
	struct celSt *urm;
	void *info;
}TCelSt,*ACelSt;

typedef struct stiva
{
	size_t dime; //dim unui element = dim unei pg web
	ACelSt vf; //adresa celulei din varf
}TStiva,*ASt;

//functii de push,pop si trebuie sa parcurg stiva fara a itera ??
typedef void (*TFreeS)(void *info);
typedef void (*afiElS)(void *info);
int EMPTYS(void *a);
void *InitS(size_t d);
//functie pt inserare in stiva: 1 a reusit,0 = n a reusit
int PushS(void *a,void *ae); // ae = adr elem de inserat 
int Top(void *a,void *ae);//copiaza elem din varful stivei la adr ae
int PopS(void *a,void *ae); //sterge elem din varful stivei si pastreaza info la adr ae
void DistrS(void *st,TFreeS f);
void AfisSt(void *a,afiElS afis,TFreeS f);//echiv cu afisarea unei liste
void RastoarnaS(void *dest,void *src);

#endif