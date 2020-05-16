#include "lib.h"
#define MAX_ATTR 1000

int doarSpatii(char *content)
{
	for(int i = 0; i < strlen(content); i++)
		if(content[i] != ' ')
			return 0;
	return 1;
}
char **attrs(char *value,int *elem)
{	
	*elem = 0;
	char **atribute = calloc(MAX_ATTR,sizeof(char*));
	if(!atribute)
		return NULL;
	char *p = strtok(value,";");
	while(p)
	{
		if(!doarSpatii(p))
		{
		atribute[*elem] = calloc(1024,sizeof(char));
		strcpy(atribute[*elem],p);
		(*elem)++;
		}
		p = strtok(NULL,";");
	}
	return atribute;
}
void trim(char **value)// elimina spatii valoare atribut style
{	int nr = 0;
	char *aux = calloc(1024,sizeof(char));
	for(int i = 0; (*value)[i] != '\0';i++)
		if((*value)[i] != ' ' && (*value)[i] != '\n')
			aux[nr++] = (*value)[i];
	strcpy(*value,aux);
	free(aux);
}
void nameVal(char **name,char **val,char *eticheta)
{
 char *p,*copie = malloc(100);
 strcpy(copie,eticheta);
 p = strtok(copie,":");
 strcpy(*name,p);
 if(p)
 	p = strtok(NULL,":");
 if(p)
 	strcpy(*val,p);
free(copie);
}
void adaugaID(char **id)
{	
	char *copie = calloc(20,sizeof(char));
	strcpy(copie,*id);
	char *p = strtok(copie,".");
	char ultim;
	while(p)
	{
		ultim = p[0];
		p = strtok(NULL,".");
	}
	ultim++;
	(*id)[strlen(*id)-1] = ultim;
	free(copie);
}
//parcurgere in latime arbore
void bfsID(TArb root)
{
	void *queue = InitQ(sizeof(TNodArb));
	IntrQ(queue,(void*)root);
	
	while(!EMPTYQ(queue))
	{
		TArb scos = calloc(1,sizeof(TNodArb));// TRE ELIBERAT
		ExtrQ(queue,scos);
		TArb p;
		char id = '1';
		for(p = scos->firstChild; p != NULL ; p = p->nextSibling){
			if(scos->info->id == NULL){
				if(!p->info->id)
					p->info->id = calloc(30,sizeof(char));
				p->info->id[0] = id;
				id++;
			}
			else
			{	if(!p->info->id)
				p->info->id = calloc(30,sizeof(char));
				strcpy(p->info->id,scos->info->id);
				strcat(p->info->id,".");
				p->info->id[strlen(p->info->id)] = id++;
			}
			IntrQ(queue,(void*)p);
		}
	}
}

//afisare cu indentare
void RSD(TArb root,int *taburi,FILE *out)
{
	if(!root)
		return;
	else
	{	for(int i = 0; i < *taburi; i++)
			fprintf(out,"\t");
		if(root->info)
			fprintf(out,"<%s",root->info->type);

		if(root->info->style)
		{
		fprintf(out," style=\"");
		TAttr st;
		if(root->info->style->next)
		{
			for( st = root->info->style; st->next != NULL; st = st->next)
			{
				if(st->name && st->value)
					fprintf(out,"%s: %s; ",st->name,st->value);
			}
			if(st->name && st->value)
				fprintf(out,"%s: %s;\"",st->name,st->value);//la ultimul atribute nu exista spatiu intre ; si "  de la sfarsit
		}
		else
			fprintf(out,"%s: %s;\"",root->info->style->name,root->info->style->value);
		}
		
		if(root->info->otherAttributes)
		{	fprintf(out," ");
			TAttr attr;
			if(root->info->isSelfClosing == 0){
				for(attr = root->info->otherAttributes; attr->next != NULL; attr = attr->next)
					fprintf(out,"%s=\"%s\"",attr->name,attr->value);
				fprintf(out,"%s=\"%s\"",attr->name,attr->value);
					}
			else
			{
				for(attr = root->info->otherAttributes; attr->next != NULL; attr = attr->next)
					fprintf(out,"%s=\"%s\" ",attr->name,attr->value);
				fprintf(out,"%s=\"%s\"",attr->name,attr->value);
			}
		}
		if(root->info->isSelfClosing == 0){
			fprintf(out,">\n");
		//am afisat atributele la stil si celelalte atribute 
		//acum urmeaza continut daca e dif de null
			if(root->info->contents && strlen(root->info->contents) > 0){
				for(int i = 0; i < (*taburi); i++)
					fprintf(out,"\t");
				fprintf(out,"\t");
				fprintf(out,"%s\n",root->info->contents);
			}
		}
		(*taburi)++;
		for(TArb p = root->firstChild; p != NULL ;p = p->nextSibling)
			RSD(p,taburi,out);
		(*taburi)--;
		if(root->info->isSelfClosing == 0){
			for(int i = 0; i < *taburi; i++)
				fprintf(out,"\t");
			fprintf(out,"</%s>\n",root->info->type);
		}
		else fprintf(out,"/>\n");

	}
}
//cautarea se face pe nivele,adica parcurg in latime
int cautaID(TArb root,char *id,TArb *frate)
{
	if(!root)
		return 0;
	if(root->info->id){
	if(!strcmp(root->info->id,id)){
		//fprintf("%s\n",root->info->id);
		*frate = root;
		return 1;
	}
	}
	for(TArb p = root->firstChild; p != NULL ;p = p->nextSibling)
		cautaID(p,id,frate);
	return 0;
}
int cautaClass(TArb root,char *valueClass,TArb *tata)
{
	if(!root)
		return 0;
	if(root->info->otherAttributes)
	{	
		TAttr class;
		for( class = root->info->otherAttributes; class != NULL ; class = class->next)
			if(!strcmp(class->name,"class") && !strcmp(class->value,valueClass))
				break;
		if(class && !(*tata)){
		*tata = root;
		return 1;
		}
	}	
	for(TArb p = root->firstChild; p != NULL ;p = p->nextSibling)
		cautaClass(p,valueClass,tata);
	return 0;
}
//functie care returneaza tag cu clasa ceruta si modifica id-urile celorlalte
int changeClass(TArb root,char *valueClass,TArb *tata)
{
	if(!root)
		return 0;
	if(root->info->otherAttributes)
	{	
		TAttr class;
		for( class = root->info->otherAttributes; class != NULL ; class = class->next)
			if(!strcmp(class->name,"class") && !strcmp(class->value,valueClass))
				break;
		if(class && !(*tata)){
		*tata = root;
		return 1;
		}
	}	
	TArb p;
	for( p = root->firstChild; p != NULL ;p = p->nextSibling){
		if(!(*tata))
		 changeClass(p,valueClass,tata);
	}
	return 0;

}
int findClass(TArb r,char *class)
{
	TAttr attr = r->info->otherAttributes;
	for(; attr ; attr = attr->next)
		if(!strcmp(class,attr->value) && !strcmp(attr->name,"class"))
			return 1;
}
int cautaTag(TArb root,char *type,TArb *tata)
{
	if(!root)
		return 0;

	if(root->info->type){
	if(!strcmp(root->info->type,type) && !(*tata)){
		//fprintf("%s\n",root->info->id);
		*tata = root;
		return 1;
	}
	}
	for(TArb p = root->firstChild; p != NULL ;p = p->nextSibling)
		cautaTag(p,type,tata);
	return 0;
}

