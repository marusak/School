
/* c402.c: ********************************************************************}
{* Téma: Nerekurzivní implementace operací nad BVS 
**                                     Implementace: Petr Přikryl, prosinec 1994
**                                           Úpravy: Petr Přikryl, listopad 1997
**                                                     Petr Přikryl, květen 1998
**			  	                        Převod do jazyka C: Martin Tuček, srpen 2005
**                                         Úpravy: Bohuslav Křena, listopad 2009
**                                         Úpravy: Karel Masařík, říjen 2013
**                                         Úpravy: Radek Hranický, říjen 2014
**                                         Úpravy: Radek Hranický, listopad 2015
**
** S využitím dynamického přidělování paměti, implementujte NEREKURZIVNĚ
** následující operace nad binárním vyhledávacím stromem (předpona BT znamená
** Binary Tree a je u identifikátorů uvedena kvůli možné kolizi s ostatními
** příklady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivní vložení nového uzlu do stromu
**     BTPreorder ...... nerekurzivní průchod typu pre-order
**     BTInorder ....... nerekurzivní průchod typu in-order
**     BTPostorder ..... nerekurzivní průchod typu post-order
**     BTHeight ........ výpočet výšky stromu
**     BTDisposeTree ... zruš všechny uzly stromu
**
** U všech funkcí, které využívají některý z průchodů stromem, implementujte
** pomocnou funkci pro nalezení nejlevějšího uzlu v podstromu. Tyto pomocné
** funkce jsou:
**
**     Leftmost_Preorder
**     Leftmost_Inorder
**     Leftmost_Postorder
**
** Ve vaší implementaci si můžete definovat pomocné zásobníky pro uložení
** ukazetelů na uzly stromu (tStackP)
** nebo pro uložení booleovských hodnot TRUE/FALSE (tStackB).
** Pro práci s pomocnými zásobníky můžete použít následující funkce:
**
**     SInitP / SInitB ....... inicializace zásobníku
**     SPushP / SPushB ....... vložení prvku na vrchol zásobníku
**     SPopP / SPopB ......... odstranění prvku z vrcholu zásobníku
**     STopP / STopB ......... získání hodonty prvku na vrcholu zásobníku
**     STopPopP / STopPopB ... kombinace předchozích dvou funkcí 
**     SSizeP / SSizeB ....... zjištění počtu prvků v zásobníku
**     SEmptyP / SEmptyB ..... zjištění, zda je zásobník prázdný 
** 
** Pomocné funkce pro práci ze zásobníky je zakázáno upravovat!
** 
** Přesné definice typů naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na něj je typu tBTNodePtr. Jeden uzel obsahuje položku int Cont,
** která současně slouží jako užitečný obsah i jako vyhledávací klíč 
** a ukazatele na levý a pravý podstrom (LPtr a RPtr).
**
** Příklad slouží zejména k procvičení nerekurzivních zápisů algoritmů
** nad stromy. Než začnete tento příklad řešit, prostudujte si důkladně
** principy převodu rekurzivních algoritmů na nerekurzivní. Programování
** je především inženýrská disciplína, kde opětné objevování Ameriky nemá
** místo. Pokud se Vám zdá, že by něco šlo zapsat optimálněji, promyslete
** si všechny detaily Vašeho řešení. Povšimněte si typického umístění akcí
** pro různé typy průchodů. Zamyslete se nad modifikací řešených algoritmů
** například pro výpočet počtu uzlů stromu, počtu listů stromunebo pro
** vytvoření zrcadlového obrazu stromu (pouze popřehazování ukazatelů
** bez vytváření nových uzlů a rušení starých).
**
** Při průchodech stromem použijte ke zpracování uzlu funkci BTWorkOut().
** Pro zjednodušení práce máte předem připraveny zásobníky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro práci
** s pomocnými zásobníky neupravujte 
** Pozor! Je třeba správně rozlišovat, kdy použít dereferenční operátor *
** (typicky při modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem 
** (např. při vyhledávání). V tomto příkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, použijeme dereferenci.
**/

#include "c402.h"
int solved;

