#include "TCoada.h"
void *InitQ(size_t d) // d e dim unui elem introdus in caoda
{
	AQ q = (AQ)calloc(1,sizeof(TCoada));
	if(!q)
		return NULL;
	q->dime = d;
	q->ic = q->sc = NULL;
	return (void*)q;
}

int EMPTYQ(void *a)
{
	if(((AQ)(a))->ic == NULL && ((AQ)(a))->sc == NULL ) // coada nula doar atunci cand inceputul si sfarsitul puncteaza catre null 
		return 1;
	return 0;
}
int IntrQ(void *a,void *ae)
{
	ACelQ aux = calloc(1,sizeof(TCelQ));
	if(!aux)
		return 0;
	aux->info = calloc(1,((AQ)a)->dime);
	memcpy(aux->info,ae,((AQ)a)->dime);// ae fiind alocat inainte de a apela functia
	aux->urm = NULL;
	if(!EMPTYQ(a)) // atunci adauga celula la sfarsitul listei
	{	
		((AQ)a)->sc->urm = aux; 
		((AQ)a)->sc = aux;
	}
	else
	{	
		((AQ)a)->ic = aux;
		((AQ)a)->sc = aux;
	}
	return 1;
}

int ExtrQ(void *a,void *ae)//TFreeQ freeEl)
{
	if(!EMPTYQ(a))
	{
		ACelQ aux = calloc(1,sizeof(TCelQ));
		if(!aux)
		return 0;
		aux->info = calloc(1,((AQ)(a))->dime);
		// daca exista doar o celula
	 	if(((AQ)a)->sc == ((AQ)a)->ic){
			aux = ((AQ)a)->ic;
			((AQ)a)->ic = ((AQ)a)->sc = NULL;
	 	}
		else // preia ultima celula din lista cu 2 sau mai multe celule
		{
			aux = ((AQ)a)->ic;
			((AQ)a)->ic = aux->urm;
		}
		if(!aux->info)
			return 0;
		if(!ae)
			return 0;

		memcpy(ae,aux->info,((AQ)(a))->dime);
		//freeEl(aux->info);
		//free(aux);
		return 1;
	}
	else
		return 0;

}
/*
// @ freeEl este un pointer la o functie ce elibereaza void *elem
void DistrQ(void **a,TFreeQ freeEl)
{
	if(*a == NULL)
		return;

	ACelQ q = ((AQ)(*a))->ic,aux;
	for(;q != NULL; )
	{
		aux = q;
		q = q->urm;
		freeEl(aux->info);
		free(aux);
	}	

	free(*a);
	*a = NULL;
}

void DistrQN(void **a,TFreeQ freeEl,int nr)
{	
	int elim = 0;
	if(*a == NULL)
		return;
	// distruge nr celule din coada in cazul in care nu ni se cere distrugerea integrala
	while(!EMPTYQ(*a) && elim < nr)
	{ 
	 void *elem = calloc(1,100);
	 if(!elem)
	 	return;
	 ExtrQ(a,elem,freeEl);
	 elim++;
	}
}

// @cmp pointer la o functie care compara doua elem de tip void *
void IntrQSorted(void **q,void *ae,TFCmp cmp,TFreeQ fEl)
{
	if(EMPTYQ(*q))
	{
		IntrQ(q,ae);
		return;
	}
	void *aux = InitQ(((AQ)(*q))->dime);
	if(!aux)
		return;

	void *elem = calloc(1,((AQ)(*q))->dime);
	if(!elem)
	{
		free(aux);
		return;
	}
	//scoatere elem din coada q si adauga in coada auxiliara
	while(!EMPTYQ(*q))
	{
		if(cmp(((AQ)(*q))->ic->info,ae) > 0 )
		{
			ExtrQ(q,elem,fEl);
			IntrQ(&aux,elem);
		}
		else
			break;
	}
	IntrQ(&aux,ae);
	while(!EMPTYQ(*q)){
		ExtrQ(q,elem,fEl);
		IntrQ(&aux,elem);
	}
	free(elem);
	((AQ)(*q))->ic = ((AQ)(aux))->ic;
	((AQ)(*q))->sc = ((AQ)(aux))->sc;
	free(aux);
}

// afis este * la o functie de afisare a elementelor de tip void * din coada
void AfisQ(void *q,TFAfisQ afis,TFreeQ freeEl,FILE *out)
{	
	void *x;
	void *aux = InitQ(((AQ)q)->dime);
	//copiem elem intr o coada temporara
	while(!EMPTYQ(q))
	{
		x = calloc(1,((AQ)q)->dime);
		ExtrQ(&q,x,freeEl);
		IntrQ(&aux,x);
	}
	while(!EMPTYQ(aux))
	{
		x = calloc(1,((AQ)q)->dime);
		if(!x)
			return;
		ExtrQ(&aux,x,freeEl);
		IntrQ(&q,x);
		afis(x,out);
	}
	DistrQ(&aux,freeEl);
	
}
*/