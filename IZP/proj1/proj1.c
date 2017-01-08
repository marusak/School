/*
 * Soubor:  proj1.c
 * Datum:   2014/11/10
 * Autor:   Marušák Matej, xmarus06@stud.fit.vutbr.cz
 * Projekt: Výpočty v tabulce, projekt č. 1 pro předmět IZP
 * Popis:   Program je jednoduchý tabulkový kalkulátor. Program umožnuje vyhladať maximum alebo minimum, umožnuje vypočítať
 *          súčet a aritmetický priemer vybraných buniek.
*/


#include <stdio.h>//standarny vstup a vystup
#include <string.h>//funkcia strcmp - porovnanie dvoch retazcov
#include<stdlib.h>//exit funkcia - ukoncenie celeho programu pri neocakavanom stave

#define ERR_ZLY_PAR 1//chybny parmeter prikazoveho riadku
#define ERR_BUNKA_MIMO_ROZSAH 2 // cislo bunky sa nevyskytuje na riadku
#define ERR_NIE_C 3 //argument nie je cislo
#define ERR_ZLE_ZAKONCENIE 4 //riadok tabulky nieje ukonceny znakom konca riadku
#define ERR_NIE_C_BUN 5 //bunka nieje cislo
#define DLHI_RIADOK 6 //riadok je dlhsi ako 1024 znakov

#define MAX_RIADKY 2000000//maximum riadkov, ktore vie program spracovat

void chyba(int e);//vypis chyb a ukoncenie programu
int najdi_zac_bunky(int bunka, int x, int j,char *i);//vracia polohu prveho znaku bunky, ktoru hlada
void help(void);//vypis --help
int je_cislo(char c[],int stat1);//zistuje ci je retazec cislo
void prog(int r1, int rn, int s1, int sn, int stat);//vykonava argumenty
void vrat_bunku(int bun, int ak, int *k, int *d, int *spc, char *VB, char *i);//funkcia vracia konkretnu bunku
void parametry(int *r1, int *rn, int *s1, int *sn, int *stat, int argc, char *argv[]);//kontrola vstupnych parametrov a ich spracovanie



int main(int argc, char *argv[])
{
int stat = 0;//uklada vybratu operaciu
int r1,rn,s1,sn;//ukladaju vyber buniek

parametry(&r1,&rn,&s1,&sn,&stat,argc,argv);
prog (r1,rn,s1,sn,stat);

return 0;
}


/*funkcia spracuje vstupne parametre, nastavi "stat" pre vybrate operacie a "r1,rn,s1,sn"
 * pre vybrate bunky. V pripade volania help, rovno odkaze na funkciu ktora takto robi.
 * Funkcia zaroven osetruje necakane stavy a s prislusinim chybovim kodom odkazuje na 
 * funkciu, ktora tlaci chyby a ukoncuje program.
 */
void parametry(int *r1, int *rn, int *s1, int *sn, int *stat, int argc, char *argv[])
{
if (argc == 2  && strcmp(argv[1], "--help") == 0){
  help();}
else if (argc <= 2){
  chyba(ERR_ZLY_PAR);}

else if (strcmp(argv[2], "row") == 0){
  if (argc != 4){
    chyba(ERR_ZLY_PAR);}
  else {
    *r1 = *rn = je_cislo(argv[3],1);
    if(*r1 < 1){
      chyba(ERR_ZLY_PAR);}
    *s1 = 1;
    *sn = 1024;}
}

else if (strcmp(argv[2], "col") == 0){
  if (argc !=4){
       chyba(ERR_ZLY_PAR);}
  else {
    *s1 = *sn = je_cislo(argv[3],1);
    if (*sn < 1){
      chyba(ERR_ZLY_PAR);}
    *r1 = 1;
    *rn = MAX_RIADKY;}
}

else if (strcmp(argv[2], "rows") == 0){
  if (argc !=5){
    chyba(ERR_ZLY_PAR);}
  else {
    *r1 = je_cislo(argv[3],1);
    *rn = je_cislo(argv[4],1);
    if (*r1 < 1 || *rn < *r1){
      chyba(ERR_ZLY_PAR);}
    *s1= 1;
    *sn = 1024;}
}

else if (strcmp(argv[2], "cols") == 0){
  if (argc !=5){
    chyba(ERR_ZLY_PAR);}
  else {
    *s1 = je_cislo(argv[3],1);
    *sn = je_cislo(argv[4],1);
    if (*s1 < 1 || *sn < *s1){
      chyba(ERR_ZLY_PAR);}
    *r1 = 1;
    *rn = MAX_RIADKY;}
}

else if (strcmp(argv[2], "range") == 0){
  if (argc !=7){
    chyba(ERR_ZLY_PAR);}
  else {
    *r1 = je_cislo(argv[3],1);
    *rn = je_cislo(argv[4],1);
    *s1 = je_cislo(argv[5],1);
    *sn = je_cislo(argv[6],1);
    if (*r1 < 1 || *rn < *r1 || *s1 < 1 || *sn < *s1){
      chyba(ERR_ZLY_PAR);}
  }
}

else chyba(ERR_ZLY_PAR);

if (strcmp(argv[1], "select") == 0) *stat = 1;
else if (strcmp(argv[1], "min") == 0) *stat = 2;
else if (strcmp(argv[1], "max") == 0) *stat = 3;
else if (strcmp(argv[1], "sum") == 0) *stat = 4;
else if (strcmp(argv[1], "avg") == 0) *stat = 5;
else chyba(ERR_ZLY_PAR);
}