TArb *parents(TArb root,char *type,int *elem,TArb *noduri)
{
	int i;
	if(!root)
		return NULL;
	if(root->info->type && !strcmp(root->info->type,type))
	{	if(!noduri[*elem])
			noduri[*elem]  = calloc(1,sizeof(TNodArb));
		noduri[*elem] = root; 
		(*elem)++;
	}
	for(TArb p = root->firstChild; p != NULL ;p = p->nextSibling)
		parents(p,type,elem,noduri);
	return noduri;
}
//functie care retine parintii tuturor tag-urile cerute de selector
// de ex daca se cere la selector tag p sters ,retin toti parintii tuturor tag-urilor p
TArb *roots(TArb root,char *type,int *elem,TArb *noduri)
{
	int i;
	if(!root)
		return NULL;
	TArb fiu = NULL;
	cautaTag(root,type,&fiu);
	if(fiu)
	{
		if(!noduri[*elem])
			noduri[*elem]  = calloc(1,sizeof(TNodArb));
		noduri[*elem] = root; 
		(*elem)++;
	}
	for(TArb p = root->firstChild; p != NULL ;p = p->nextSibling)
		roots(p,type,elem,noduri);
	
	return noduri;
}

int addTag(TArb a,char *cmd,FILE *out)
{
	char *p = strtok(cmd," ");
		char *parinte = calloc(100,sizeof(char));
	   	p = strtok(NULL," ");
		strcpy(parinte,p);		

		char *tagHTML =calloc(100,sizeof(char));

		p = strtok(NULL,"=");
		p = strtok(NULL,"\"");
		strcpy(tagHTML,p);	
		char *tagtype = calloc(100,sizeof(char));
		strcpy(tagtype,tagHTML);
			
		char *idParinte = calloc(30,sizeof(char));
		p = strtok(parinte,"=");
		p = strtok(NULL,"=");
		strcpy(idParinte,p);
		TArb frate = NULL;

		int rez = cautaID(a,idParinte,&frate);
		if(frate)
			{
			TArb fiu = calloc(1,sizeof(TNodArb));
			TParseState currentState = 1,nextState;
			for(int j = 0; j < strlen(tagtype); j++){
				nextState = Interpret(currentState,tagtype[j]);
				//sa verific ce fac cu spatiile de la content la fiecare tag adaugat			
				if(currentState == 1 && nextState == 1) // nu tre 1 si 2
					{	
						if(!fiu->info)
				 			fiu->info = calloc(1,sizeof(TNodInfo));
						if(!fiu->info->contents)
							fiu->info->contents = calloc(1000,sizeof(char));
						if(tagtype[j] != ' ')
							fiu->info->contents[strlen(fiu->info->contents)] = tagtype[j];
					}
				else if(currentState == 2 && nextState == 3){
						fiu = calloc(1,sizeof(TNodArb));
						if(!fiu->info)
							fiu->info = calloc(1,sizeof(TNodInfo));
						if(!fiu->info->type)
							fiu->info->type = calloc(30,sizeof(char));
						fiu->info->type[strlen(fiu->info->type)] = tagtype[j];

					}
				else if(currentState == 3 && nextState == 3)
						{
						if(!fiu->info)
							fiu->info = calloc(1,sizeof(TNodInfo));
						if(!fiu->info->type)
							fiu->info->type = calloc(30,sizeof(char));
						fiu->info->type[strlen(fiu->info->type)] = tagtype[j];
						}
				else if(currentState == 4 && nextState == 1){
						TArb kid = NULL;
						if(frate->firstChild == NULL)
							frate->firstChild = fiu;
						else{
							for( kid = frate->firstChild; kid->nextSibling != NULL;kid = kid->nextSibling);
							kid->nextSibling = fiu;
							}
																}

				currentState = nextState;
					}
				
				}
		else fprintf(out,"Add tag failed: node with id %s not found!\n",idParinte);
		return rez;
				
}

