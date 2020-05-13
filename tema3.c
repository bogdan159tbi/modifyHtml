#include "lib.h"
#define MAX_ATTR 1000

char **attrs(char *value,int *elem)
{	
	*elem = 0;
	char **atribute = calloc(MAX_ATTR,sizeof(char*));
	if(!atribute)
		return NULL;
	char *p = strtok(value,";");
	while(p)
	{
		atribute[*elem] = calloc(1024,sizeof(char));
		strcpy(atribute[*elem],p);
		(*elem)++;
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
 char *p;
 p = strtok(eticheta,":");
 strcpy(*name,p);
 p = strtok(NULL,":");
 strcpy(*val,p);
 free(eticheta);
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
int doarSpatii(char *content)
{
	for(int i = 0; i < strlen(content); i++)
		if(content[i] != ' ')
			return 0;
	return 1;
}
//afisare cu indentare
void RSD(TArb root,int *taburi)
{
	if(!root)
		return;
	else
	{	for(int i = 0; i < *taburi; i++)
			printf("\t");
		if(root->info)
		printf("<%s",root->info->type);

		if(root->info->style){
			printf(" style=\"");
		TAttr st;
		if(root->info->style->next){
			for( st = root->info->style; st->next != NULL; st = st->next)
				printf("%s: %s; ",st->name,st->value);
			//printf(" ");
			printf("%s: %s;\"",st->name,st->value);//la ultimul atribute nu exista spatiu intre ; si "  de la sfarsit
					}
		else
			printf("%s: %s;\"",root->info->style->name,root->info->style->value);
		}
		
		if(root->info->otherAttributes)
		{	printf(" ");
			TAttr attr;
			if(root->info->isSelfClosing == 0){
				for(attr = root->info->otherAttributes; attr->next != NULL; attr = attr->next)
					printf("%s=\"%s\"",attr->name,attr->value);
				printf("%s=\"%s\"",attr->name,attr->value);
					}
			else
			{
				for(attr = root->info->otherAttributes; attr->next != NULL; attr = attr->next)
					printf("%s=\"%s\" ",attr->name,attr->value);
				printf("%s=\"%s\"",attr->name,attr->value);
			}
		}
		if(root->info->isSelfClosing == 0){
			printf(">\n");
		//am afisat atributele la stil si celelalte atribute 
		//acum urmeaza continut daca e dif de null
			if(root->info->contents && strlen(root->info->contents) > 0){
				for(int i = 0; i < (*taburi); i++)
					printf("\t");
				printf("\t");
				printf("%s\n",root->info->contents);
			}
		}
		(*taburi)++;
		for(TArb p = root->firstChild; p != NULL ;p = p->nextSibling)
			RSD(p,taburi);
		(*taburi)--;
		if(root->info->isSelfClosing == 0){
			for(int i = 0; i < *taburi; i++)
				printf("\t");
			printf("</%s>\n",root->info->type);
		}
		else printf("/>\n");

	}
}
//cautarea se face pe nivele,adica parcurg in latime
int cautaID(TArb root,char *id,TArb *frate)
{
	if(!root)
		return 0;
	if(root->info->id){
	if(!strcmp(root->info->id,id)){
		//printf("%s\n",root->info->id);
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

int cautaTag(TArb root,char *type,TArb *tata)
{
	if(!root)
		return 0;
	if(root->info->type){
	if(!strcmp(root->info->type,type)){
		//printf("%s\n",root->info->id);
		*tata = root;
		return 1;
	}
	}
	for(TArb p = root->firstChild; p != NULL ;p = p->nextSibling)
		cautaTag(p,type,tata);
	return 0;
}


void addTag(TArb a,char *cmd)
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
		return 0;//nu exista ,tre sa dau si fprintf
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
	else return 0;//nu exista ,tre sa dau si fprintf
}
void deleteID(TArb dad,char *id)
{
	if(id[strlen(id)- 1] == '1' ) //iau firstChild
	{	
		TArb child;
		child = dad->firstChild;
		dad->firstChild = child->nextSibling;		
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
		//free(kids);	
	}

}
int main(int argc, char **argv)
{
	FILE *in = fopen("simple.html","r");
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
					//printf("%s=%s\n",tmp,value);
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
				//printf("value= %s\n",value);
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
	FILE *comenzi = fopen("/home/bdi/SD/hw3HTML/_test/commands/commands-simple-8.in","r");
	int linii;
	if(!comenzi)
		return 1;
	fscanf(comenzi,"%d",&linii);
	for(int i = 1; i <= linii+1 ;i++){
		char *cmd = calloc(200,sizeof(char)); // lungimea unei comenzi citite din fisier
		fgets(cmd,200,comenzi);

		if(strlen(cmd) > 1){
			cmd[strlen(cmd)-1] = '\0';
		if(strstr(cmd,"add")){
			addTag(a,cmd);//nu stiu daca se garanteaza corectitudinea comenzilor	
			bfsID(a);
		}
		else if(strstr(cmd,"deleteRecursively"))
		{	
			char *selector = cmd + strlen("deleteRecurively") + 2 + strlen("selector=\"");//ca sa obtin selector direct
			selector[strlen(selector) - 1] = '\0'; // ca sa elimin " de la sfarsit
			int rez = 0;
			if(selector[0] == '.'){// cautam class la atribute
				rez = delClass(a,selector+1);
				if(rez)
				bfsID(a); 
			}
			else if(!strchr(selector,'>') && !strchr(selector,' ') && !strchr(selector,'.') && !strchr(selector,'#')) {// ca sa iau tag-ul
				rez = delTag(a,selector);
				//if(rez)
				//	bfsID(a); 
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
				cautaTag(a,dadtype,&dad);
				if(dad)
				{
					TArb kids,ant = NULL;
					for(kids = dad->firstChild ; kids != NULL ; kids = kids->nextSibling){
						if(!strcmp(kids->info->type,childtype))
						{
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
				}
				bfsID(dad);
			}			
			else if(strchr(selector+1,' '))
			{
				char *dadtype, *childtype;
				char *tok = strtok(selector," ");
				dadtype = tok;
				tok = strtok(NULL," ");
				childtype = tok;
				TArb dad = NULL,son = NULL;
				cautaTag(a,dadtype,&dad);
				if(dad) // exista nodul tata in arbroe si cauta subarbore cu radacina tata
				{	
					cautaTag(dad,childtype,&son);
					if(son)//daca am gasit tre sa l sterg si sa refac legaturile
					{
						char *idson = malloc(10);
						strcpy(idson,son->info->id);
						char *iddad = malloc(10);
						strcpy(iddad,idson);
						iddad[strlen(iddad) -2 ] = '\0';
						TArb parent = NULL;
						cautaID(dad,iddad,&parent);
						deleteID(parent,idson);
						bfsID(parent);
					}
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
				if(dad)
					deleteID(dad,id);
				bfsID(dad);
			}
			
		}
		else if(strstr(cmd,"appendStyle"))
			{
			//	printf("append\n");
			}
		else if(strstr(cmd,"overrideStyle"))
			{
				//printf("override\n");
		
			}
		}

	}
	RSD(a,&taburi);//indentare
	//cautaClass(a,"class1",&pla);

	return 0;
}