void BTWorkOut (tBTNodePtr Ptr)		{
/*   ---------
** Pomocná funkce, kterou budete volat při průchodech stromem pro zpracování
** uzlu určeného ukazatelem Ptr. Tuto funkci neupravujte.
**/
			
	if (Ptr==NULL) 
    printf("Chyba: Funkce BTWorkOut byla volána s NULL argumentem!\n");
  else 
    printf("Výpis hodnoty daného uzlu> %d\n",Ptr->Cont);
}
	
/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu tBTNodePtr. Tyto funkce neupravujte.
**/

void SInitP (tStackP *S)  
/*   ------
** Inicializace zásobníku.
**/
{
	S->top = 0;  
}	

void SPushP (tStackP *S, tBTNodePtr ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{ 
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
  if (S->top==MAXSTACK) 
    printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {  
		S->top++;  
		S->a[S->top]=ptr;
	}
}	

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
		return(NULL);	
	}	
	else {
		return (S->a[S->top--]);
	}	
}

tBTNodePtr STopP (tStackP *S)
/*         --------
** Vrátí hodnotu prvku na vrcholu zásobníku
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
		return(NULL);	
	}	
	else {
		return (S->a[S->top]);
	}	
}

void SPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
	}	
	else {
		S->top--;
	}	
}

int SSizeP (tStackP *S) {
/*   -------
** Vrátí počet prvků v zásobníku
**/
  return(S->top);
}

bool SEmptyP (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
  return(S->top==0);
}	

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu bool. Tyto funkce neupravujte.
*/

void SInitB (tStackB *S) {
/*   ------
** Inicializace zásobníku.
**/

	S->top = 0;  
}	

void SPushB (tStackB *S,bool val) {
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
	if (S->top==MAXSTACK) 
		printf("Chyba: Došlo k přetečení zásobníku pro boolean!\n");
	else {
		S->top++;  
		S->a[S->top]=val;
	}	
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0) {
		printf("Chyba: Došlo k podtečení zásobníku pro boolean!\n");
		return(NULL);	
	}	
	else {  
		return(S->a[S->top--]); 
	}	
}

bool STopB (tStackB *S)
/*         --------
** Vrátí hodnotu prvku na vrcholu zásobníku
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
		return(NULL);	
	}	
	else {
		return (S->a[S->top]);
	}	
}

void SPopB (tStackB *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
	}	
	else {
		S->top--;
	}	
}

int SSizeB (tStackB *S) {
/*   -------
** Vrátí počet prvků v zásobníku
**/
  return(S->top);
}

bool SEmptyB (tStackB *S) {
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Následuje jádro domácí úlohy - funkce, které máte implementovat. 
*/

void BTInit (tBTNodePtr *RootPtr)	{
/*   ------
** Provede inicializaci binárního vyhledávacího stromu.
**
** Inicializaci smí programátor volat pouze před prvním použitím binárního
** stromu, protože neuvolňuje uzly neprázdného stromu (a ani to dělat nemůže,
** protože před inicializací jsou hodnoty nedefinované, tedy libovolné).
** Ke zrušení binárního stromu slouží procedura BTDisposeTree.
**	
** Všimněte si, že zde se poprvé v hlavičce objevuje typ ukazatel na ukazatel,	
** proto je třeba při práci s RootPtr použít dereferenční operátor *.
**/
	
    *RootPtr = NULL;
	//solved = FALSE;		  /* V případě řešení smažte tento řádek! */	
}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vloží do stromu nový uzel s hodnotou Content.
**
** Z pohledu vkládání chápejte vytvářený strom jako binární vyhledávací strom,
** kde uzly s hodnotou menší než má otec leží v levém podstromu a uzly větší
** leží vpravo. Pokud vkládaný uzel již existuje, neprovádí se nic (daná hodnota
** se ve stromu může vyskytnout nejvýše jednou). Pokud se vytváří nový uzel,
** vzniká vždy jako list stromu. Funkci implementujte nerekurzivně.
**/
    while (1){
        if ( (*RootPtr) == NULL){
            *RootPtr = malloc(sizeof(struct tBTNode));
            //osetrenie mallocu
            (*RootPtr)->Cont = Content;
            (*RootPtr)->LPtr = NULL;
            (*RootPtr)->RPtr = NULL;
            return;
        }
        if ( (*RootPtr)->Cont == Content)
            return;
        if ( (*RootPtr)->Cont < Content)
            RootPtr = &(*RootPtr)->RPtr;
        else
            RootPtr = &(*RootPtr)->LPtr;
    }	
		
	//solved = FALSE;		  /* V případě řešení smažte tento řádek! */	
}

