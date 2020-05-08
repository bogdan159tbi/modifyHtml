#include "lib.h"
#define MAX_ATTR 10

char **attrs(char *value,int *elem)
{	
	*elem = 0;
	char **atribute = calloc(MAX_ATTR,sizeof(char*));
	if(!atribute)
		return NULL;
	char *p = strtok(value,";");
	while(p)
	{
		atribute[*elem] = calloc(100,sizeof(char));
		strcpy(atribute[*elem],p);
		(*elem)++;
		p = strtok(NULL,";");
	}
	return atribute;
}
void trim(char **value)// elimina spatii valoare atribut style
{	int nr = 0;
	char *aux = calloc(30,sizeof(char));
	for(int i = 0; (*value)[i] != '\0';i++)
		if((*value)[i] != ' ' && (*value)[i] != '\n')
			aux[nr++] = (*value)[i];
	strcpy(*value,aux);
	free(aux);
}
void nameVal(char **name,char **val,char *eticheta)
{
 char *p;
 p = strtok(eticheta,":");
 strcpy(*name,p);
 p = strtok(NULL,":");
 strcpy(*val,p);
 free(eticheta);
}

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
	char *tmp = calloc(30,sizeof(char)),*value = calloc(100,sizeof(char));
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
			if(!new->info->type)
				new->info->type = calloc(30,sizeof(char));
			new->info->type[strlen(new->info->type)] = c;
			if(!st)
				st = InitS(sizeof(TNodArb));
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
				
				if(!rez){
					a = scos;
					//printf("%s\n",a->info->type);
				}
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
		else if(currentState == 5 && nextState == 6){
			tmp= calloc(100,sizeof(char));
			tmp[strlen(tmp)] = c;//atribut
		}
		else if(currentState == 6 && nextState == 6)
			tmp[strlen(tmp)] = c;
		else if(currentState == 8 && nextState == 5){
			//adauga valoarea atributului	
			//printf("%s=\"%s\"\n",tmp,value);
			TArb scos = calloc(1,sizeof(TNodArb));
			int rez = PopS(st,scos);
			if(rez)
			{
			if(!scos->info)	
			scos->info = calloc(1,sizeof(TNodInfo));
			if(strcmp(tmp,"style")){
				if(!scos->info->otherAttributes)
				{
					scos->info->otherAttributes = calloc(1,sizeof(TNodAttr));
					TAttr p = scos->info->otherAttributes;
					p->name = tmp;
					p->value = value;
				}
				else //adauga la sf1
				{
					TAttr p = scos->info->otherAttributes;
					for(; p->next != NULL; p = p->next );
					p->next = calloc(1,sizeof(TNodAttr));
					p = p->next;
					p->name = tmp;
					p->value = value;
				//valoarea
				}	
			}
			else
			{
				//separa termenii pt value si name si ; intre ele						
				int elem;
				char **sir = attrs(value,&elem);
				char *nume,*val;
				for(int i = 0;i < elem;i++){
					nume = calloc(30,sizeof(char));
 					val = calloc(30,sizeof(char));
					nameVal(&nume,&val,sir[i]);
					trim(&val);
					if(!scos->info->style)
						{scos->info->style = calloc(1,sizeof(TNodAttr));
						scos->info->style->name = nume;
						scos->info->style->value = val;
						}
					else //adauga la sf1
					{ 
					TAttr p = scos->info->style;
					for(p = scos->info->style; p->next != NULL; p = p->next );
					p->next = calloc(1,sizeof(TNodAttr));
					p = p->next;
					p->name = nume;
					p->value = val;
					}
				}
			}
			PushS(st,(void*)scos);
			}
			value = calloc(100,sizeof(char));//reinitializeaza
		}
		else if(currentState == 8 && nextState == 8)
			value[strlen(value)] = c;
		else if(currentState == 9 && nextState == 1){
			//self closing
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
	
	for(b = a;b != NULL;b = b->nextSibling){
		if(b->info->type)
			printf("%s\n",b->info->type);
		else
			printf("tata\n");
		for(TArb p = b->firstChild; p != NULL ; p = p->nextSibling){
			if(p->info)
				{if(p->info->type)
					printf("%s ",p->info->type);
				}
			else printf("nu ");
			if(p->info->style){
			TAttr px = p->info->style;
			for(;px != NULL;px = px->next)
			printf("%s %s ",px->name,px->value);
			}	
	
		}
	}
	
	return 0;
}