/*funkcia vracia polohu prveho znaku hladanej bunky. Vstupne udaje: 
 * bunka: cislo hladanej bunky
 * x:poloha na ktorej zacina prechadzanie retazca
 * j: bunka na ktorej aktualne sme
 * *i:retazec s celym riadkom v ktorom hlada polohu bunky 
 */
int najdi_zac_bunky(int bunka, int x, int j, char *i)
{
if (i[x] == '\n' || i[x] == '\0'){
  chyba(ERR_BUNKA_MIMO_ROZSAH);} //ak je subor prazdny
if (x == 0 && i[0] == ' '){//pripad ze riadok zacina medzerami
  while(i[x] == ' '){
    x++;}
  }  
while (bunka != j) {
  if (i[x] == '\r' || i[x] == '\n' ){
    chyba(ERR_BUNKA_MIMO_ROZSAH);}
  if (i[x] == ' '){
    while (i[x] == ' '){
      x++;
      if (i[x] == '\r' || i[x] == '\n'){
        chyba(ERR_ZLE_ZAKONCENIE);}
      if (i[x] != ' '){
        j++;
        }
    }
    x--;
  } /*reisi pripad ze riadok konci medzerami s nedostatocnym poctom buniek*/
x++;
}
return x;
}

/*Funkcia na zakladne argumentu e, vypise chybu na stderr.
 *Nasledne ukonci cely program s chybovym kodom odpovedajucim chybe.
 */
void chyba(int e)
{
switch(e)
  {
    case 1: fprintf(stderr,"nespravne udaje na vstupe\n");
    exit(1);
    break;

    case 2: fprintf(stderr,"pocet buniek nieje dostacujuci\n");
    exit(2);
    break;

    case 3 : fprintf(stderr,"argument nie je cislo\n");
    exit(3);
    break;

    case 4 : fprintf(stderr,"riadok tabulky musi byt ukonceny znakom konca riadku\n");
    exit(4);
    break;

    case 5 : fprintf(stderr,"Bunka nie je cislo\n");
    exit(5);
    break;

    case 6 : fprintf(stderr,"Riadok je dlhsi ako 1024 znakov\n");
    exit(6);
    break;

    default:
        break;
  }
}

/*Funkcia vypise hlavicku a ovladanie programu. Funkcia sa vola z prikazovej riadky prikazom --help.
 * Funkcia ukonci cely program po vypisani.
 */
 
void help()
{
printf(
  "------------------------------------------------------------------------------------------------------------------------------------------------"
  "\nProgram Výpočty v tabulce.\n"
  "Autor: Marušák Matej (c) 2014\n\n"
  "Program je jednoduchý tabulkový kalkulátor.\nProgram umožnuje vyhladať maximum alebo minimum, umožnuje vypočítať "
  "súčet a aritmetický priemer vybraných buniek.\n\n"
  "Ovládanie: \n "
  "         proj1 --help\n "
  "         proj1 operace vyber_buniek\n\n"
  "Argumenty:\n\n "
  "Operace:\n\n"
  "  select - značí operaci, která z dané tabulky pouze vybere a následně vytiskne hodnoty daných buněk\n"
  "  min    - značí vyhledání a následný tisk minimální hodnoty z daného rozsahu buněk\n"
  "  max    - značí vyhledání a následný tisk maximální hodnoty z daného rozsahu buněk\n"
  "  sum    - značí výpočet a následný tisk sumy hodnot všech vybraných buněk\n"
  "  avg    - značí výpočet a následný tisk aritmetického průměru vybraných buněk\n\n"

  "Vyber_buniek:\n\n"
  "  row X         - značí výběr všech buněk na řádku X (X > 0)\n"
  "  col X         - značí výběr všech buněk ve sloupci X (X > 0)\n"
  "  rows X Y      - značí výběr všech buněk od řádku X (včetně) až po Y (včetně). 0 < X <= Y\n"
  "  cols X Y      - značí výběr všech buněk od sloupce X (včetně) až po Y (včetně). 0 < X <= Y\n"
  "  range A B X Y - značí výběr buněk od řádku A po řádek B a od sloupce X po sloupec Y (včetně daných řádků a sloupců). 0 < A <= B, 0 < X <= Y\n\n"
  "---------------------------------------------------------------------------------------------------------------------------------------------------\n"
);
exit(0);
}