/*                                  PREORDER                                  */

void Leftmost_Preorder (tBTNodePtr ptr, tStackP *Stack)	{
/*   -----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Preorder navštívené uzly zpracujeme voláním funkce BTWorkOut()
** a ukazatele na ně is uložíme do zásobníku.
**/
    while(1){
    BTWorkOut(ptr); //print tata
    SPushP(Stack,ptr->RPtr);//uloz praveho
    if (ptr->LPtr == NULL){
        if (SEmptyP(Stack))
            return;
        ptr = STopPopP(Stack);
    }
    else
        ptr = ptr->LPtr;
    }
    //nastav laveho
    //ak lavy je NULL nastav ako laveho vrchol zasobnika
	
	
	//solved = FALSE;		  /* V případě řešení smažte tento řádek! */	
}

void BTPreorder (tBTNodePtr RootPtr)	{
/*   ----------
** Průchod stromem typu preorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Preorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/

    tStackP stack;
    SInitP(&stack);
    tBTNodePtr ptr = RootPtr;
    while(1){
        if (ptr == NULL)
            return;
        BTWorkOut(ptr); //print tata
        if (ptr->RPtr != NULL)
            SPushP(&stack,ptr->RPtr);//uloz praveho
        if (ptr->LPtr == NULL){//ak uz nic nieje nalavo
            if (SEmptyP(&stack))//a nie je prazdny zasobnik
                return;
            ptr = STopPopP(&stack);//vezmem si novy uzol zo zaosbniku
        }
        else
            ptr = ptr->LPtr;
        }
	// solved = FALSE;		  /* V případě řešení smažte tento řádek! */	
}


/*                                  INORDER                                   */ 

void Leftmost_Inorder(tBTNodePtr ptr, tStackP *Stack)		{
/*   ----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Inorder ukládáme ukazatele na všechny navštívené uzly do
** zásobníku. 
**/
	
	//asi zas nepotrebujem
	
	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */	
	
}

void BTInorder (tBTNodePtr RootPtr)	{
/*   ---------
** Průchod stromem typu inorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Inorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/
    tStackP stack;
    SInitP(&stack);
    tBTNodePtr ptr = RootPtr;
    while(1){
        if (ptr == NULL && SEmptyP(&stack))
            return;
        if (ptr == NULL){//vyberiem si zo zasobika a vytlacim to a idem doprava
            ptr = STopPopP(&stack);
            BTWorkOut(ptr);
            ptr = ptr->RPtr;
        }
        else{
            SPushP(&stack,ptr);
            ptr = ptr->LPtr;
        }
    }


    //ulozim si tatu
    //idem dolava
    //ked vlavo nic nieje vyberiem si zo zasobiku-vytlacim- jedneho a idem dolava
    
	
	
	//solved = FALSE;		  /* V případě řešení smažte tento řádek! */	
}

/*                                 POSTORDER                                  */ 

void Leftmost_Postorder (tBTNodePtr ptr, tStackP *StackP, tStackB *StackB) {
/*           --------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Postorder ukládáme ukazatele na navštívené uzly do zásobníku
** a současně do zásobníku bool hodnot ukládáme informaci, zda byl uzel
** navštíven poprvé a že se tedy ještě nemá zpracovávat. 
**/

}



	
	//asi ani toto netreba
	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */	

