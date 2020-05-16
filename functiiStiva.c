#include "TStiva.h"

int EMPTYS(void *a)
{
  if((((ASt)(a))->vf) == NULL)
	return 1;

  return 0;
}
void *InitS(size_t d)
{
	ASt st = (ASt)calloc(1,sizeof(TStiva));
	if(!st)
		return NULL;
	st->dime = d;
	st->vf = calloc(1,sizeof(TCelSt));
	st->vf = NULL;
	return (void*)st;
}

int PushS(void *st,void *ae)
{
	ACelSt aux = (ACelSt)calloc(1,sizeof(TCelSt));
	if(!aux)
		return 0;
	aux->info = calloc(1,((ASt)(st))->dime);
	memcpy(aux->info,ae,((ASt)st)->dime);

	aux->urm = ((ASt)(st))->vf;
	((ASt)st)->vf = aux;
	//free(ae);
	return 1;
}
//elimina elem din varful stivei
int PopS(void *s,void *ae)
{
	if(EMPTYS(s))
		return 0;
	memcpy(ae,((ASt)(s))->vf->info,((ASt)(s))->dime); 
	ACelSt aux = ((ASt)(s))->vf;
	((ASt)(s))->vf = aux->urm;
	//f(aux->info);//TNodArb
	free(aux);
	return 1;
}
int Top(void *st,void *ae)
{
	if(EMPTYS(st))
		return 0;
	memcpy(ae,((ASt)st)->vf->info,((ASt)st)->dime);
	return 1;
}
// functie  care elibereaza stiva 
void DistrS(void *st,TFreeS f)
{
	ACelSt p = ((ASt)(st))->vf,aux;
	void *elem = calloc(1,((ASt)(st))->dime);
	if(!elem)
		return;
	while(!EMPTYS(st))
		PopS(st,elem);
}

void AfisSt(void *a,afiElS afis,TFreeS f)
{
	ACelSt p = ((ASt)a)->vf;
	void *elem = calloc(1,((ASt)a)->dime);
	while (!EMPTYS(a))
	{
		PopS(a,elem);
		afis(elem);
	}
	free(elem);
}

void RastoarnaS(void *dest,void *src)
{
	if( ((ASt)src)->dime != ((ASt)dest)->dime || src == NULL) 
		return;
	ACelSt ld = ((ASt)dest)->vf,aux;
	ACelSt ls = ((ASt)src)->vf;
	for(; ls != NULL; ls = ls->urm)
	{
		aux = ls;
		aux->urm = ld;
		ld = aux;
	}
	((ASt)src)->vf = NULL;
	((ASt)dest)->vf = ld;
}