/*Funkcia zistuje ci je argument alebo bunka cislo.
 * Ma 3 rozne varianty - podla parametra stat1:
 *   -1- zistuje ci je parameter prikazovej riadky cislo - ak nie je cislo, skonci program s chybou.
 *   -2- zistuje ci je bunka cislo - ak nie je cislo, skonci s chybou. (pre min, max, avg, sum)
 *   -3- zsituje ci je bunka cislo - ak nie je, vracia 0. (pre select)
 * char c[] je retazec, z ktoreho zituje ci je cislo.
 */

int je_cislo(char c[], int stat1)
{
switch(stat1){
  case 1://musi byt cislo a parameter
    {
    int j;
    char c1[1023] = "";
    j = atoi(c);
    sprintf(c1, "%d",j); //riesi pripad ze zacina cislicou a pokracuje pismenom napr. 2aB1
    if (strlen(c1) != strlen(c)){
      chyba(ERR_NIE_C);}
    return j;
    }
  case 2://musi byt cislo, ale nie parameter
    {
    double j;
    char *a;
    j = strtod(c,&a);
    if (*a != '\0'){
      chyba(ERR_NIE_C_BUN);}
    return j;
    }
  case 3://iba zistuje ci je cislo, ak nieje vracia 0
    {
    double j;
    char *a;
    j = strtod(c,&a);
    if (*a != '\0'){
      j = 0;}
    return j;
    }
  default:
    ;
  } 
return 0;
}

/*Funkcia vykonava argumenty.
 * Prechadza kazdy riadok, ak nie je v rozsahu <a,b>, preskoci riadok.
 * Ak je riadok v danom rozsahu, pyta si bunky v rozsahu <c,d> cez funkciu vrat_bunku.
 * Za pomoci argumenta stat sa rozhoduje, co s bunkou vykona.  
 */

void prog(int a, int b, int c, int d, int stat)
{
int r = 1;//aktualny riadok
int spc = 0;//skutocny pocet buniek - pocet buniek na riadku
double vys = 0;//vysledok pre funkcie max, min, avg, sum
int poc = 0;//pocitadlo pre funkciu avg
char i[1024];//jeden riadok

while (fgets(i,1025, stdin) !='\0'){
  if (strlen(i) == 1024 && i[1023] != '\n'){
    chyba(DLHI_RIADOK);}
  if (r >= a && r <= b){
    int  bun = c;//bunka ktoru idem hladat
    int ak = 1; //aktualne bunka
    int k = 0; //pocitadlo
    char VB[1024];
    while (d >= bun){
      vrat_bunku(bun, ak, &k, &d, &spc, &VB[0],&i[0]);
      switch(stat){
        case 1:vys = je_cislo(VB,3);
	       if (vys != 0){
                 printf("%.10g\n",vys);}
               else {
		 printf("%s\n",VB);}
               break;
       case 2: if (r == a && bun == c){
                 vys = je_cislo(VB,2);}
               if (vys > je_cislo(VB,2)){
                 vys = je_cislo(VB,2);}
               break;
       case 3: if (r == a && bun == c){
            	   vys = je_cislo(VB,2);}
               if (vys < je_cislo(VB,2)){
		 vys = je_cislo(VB,2);}
               break;
       case 4: vys = vys + je_cislo(VB,2);
               break;
       case 5: vys = vys + je_cislo(VB,2);
               poc++;
               break;
       default:
              break;
    }

   ak = bun;
   bun++;
 }
   r++;
 }
else r++;
 }
if (r <= b && b != MAX_RIADKY) chyba(ERR_BUNKA_MIMO_ROZSAH);//ak je nedostatok riadkov
switch(stat){
  case 2:
  case 3:
  case 4: printf("%.10g\n",vys);
          break;
  case 5: printf("%.10g\n",vys/poc);
          break;
  default:
          break;
  }
}
/*Po prebehnuti funkcie, je vo VB ulozena bunka dana parametrami:
 *  -bun je bunka ktoru chceme ulozit do VB
 *  -ak je aktualna bunka na ktorej sa nachadzame
 *  -k je poloha kde sa nachadzame v retazci
 *  -d je horne ohranicenie poctu stlpcov na jednom riadku
 *  -spc je skutocny pocet buniek na riadku.
 *  -i je cely riadok
 */

void vrat_bunku(int bun, int ak, int *k, int *d, int *spc, char *VB, char *i)
{
*k = najdi_zac_bunky(bun,*k,ak,&i[0]);
int ZK = *k;//uklada polohu prveho znaku v bunke
while (i[*k] != '\r' && i[*k] != ' ' && i[*k] != '\n'){
  ++*k;}
strncpy(VB, i+ZK, *k - ZK);
VB[*k - ZK] = '\0';
if (i[*k] == '\r' || i[*k] == '\n'){
  if (*d == 1024){
    *d = bun;}//pocet stlpcov
  *spc = bun;//skutocny pocet buniek
  }
if (*spc == bun && i[*k] == ' '){
  chyba(ERR_ZLE_ZAKONCENIE);}//koniec riadku s medzerou
}