void BTPostorder (tBTNodePtr RootPtr)	{
/*           -----------
** Průchod stromem typu postorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Postorder, zásobníku ukazatelů a zásobníku hotdnot typu bool.
** Zpracování jednoho uzlu stromu realizujte jako volání funkce BTWorkOut(). 
**/
    //ak je null a na zasobiku je prazdno
        //return


	if (RootPtr == NULL)
        return ;
    tStackP stack;
    SInitP(&stack);
    
    SPushP(&stack,RootPtr);//ulozime vrchol
    int max = 0;
    tBTNodePtr ptr  = NULL;//aktualny ukazatel
    tBTNodePtr prev = NULL;//predchadzajuci
    while (!SEmptyP(&stack)){//kym je daco na stacku - na zaciatku je tam koren
        ptr = STopP(&stack);//len okopirujem
        if (prev == NULL || prev->LPtr == ptr || prev->RPtr == ptr){
            if (ptr->LPtr != NULL)
                SPushP(&stack,ptr->LPtr);
            else if (ptr->RPtr != NULL)
                SPushP(&stack,ptr->RPtr);
        }
        else if(ptr->LPtr == prev){
            if(ptr->RPtr != NULL)
                SPushP(&stack,ptr->RPtr);
        }
            else
                BTWorkOut(STopPopP(&stack));
        prev = ptr;
        if (SSizeP(&stack)-1 > max)
            max = SSizeP(&stack)-1;
    }



/*
    tStackP stack;
    SInitP(&stack);
    tStackB bstack;
    SInitB(&bstack);
    tBTNodePtr ptr = RootPtr;
    if (ptr == NULL)
        return;
    while(1){
        while (ptr != NULL){
            SPushP(&stack,ptr);
            SPushB(&bstack,0);
            ptr = ptr->LPtr;
        }

        if (STopB(&bstack)){//na vrchole 1
            BTWorkOut(STopPopP(&stack));
            SPopB(&bstack);
        }

        if (SEmptyP(&stack))
            return;
    
        if (!STopB(&bstack)){//na vrchole 0
            SPopB(&bstack);
            SPushB(&bstack,1);
            ptr = STopP(&stack)->RPtr;
        }

    }
*/
    
	 //solved = FALSE;		  /* V případě řešení smažte tento řádek! */	
}


int BTHeight (tBTNodePtr RootPtr) {	
/*   ----------
** Vypočítá výšku BVS bez použití rekurze
**
** Návratová hodnota je výška stromu. Funkci implementujte nerekurzivně
** bez deklarování jakékoli další pomocné funkce, která není v zadání.
** Využijte pomocných zásobníků. Je doporučeno použít jeden ze zásobníků
** pro průběžné ukládání cesty od kořene stromu. Počet uzlů na takovéto
** cestě můžete zjistit použitím funkce SSizeP. Výška stromu je rovna
** délce (počtu hran) nejdelší cesty  od kořene k lisu.
**
** Výška prázdného stromu však není definována. V případě prázdného stromu
** bude funkce vracet hodnotu -1.  
**/
	if (RootPtr == NULL)
        return -1;
    tStackP stack;
    SInitP(&stack);
    
    SPushP(&stack,RootPtr);
    int max = 0;
    tBTNodePtr ptr  = NULL;
    tBTNodePtr prev = NULL;
    while (SEmptyP(&stack) != 1){//kym je daco na stacku - na zaciatku je tam koren
        ptr = STopP(&stack);
        if (prev == NULL || prev->LPtr == ptr || prev->RPtr == ptr){
            if (ptr->LPtr != NULL)
                SPushP(&stack,ptr->LPtr);
            else if (ptr->RPtr != NULL)
                SPushP(&stack,ptr->RPtr);
        }
        else if(ptr->LPtr == prev){
            if(ptr->RPtr != NULL)
                SPushP(&stack,ptr->RPtr);
        }
        else
            SPopP(&stack);
        prev = ptr;
        if (SSizeP(&stack)-1 > max)
            max = SSizeP(&stack)-1;
    }
    return max;
    //solved = FALSE;		  /* V případě řešení smažte tento řádek! */	
}



void BTDisposeTree (tBTNodePtr *RootPtr)	{
/*   -------------
** Zruší všechny uzly stromu a korektně uvolní jimi zabranou paměť.
**
** Funkci implementujte nerekurzivně s využitím zásobníku ukazatelů.
**/
	if (*RootPtr == NULL)
        return ;
    tStackP stack;
    SInitP(&stack);
	
    while (1){
        if ((*RootPtr)->RPtr != NULL)
            SPushP(&stack,(*RootPtr)->RPtr);//ulozim pravy ukazatel
        if ((*RootPtr)->LPtr != NULL)
            SPushP(&stack,(*RootPtr)->LPtr);//ulozim lavy ukazatel
        
        free(*RootPtr);
        if (!SEmptyP(&stack)){
            (*RootPtr) = STopPopP(&stack);
        }
        else{
            *RootPtr = NULL;
            return;
        }
    }
	//solved = FALSE;		  /* V případě řešení smažte tento řádek! */	
}

/* konec c402.c */