void delParentTree(TArb root)
{
	if(!root)
		return;
	TArb child;
	for(child = root->firstChild; child != NULL; child = child->nextSibling)
		{delParentTree(child);
		 //elibereaza nod
		 
		}
}

void freeAttr(TAttr atribut)
{
	if(atribut){
	free(atribut->name);
	free(atribut->value);
	}
}
void freeNOD(TArb nod)
{
	if(nod->info)
	{
		/*
		if(nod->info->type)
			free(nod->info->type);
		if(nod->info->id)
			free(nod->info->id);
		*/
		for(TAttr lista = nod->info->otherAttributes; lista != NULL; )
		{
			TAttr aux = lista;
			lista = lista->next;
			freeAttr(aux);
			free(aux);
		}
		for(TAttr lista = nod->info->style; lista != NULL; )
		{
			TAttr aux = lista;
			lista = lista->next;
			freeAttr(aux);
			free(aux);
		}

	}	

}
//functie care sa elibereze recursiv subarbore
//parcurg in adancime ca sa eliberez intai nodurile frunza
//parcurd srd si folosesc o alta functie ce elibereaza toate campurile unui nod
void delSubtree(TArb root)
{
	if(!root)
		return;
	for(TArb kid = root->firstChild; kid != NULL; kid = kid->nextSibling)
		delSubtree(kid);
	freeNOD(root);
}	

int delClass(TArb root,char *class)
{
	TArb tag = NULL; 
	int primul = 0;
	changeClass(root,class,&tag);
	if(tag->info->id[strlen(tag->info->id)-1] == '1')
		{
			TArb tata = NULL;
			char *id = malloc(10);
			strcpy(id,tag->info->id);
			id[strlen(id) - 2] = '\0';
			cautaID(root,id,&tata);
			tata->firstChild = tag->nextSibling;
			return 1;
		}
	else if(!tag)
		return 0;//nu exista ,tre sa dau si ffprintf
	else
		{
		//delParentTree(tata);
		TArb bro = NULL;
		char *id = malloc(10);
		strcpy(id,tag->info->id);
		id[strlen(id) - 1] = id[strlen(id) -1] - 1;
		cautaID(root,id,&bro);
		//elibereaza ce tre sters
		bro->nextSibling = tag->nextSibling;//refac legatura intre anterior si urm la frati
		return 1;
		}
}
int delTag(TArb root,char *type)
{
	TArb tag = NULL;
	int first = 0;
	cautaTag(root,type,&tag);
	if(tag){
		if(tag->info->id && tag->info->id[strlen(tag->info->id)-1] == '1')
			{
				TArb tata = NULL;
				char *id = malloc(100);
				strcpy(id,tag->info->id);
				id[strlen(id) - 2] = '\0';
				cautaID(root,id,&tata);
				tata->firstChild = tag->nextSibling;
				delTag(root,type);
				bfsID(tata);
				return 1;
			}
		else
			{
			//delParentTree(tata);
			TArb bro = NULL;
			char *id = malloc(100);
			strcpy(id,tag->info->id);
			id[strlen(id) - 1] = id[strlen(id) -1] - 1;
			cautaID(root,id,&bro);
			//elibereaza ce tre sters
			bro->nextSibling = tag->nextSibling;//refac legatura intre anterior si urm la frati
			delTag(root,type);
			bfsID(bro);
			return 1;
			}
	}
	else return 0;//nu exista ,tre sa dau si ffprintf
}
void deleteID(TArb dad,char *id)
{
	if(id[strlen(id)- 1] == '1' ) //iau firstChild
	{	
		TArb child;
		child = dad->firstChild;
		dad->firstChild = child->nextSibling;	
		delSubtree(child);	
		//free(child);	
	}
	else if(id[strlen(id)- 1] != '1' )
	{
		TArb kids,ant = NULL;
		for(kids = dad->firstChild ; kids != NULL ; ant = kids,kids = kids->nextSibling)
		if(!strcmp(kids->info->id,id))
			break;
		if(kids)
			ant->nextSibling = kids->nextSibling;
		delSubtree(kids);
		//free(kids);	
	}

}

