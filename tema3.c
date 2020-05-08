#include "lib.h"

int main(int argc, char **argv)
{
	FILE *in = fopen("ex.html","r");
	if(!in)
		return 1;
	char c = ' ';
	TArb a = calloc(1,sizeof(TNodArb)),b,child,new;
	if(!a)
		return 1;
	b = a;
	TParseState currentState = 1,nextState;
	char *tmp = calloc(30,sizeof(char)),*value = calloc(30,sizeof(char));
	void *st = NULL;
	int nr = 0;
	
	while((c = fgetc(in)) != EOF)
		{
		nextState = Interpret(currentState,c);
		//printf("%c %d %d\n",c,currentState,nextState);
		if(c == '\n') // do nothing
			continue;
		else if(currentState == 1 && nextState == 1){
			TArb scos = calloc(1,sizeof(TNodArb));
			int rez = PopS(st,scos);
			if(rez)
			{
			if(!scos->info)	
			scos->info = calloc(1,sizeof(TNodInfo));
			if(!scos->info->contents)
				scos->info->contents = calloc(30,sizeof(char));
			scos->info->contents[strlen(scos->info->contents)] = c;
			PushS(st,(void*)scos);
			}
		}
		else if(currentState == 2 && nextState == 3){
			new = calloc(1,sizeof(TNodArb));
			if(!new->info)
				new->info = calloc(1,sizeof(TNodInfo));
			if(!new->info->type){
			new->info->type = calloc(30,sizeof(char));
			new->info->type[strlen(new->info->type)] = c;
			}
			if(!st)
				st = InitS(sizeof(TNodArb));
			nr++;// numara tag uri pagina
			PushS(st,(void*)new);
		}
		else if(currentState == 3 && nextState == 3)
		{	
			TArb scos = calloc(1,sizeof(TNodArb));
			int rez = PopS(st,scos);
			if(rez)
			{
			if(!scos->info)	
			scos->info = calloc(1,sizeof(TNodInfo));
			if(!scos->info->type)
				scos->info->type = calloc(30,sizeof(char));
			scos->info->type[strlen(scos->info->type)] = c;
			PushS(st,(void*)scos);
			}
		}
		else if(currentState == 4 && nextState == 1){
			//adauga tag node la copii
			if(!EMPTYS(st))
			{
				TArb scos = calloc(1,sizeof(TNodArb));
				PopS(st,scos);
				TArb tata = calloc(1,sizeof(TNodArb));
				int rez = PopS(st,tata);
				if(!rez)
					a = scos;
				else{
				if(!tata->firstChild)
				tata->firstChild = scos;
				else
					{	TArb p = tata->firstChild;
						while(p->nextSibling != NULL)
							p = p->nextSibling;
						p->nextSibling = scos;
					}
				PushS(st,(void*)tata);
				}
			}
		}
		else if(currentState == 5 && nextState == 6) 
			tmp[strlen(tmp)] = c;//atribut
		else if(currentState == 6 && nextState == 6)
			tmp[strlen(tmp)] = c;
		else if(currentState == 8 && nextState == 5)
			printf("sf at val\n");//adauga valoarea atributului
		else if(currentState == 8 && nextState == 8)
			value[strlen(value)] = c;
		else if(currentState == 9 && nextState == 1){
			printf("self closing\n");
			TArb scos = calloc(1,sizeof(TNodArb));
			int rez = PopS(st,scos);
			if(rez)
			{
			if(!scos->info)	
				scos->info = calloc(1,sizeof(TNodInfo));
			scos->info->isSelfClosing = 1;
			PushS(st,(void*)scos);
			}
		}
		currentState = nextState;
	}
	printf("elem stiva = %d\n",nr);
	for(b = a;b != NULL;b = b->nextSibling){
		if(b->info && b->info->contents)
		printf("%s\n",b->info->contents);
		else
			printf("tata\n");
	}
	for(b = a->firstChild; b != NULL ; b = b->nextSibling){
		if(b->info->contents && b->info->type)
		printf("<%s>%s</%s>",b->info->type,b->info->contents,b->info->type);
		else printf("nu ");
	}

	return 0;
}