void addAttr(TAttr *new,char *nume,char *val)
{
	if(!(*new))
		{*new = calloc(1,sizeof(TNodAttr));
		 if(!(*new))
			return;
		 (*new)->name = nume;
		 (*new)->value = val;
		}
	else
	{	TAttr p,ant = NULL;
		int ok = 0;
		for( p = *new; p != NULL; ant = p,p = p->next)
			if(!strcmp(p->name,nume)){
				ok = 1;
				p->value = val;
			}
		//daca n am gasit niciun astfel de atribut
		if(!ok && ant)
		{
		ant->next = calloc(1,sizeof(TNodAttr));
		ant->next->name = nume;
		ant->next->value = val;
		}
	}
}
//modifica valorile din lista style pt atribute a nodului root
void overrideStyle(TArb root,char **style,int elem)
{
	TAttr atr = root->info->style;
	//creez o lista cu noile atribute pt style pe care o adaug la info
	// si pe cealalta o sterg
	for(TAttr lista = root->info->style; lista != NULL; )
		{
			TAttr aux = lista;
			lista = lista->next;
			//freeAttr(aux);//daca da segfault posibil sa fie de aici DA,DA SEGUL FAULT
			free(aux); 
		}
	root->info->style = NULL;
	TAttr new = NULL;
	//tre sa iterez prin tot vectorul style care contine nume atribut si valoarea sa
	char *nume,*val;
	for(int i = 0; i < elem ; i++){
		nume = malloc(20);
		val = malloc(20);
		nameVal(&nume,&val,style[i]); // dupa care fac trim pe ele
		trim(&nume);
		trim(&val);
		//adauga in lista
		addAttr(&(root->info->style),nume,val);
	}
}

void bfsTAG(TArb root,char **styleValues,int elem,char *tagtype)
{
void *queue = InitQ(sizeof(TNodArb));
IntrQ(queue,(void*)root);

while(!EMPTYQ(queue))
{
	TArb scos = calloc(1,sizeof(TNodArb));// TRE ELIBERAT
	ExtrQ(queue,scos);
	TArb p;
	if(!strcmp(scos->info->type,tagtype))
		overrideStyle(scos,styleValues,elem);
	
	for(p = scos->firstChild; p != NULL ; p = p->nextSibling){
		if(!strcmp(p->info->type,tagtype))
			overrideStyle(p,styleValues,elem);
		IntrQ(queue,(void*)p);
		}
}

}
int overrideStyleTAG(TArb root,char *type,char **style,int elem)
{
	if(!root)
		return 0;
	if(root->info->type){
	if(!strcmp(root->info->type,type))
		bfsTAG(root,style,elem,type);
	
	}
	for(TArb p = root->firstChild; p != NULL ;p = p->nextSibling)
		overrideStyleTAG(p,type,style,elem);
	return 0;
}
void appendStyle(TArb r,char **style,int elem)
{
	TAttr st = r->info->style,ant;
	int i;
	char *nume,*val;
	for(i = 0; i < elem; i++)
	{
		nume = malloc(50);
		val = malloc(50);
		nameVal(&nume,&val,style[i]); // dupa care fac trim pe ele
		trim(&nume);
		trim(&val);
		int ok = 0;
		for(ant = NULL,st = r->info->style; st != NULL ; ant = st,st = st->next)
			if(!strcmp(st->name,nume))
			{	
				ok = 1;
				st->value = val;
			}
		if(!ok && ant)
			{	
			ant->next = calloc(1,sizeof(TNodAttr));
			ant->next->name = nume;
			ant->next->value = val;
			}
		else if(!ok && !ant)
		{
			r->info->style = calloc(1,sizeof(TNodAttr));
			r->info->style->name = nume;
			r->info->style->value = val;
		}
	}
}
void bfsAppendTag(TArb root,char **styleValues,int elem,char *type)
{
void *queue = InitQ(sizeof(TNodArb));
IntrQ(queue,(void*)root);

	while(!EMPTYQ(queue))
	{
		TArb scos = calloc(1,sizeof(TNodArb));// TRE ELIBERAT
		ExtrQ(queue,scos);
		TArb p;
		if(!strcmp(scos->info->type,type)){
			appendStyle(scos,styleValues,elem);//overrideStyle(scos,styleValues,elem);
		}
		
		for(p = scos->firstChild; p != NULL ; p = p->nextSibling){
			if(!strcmp(p->info->type,type))
				appendStyle(p,styleValues,elem);//overrideStyle(p,styleValues,elem);
			IntrQ(queue,(void*)p);
			}
	}
}
void bfsAppendID(TArb root,char **styleValues,int elem,char *id)
{
void *queue = InitQ(sizeof(TNodArb));
IntrQ(queue,(void*)root);

	while(!EMPTYQ(queue))
	{
		TArb scos = calloc(1,sizeof(TNodArb));// TRE ELIBERAT
		ExtrQ(queue,scos);
		TArb p;
		if(!strcmp(scos->info->id,id))
			appendStyle(scos,styleValues,elem);//overrideStyle(scos,styleValues,elem);
		
		for(p = scos->firstChild; p != NULL ; p = p->nextSibling){
			if(!strcmp(p->info->id,id))
				appendStyle(p,styleValues,elem);//overrideStyle(p,styleValues,elem);
			IntrQ(queue,(void*)p);
			}
	}
}



void findID(TArb root,char **styleValues,int elem,char *id)
{
void *queue = InitQ(sizeof(TNodArb));
IntrQ(queue,(void*)root);

while(!EMPTYQ(queue))
{
	TArb scos = calloc(1,sizeof(TNodArb));// TRE ELIBERAT
	ExtrQ(queue,scos);
	TArb p;

	if(!strcmp(scos->info->id,id))
		overrideStyle(scos,styleValues,elem);
	
	for(p = scos->firstChild; p != NULL ; p = p->nextSibling){
		if(!strcmp(p->info->id,id))
			overrideStyle(p,styleValues,elem);
		IntrQ(queue,(void*)p);
		}
}
}

int exista(TArb root,char *class)
{
	for(TAttr p = root->info->otherAttributes; p != NULL; p = p->next)
	{
		if(!strcmp(p->value,class) && !strcmp(p->name,"class"))
			return 1;
	}
	return 0;
}

int bfsAppendClass(TArb root,char **styleValues,int elem,char *class)
{void *queue = InitQ(sizeof(TNodArb));
IntrQ(queue,(void*)root);
int ok = 0;
	while(!EMPTYQ(queue))
	{
		TArb scos = calloc(1,sizeof(TNodArb));// TRE ELIBERAT
		ExtrQ(queue,scos);
		TArb p;
		if(exista(root,class)){
			ok = 1;
			appendStyle(scos,styleValues,elem);
		}
	
		for(p = scos->firstChild; p != NULL ; p = p->nextSibling){
			if(exista(p,class)){
				ok = 1;
				appendStyle(p,styleValues,elem);
			}
			IntrQ(queue,(void*)p);
		}
	}
	return (ok == 1);
}
void bfsClass(TArb root,char **styleValues,int elem,char *class)
{
void *queue = InitQ(sizeof(TNodArb));
IntrQ(queue,(void*)root);
while(!EMPTYQ(queue))
{
	TArb scos = calloc(1,sizeof(TNodArb));// TRE ELIBERAT
	ExtrQ(queue,scos);
	TArb p;
	if(exista(root,class))
		overrideStyle(scos,styleValues,elem);
	
	for(p = scos->firstChild; p != NULL ; p = p->nextSibling){
		if(exista(p,class))
			overrideStyle(p,styleValues,elem);
		IntrQ(queue,(void*)p);
		}
}

}

int spatii(char *val)
{
	//pentru func de override in caz ca selectorul este tag tag sa stiu ca tre retinute 2 val
	int nr = 0;
	for(int i = 0; i < strlen(val); i++){
		if(val[i] == '\"')
			break;
		if(val[i] == ' ')
			nr++;
	}
	if(nr >= 1)
		return 2;
	return 0;
}
int main(int argc, char **argv)
{
	FILE *in = fopen(argv[1],"r");
	if(!in)
		return 1;
	char c = ' ';
	TArb a = calloc(1,sizeof(TNodArb)),child,b,new;
	if(!a)
		return 1;
	b = a;
	TParseState currentState = 1,nextState;
	char *tmp = calloc(1024,sizeof(char)),*value = calloc(1024,sizeof(char));
	void *st = NULL;
	int nr = 0;
	while((c = fgetc(in)) != EOF)
		{
		nextState = Interpret(currentState,c);
		//fprintf("%c %d %d\n",c,currentState,nextState);
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
				scos->info->contents = calloc(100,sizeof(char));
			if((int)c != 13) // lua contents fara niciun carac
			scos->info->contents[strlen(scos->info->contents)] = c;
			
			PushS(st,(void*)scos);
			}
		}
		else if(currentState == 2 && nextState == 3){
			new = calloc(1,sizeof(TNodArb));
			if(!new->info)
				new->info = calloc(1,sizeof(TNodInfo));
			if(!new->info->type)
				new->info->type = calloc(100,sizeof(char));
			new->info->type[strlen(new->info->type)] = c;
			if(!new->info->contents)
				new->info->contents = calloc(100,sizeof(char));
			if(!st)
				{new->info->id = NULL;
				st = InitS(sizeof(TNodArb));
				}
			else 
				new->info->id = calloc(20,sizeof(char));

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
				scos->info->type = calloc(100,sizeof(char));
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
				if(!tata->firstChild){
					tata->firstChild = scos;
				}
				else
					{	
					TArb p = tata->firstChild;
					while(p->nextSibling != NULL)
						p = p->nextSibling;
					p->nextSibling = scos;
					}
					PushS(st,(void*)tata);
				}
			}
		}
		else if(currentState == 5 && nextState == 6){
			tmp= calloc(1024,sizeof(char));
			tmp[strlen(tmp)] = c;//atribut
		}
		else if(currentState == 6 && nextState == 6)
			tmp[strlen(tmp)] = c;
		else if(currentState == 8 && nextState == 5){
			//adauga valoarea atributului	
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
					//fprintf("%s=%s\n",tmp,value);
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
				//fprintf("value= %s\n",value);
				char **sir = attrs(value,&elem);
				char *nume,*val;
				for(int i = 0;i < elem;i++){
					nume = calloc(1024,sizeof(char));
 					val = calloc(1024,sizeof(char));
					nameVal(&nume,&val,sir[i]);
					trim(&val);
					trim(&nume);
					if(!scos->info->style)
						{
						scos->info->style = calloc(1,sizeof(TNodAttr));
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
			value = calloc(1024,sizeof(char));//reinitializeaza
		}
		else if(currentState == 8 && nextState == 8)
			value[strlen(value)] = c;
		else if(currentState == 9 && nextState == 1){
			//self closing
			TArb scos = calloc(1,sizeof(TNodArb));
			PopS(st,scos);
			TArb tata = calloc(1,sizeof(TNodArb));
			int rez = PopS(st,tata);
			if(rez)
			{
			if(!scos->info)	
				scos->info = calloc(1,sizeof(TNodInfo));
			if(!scos->info->type)
				scos->info->type = calloc(100,sizeof(char));
			if(!scos->info->contents)
				scos->info->contents = calloc(100,sizeof(char));
			if(!scos->info->id)
				scos->info->id = calloc(100,sizeof(char));
			scos->info->isSelfClosing = 1;
			if(!tata->firstChild)
				tata->firstChild = scos;
			else
				{	
				TArb p = tata->firstChild;
				while(p->nextSibling != NULL)
					p = p->nextSibling;
				p->nextSibling = scos;
				}
			}
			else  a = scos;

			PushS(st,(void*)tata);
		}
		currentState = nextState;
	}
	//adaugare id prin parcurgere in latime
	bfsID(a);
	int taburi = 0;
	FILE *comenzi = fopen(argv[2],"r") , *out = fopen(argv[3],"w");

	int linii;
	if(!comenzi)
		return 1;
	fscanf(comenzi,"%d",&linii);
	for(int i = 1; i <= linii+1 ;i++){
		char *cmd = calloc(1024,sizeof(char)); // lungimea unei comenzi citite din fisier
		fgets(cmd,1024,comenzi);
		if(strlen(cmd) > 1){
			cmd[strlen(cmd)-1] = '\0';
		if(strstr(cmd,"add")){
			int rez = addTag(a,cmd,out);//nu stiu daca se garanteaza corectitudinea comenzilor	
			bfsID(a);

		}
		else if(strstr(cmd,"deleteRecursively"))
		{	
			char *selector = cmd + strlen("deleteRecurively") + 2 + strlen("selector=\"");//ca sa obtin selector direct
			selector[strlen(selector) - 1] = '\0'; // ca sa elimin " de la sfarsit
			int rez = 0;
			if(selector[0] == '.')
			{// cautam class la atribute
				rez = delClass(a,selector+1);
				if(rez)
					bfsID(a); 
			}
			else if(!strchr(selector,'>') && !strchr(selector,' ') && !strchr(selector,'.') && !strchr(selector,'#')) 
			{// ca sa iau tag-ul 
				//selector e un singur tag
				//functie care sa elimine recursiv toti subarorii care au root tag ul din selector
				TArb *parinti = calloc(1024,sizeof(TArb));
				int elem = 0;
				roots(a,selector,&elem,parinti);
				//avem toti parintii tag urilor egale cu selector
				//acum sa refac legaturile pentru fii nodurilor
				// si o func care sterge subarborii adica sa elibereze

				for(int i = 0; i < elem ;i++)
				{
					TArb p = parinti[i]->firstChild,ant = NULL;
					for(; p != NULL ; p = p->nextSibling)
					{	
						if(!strcmp(p->info->type,selector))
						{
							if(!ant)
								parinti[i]->firstChild = p->nextSibling;
							else
								ant->nextSibling = p->nextSibling;
							TArb aux = p;
							//free aux adica subarborele
						}
						else
							ant = p;
					}
				}
				bfsID(a);
			}
			else if(strchr(selector,'>')) // cauta tag parinte si copil: tata>copil
			{
				char *tok = strtok(selector,">");
				char *dadtype = malloc(20),*childtype = malloc(20);
				strcpy(dadtype,tok);
				tok = strtok(NULL,">");
				strcpy(childtype,tok);
				//sa verific daca am scos bine tok
				TArb dad = NULL;
				//cautaTag(a,dadtype,&dad);
				//trebuie un vector cu toate tagurile de tip = dadtype
				int nodtata = 0;
				TArb *tati = calloc(100,sizeof(TArb));

				parents(a,dadtype,&nodtata,tati);
				int ok = 0;
				for(int i = 0; i < nodtata; i++)
				{
					dad = tati[i];
					TArb kids,ant = NULL;
					for(kids = dad->firstChild ; kids != NULL ; kids = kids->nextSibling)
					{
						if(!strcmp(kids->info->type,childtype))
						{	ok = 1;
							if(!ant)
								dad->firstChild = kids->nextSibling;
							else
								ant->nextSibling = kids->nextSibling;
							TArb aux = kids;
							//free aux
							rez = 1;
						}
							else
								ant = kids;
					}
					bfsID(dad);
				}
				if(!ok)
					fprintf(out,"Delete recursively failed: no node found for selector %s>%s!\n",dadtype,childtype);
				
			}			
			else if(strchr(selector+1,' '))
			{
				char *dadtype, *childtype;
				char *tok = strtok(selector," ");
				dadtype = tok;
				tok = strtok(NULL," ");
				childtype = tok;
				TArb dad = NULL,son = NULL;
				int result = cautaTag(a,dadtype,&dad);
				if(dad)
					result = 1;
				while(result){ 
					if(dad) // exista nodul tata in arbroe si cauta subarbore cu radacina tata
					{	
						cautaTag(dad,childtype,&son);
						if(son)//daca am gasit tre sa l sterg si sa refac legaturile
						{
							char *idson = malloc(30);
							strcpy(idson,son->info->id);
							char *iddad = malloc(30);
							strcpy(iddad,idson);
							iddad[strlen(iddad) -2 ] = '\0';
							TArb parent = NULL;
							cautaID(dad,iddad,&parent);
							deleteID(parent,idson);
							delSubtree(son);
							bfsID(parent);
						}
						
					}
					dad = NULL;
					son = NULL;
					cautaTag(a,dadtype,&dad);
					cautaTag(dad,childtype,&son);
					if(!son)
						result = 0;
				}

			}
			else if(selector[0] == '#') //foloseste un fel de heap sort
			{	
				char *id = selector + 1;
				char *parentID = malloc(10);
				strcpy(parentID,id);
				parentID[ strlen(id) -2 ] = '\0';
				TArb dad = NULL,child = NULL;
				cautaID(a,parentID,&dad);
				if(dad){
					deleteID(dad,id);
				}
				bfsID(dad);
			}
			
		}
		else if(strstr(cmd,"appendStyle"))
			{
				char *rest = cmd + strlen("appendStyle") + strlen("selector") + 3; //selector=" sunt avansate 
				char *selector = malloc(200);
				int exista = spatii(rest);
				char *select = NULL;
				char *style;
				if(exista == 2){
					char *rest2 = malloc(200);
					strcpy(rest2,rest);
					select = strtok(rest,"\"");
					rest2 = rest2+strlen(select) + 2 + strlen("style=\"");
					rest2[strlen(rest2)-1] = '\0';
					style = rest2;
				}
				else{
				char *tok = strtok(rest,"\"");
				strcpy(selector,tok);
				tok = strtok(NULL,"\"");
				tok = strtok(NULL,"\"");
				style = tok;
				}//am obtinut atributele pe care trebuie sa le foloses pt style
				if(!select)
				{
					if(strchr(selector,'>')) // sa caut toate tag urile,pana acum ia doar 1
					{	
						char *tok = strtok(selector,">");
						char *dadtype = malloc(20),*childtype = malloc(20);
						strcpy(dadtype,tok);
						tok = strtok(NULL,">");
						strcpy(childtype,tok);
						//sa verific daca am scos bine tok
						TArb dad = NULL;
						TArb *tati = calloc(20,sizeof(TArb)),*fii = calloc(20,sizeof(TArb));
						int nr1 = 0,nr2 = 0;
						parents(a,dadtype,&nr1,tati);
						for(int i = 0 ; i < nr1 ;i++)
						{
							TArb kids;
							for(kids = tati[i]->firstChild ; kids != NULL ; kids = kids->nextSibling){
								if(!strcmp(kids->info->type,childtype))
								{
									int elem = 0;
									char **atribute = attrs(style,&elem);
									appendStyle(kids,atribute,elem);
								}	
							}
						}
					}	
					else if(!strchr(selector,'>') && !strchr(selector,' ') && !strchr(selector,'.') && !strchr(selector,'#')) 
						{

							int elem = 0;
							char **atribute = attrs(style,&elem);
							bfsAppendTag(a,atribute,elem,selector);
							
						}
					else if(selector[0] == '.')//cautam clasa data de selector
					{
						int elem = 0;
						char **atribute = attrs(style,&elem);
						int ok = bfsAppendClass(a,atribute,elem,selector + 1);// ca sa trec peste punct
						if(!ok)
							fprintf(out,"Append to style failed: no node found for selector %s!\n",selector);
					}
					else if(selector[0] == '#') 
					{
						int elem = 0;
						char **atribute = attrs(style,&elem);
						char *id = selector + 1;
						char *parentID = malloc(10);
						strcpy(parentID,id);
						parentID[ strlen(id) -2 ] = '\0';
						TArb dad = NULL,child = NULL;
						cautaID(a,parentID,&dad);
						if(dad)
							bfsAppendID(dad,atribute,elem,id);//findID(dad,atribute,elem,id);	
					}
					else if(strchr(selector+1,'.'))
					{
						int elem = 0;
						char **atribute = attrs(style,&elem);
						char *tok = strtok(selector,".");
						char *type = tok;
						tok = strtok(NULL,".");
						char *class = tok;
						TArb tag = NULL; // tre sa fac o functie cautare tag care sa returneze vector cu toate tag-urile
						TArb *tati = calloc(20,sizeof(TArb));
						int nr = 0;
						parents(a,type,&nr,tati);
						int ok = 0;
						for(int i = 0 ;i < nr ;i++)
						{
							TArb tag = tati[i];
							if(findClass(tag,class)){
								ok = 1;
								appendStyle(tag,atribute,elem);
							}
						}
						if(!ok)
							fprintf(out,"Append to style failed: no node found for selector %s.%s!\n",type,class);

					}
				}	
				else
				{
					int elem = 0;
					char **atribute = attrs(style,&elem);
					char *dadtype, *childtype;
					char *tok = strtok(select," ");
					dadtype = tok;
					tok = strtok(NULL," ");
					childtype = tok;
					TArb dad = NULL,son = NULL;
					TArb *tati = calloc(1024,sizeof(TArb)),*fii = calloc(1024,sizeof(TArb));
					int nr1 = 0,nr2 = 0;
					parents(a,dadtype,&nr1,tati);

					for(int i = 0 ; i < nr1 ;i++)
					{
						parents(tati[i],childtype,&nr2,fii);
						for(int j = 0 ; j < nr2;j++){
							//printf("%s %s\n",tati[i]->info->type,fii[j]->info->type);
							appendStyle(fii[j],atribute,elem);
						}
					}
					//if(dad) // exista nodul tata in arbrore si cauta subarbore cu radacina tata
					//	bfsAppendTag(dad,atribute,elem,childtype);//nu stiu daca ia toate tag-urile
					
				}

			}
		else if(strstr(cmd,"overrideStyle"))
			{
				char *rest = cmd + strlen("overrideStyle") + strlen("selector") + 3; //selector=" sunt avansate 
				char *selector = malloc(20);
				int exista = spatii(rest);
				char *select = NULL;
				char *style;
				if(exista == 2){
					char *rest2 = malloc(50);
					strcpy(rest2,rest);
					select = strtok(rest,"\"");
					
					rest2 = rest2+strlen(select) + 2 + strlen("style=\"");
					rest2[strlen(rest2)-1] = '\0';
					style = rest2;
				}
				else{
				char *tok = strtok(rest,"\"");
				strcpy(selector,tok);
				//selector[strlen(selector) -1 ] = '\0';
				tok = strtok(NULL,"\"");
				tok = strtok(NULL,"\"");
				style = tok ;
				}//am obtinut atributele pe care trebuie sa le foloses pt style
				if(!select)
				{
					if(strchr(selector,'>')) // cauta tag parinte si copil: tata>copil
					{	

						char *tok = strtok(selector,">");
						char *dadtype = malloc(20),*childtype = malloc(20);
						strcpy(dadtype,tok);
						tok = strtok(NULL,">");
						strcpy(childtype,tok);
						//sa verific daca am scos bine tok
						TArb dad = NULL;
						TArb *tati = calloc(20,sizeof(TArb)),*fii = calloc(20,sizeof(TArb));
						int nr1 = 0,nr2 = 0;
						parents(a,dadtype,&nr1,tati);
						for(int i = 0; i< nr1; i++)
						{
							TArb kids = NULL;
							int elem = 0;
							char *copie = malloc(100);
							strcpy(copie,style);
							char **atribute = attrs(copie,&elem);
							for(kids = tati[i]->firstChild ; kids != NULL ; kids = kids->nextSibling)
							{
								if(!strcmp(kids->info->type,childtype))
									overrideStyle(kids,atribute,elem);
							}
							
						}
					}	
					else if(!strchr(selector,'>') && !strchr(selector,' ') && !strchr(selector,'.') && !strchr(selector,'#')) 
						{
							int elem = 0;
							char **atribute = attrs(style,&elem);
							bfsTAG(a,atribute,elem,selector);
						}
					else if(selector[0] == '.')//cautam clasa data de selector
					{
						int elem = 0;
						char **atribute = attrs(style,&elem);
						bfsClass(a,atribute,elem,selector + 1); // ca sa trec peste punct
					}
					else if(selector[0] == '#') 
					{
						int elem = 0;
						char **atribute = attrs(style,&elem);
						char *id = selector + 1;
						char *parentID = malloc(10);
						strcpy(parentID,id);
						parentID[ strlen(id) -2 ] = '\0';
						TArb dad = NULL,child = NULL;
						cautaID(a,parentID,&dad);
						if(dad)
							findID(dad,atribute,elem,id);	
						
					}
					else if(strchr(selector+1,'.'))
					{
						int elem = 0;
						char **atribute = attrs(style,&elem);
						char *tok = strtok(selector,".");
						char *type = tok;
						tok = strtok(NULL,".");
						char *class = tok;
						TArb tag = NULL; // tre sa fac o functie cautare tag care sa returneze vector cu toate tag-urile
						TArb *tati = calloc(20,sizeof(TArb));
						int nr = 0;
						parents(a,type,&nr,tati);
						
						for(int i = 0 ;i < nr ;i++)
						{
							TArb tag = tati[i];
							if(findClass(tag,class)){
								overrideStyle(tag,atribute,elem);
							}
						}

					}
				}
				else
				{	
					int elem = 0;
					char **atribute = attrs(style,&elem);
					char *dadtype, *childtype;
					char *tok = strtok(select," ");
					dadtype = tok;
					tok = strtok(NULL," ");
					childtype = tok;
					TArb dad = NULL,son = NULL;
					TArb *tati = calloc(500,sizeof(TArb)),*fii = calloc(500,sizeof(TArb));
					int nr1 = 0,nr2 = 0;
					parents(a,dadtype,&nr1,tati);
					//if(dad) // exista nodul tata in arbrore si cauta subarbore cu radacina tata
					//	overrideStyleTAG(dad,childtype,atribute,elem);
					int ok = 0;
					for(int i = 0 ; i < nr1 ;i++)
					{
						parents(tati[i],childtype,&nr2,fii);
						if(nr2)
							ok = 1;
						for(int j = 0 ; j < nr2;j++)
							overrideStyle(fii[j],atribute,elem);
					}
					if(!ok)
						fprintf(out,"Override style failed: no node found for selector %s %s!\n",dadtype,childtype);
				}
			}
		
		}

	}
	RSD(a,&taburi,out);//indentare
	
	// n am rezolvat cu commanda la 22 pt big si small
	// sa rezolv valgrind
	delSubtree(a);
	
	return 0;
}