/*
 * Soubor:  proj1.c
 * Datum:   2014/11/17
 * Autor:   Marušák Matej, xmarus06@stud.fit.vutbr.cz
 * Projekt: Iteračné výpočty, projekt č. 2 pro předmět IZP
 * Popis:   Program počíta vzdialenosti a výšky meraného objektu pomocou údajou zo senzoru meriacieho prístroja.\n
*/

#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define PI 3.1415926535
#define HALF_PI PI/2


double cfrac_tan(double x, unsigned int n);
double taylor_tan(double x, unsigned int n);
void arg_tan(double a, int n, int m);
double absol(double c);
int pars(int argc, char *argv[]); 
int errs(int e);
double is_number(char nb[], int stat);
void help(void);
double CHNG_D(double num);
void arg_m(double x, double a, double b);

enum ERRORS {E_OK, ISNT_NUM, INVALID_PARS , INVALID_RANGE, E_DIFF};//chybove kody

int main(int argc, char *argv[])
{
int return_value = 0;
return_value = pars(argc, argv);
if (return_value != E_OK) {
  errs(return_value);}
return return_value;
}


/* Vykonava argument -m.
 * x je vyska pristroja
 * a je uhol alfa
 * b je uhol beta
 */ 

void arg_m(double x, double a, double b)
{
double ranges[9] = {0, 0.0006694328, 0.0214097778, 0.1065581683, 0.2759163835, 0.5204290620,
0.8147188303, 1.1219610091, 1.3872676574};//horne zavory pre pocty iteracii (index pola je pocet iteracii)/odvovodnenei v dokumentacii

int range = 1;
while (x > ranges[range]){
  range++;}//zistuje kolko iteracii treba vypocitat
double distance = 0.0;
distance = x / cfrac_tan(a,range);
printf("%.10e\n",distance);

if(b != 0){
  range = 1;
  while (x > ranges[range]){
    range++;}
  double height = 0.0;
  height = x + ( distance * cfrac_tan(b, range));
  printf("%.10e\n",height);
  }
}


/*meni cislo num aby bolo v rozsahu medzi -HALF_PI az HALF_PI*/

double CHNG_D(double num)
{
if (num < -HALF_PI) {
  while (num < -HALF_PI){
    num = num + PI;}
return num;
}
else {
  while (num > HALF_PI){
    num = num - PI;}
return num;
}
}


/*Vypise pomoc a skonci*/

void help(void){
printf(
  "----------------------------------------------------------------------------------------------------"
  "\nProgram Iteracne vypocty.\n"
  "Autor: Marušák Matej (c) 2014\n\n"
  "Program počíta vzdialenosti a výšky meraného objektu pomocou údajou zo senzoru meriacieho prístroja.\n"
  "Ovládanie: \n"
  "         proj2 --help\n"
  "         proj2 [-c X] -m A [B]\n"
  "         proj2 --tan\n\n"
  "Argumenty:\n\n"
  "--help          - Vypíše túto pomoc a skončí.\n"
  "[-c X] -m A [B] - Počíta vzdialenosť:\n"
  "                - [] je nepovinny udaj.\n"
  "                - A = uhol v radiánoch od senzora po pätu objektu. 0 < A <= 1.4\n"
  "                - B = uhol v radiánoch od senyora po vrchol obektu. 0 < B <= 1.4\n"
  "                - X nastavuje výšku meracieho prístroja. Implicitná výška = 1.5 matra. 0 < X <= 100\n"
  "--tan A N M     - zrovnáva presnosť tangensu uhla A medzi volaním tan z mat. knižnice,\n"
  "                  výpočtom pomocou Taylorovho polynómu a zreťazených zlomkov.\n"
  "                - A = uhol v radiánoch. \n"
  "                - N a M udávajú v ktorých iteráciách výpočtu má porovnávanie prebiehať.\n"
  "                - VÝPIS: \n"
  "                        - I M T TE C CE\n"
  "                        - I iteracia\n"
  "                        - M volanie mat. knižnice\n"
  "                        - T volanie Taylorovho polynómu\n"
  "                        - TE absolútna chyba medzi M a T\n"
  "                        - C volanie zreťazených zlomkov\n"
  "                        - CE absolútna chyba medzi M a C\n\n"
  "---------------------------------------------------------------------------------------------------\n"
);
}



/*Funkcia spracuva hodnoty prikazovej riadky.
 * argc    - pocet argumentov
 * *argv[] - argumenty
 * osetruje neocakavane stavy, presuva vykonavanie na funkciu ktora vypliva z argumentov.
 */

int pars(int argc, char *argv[])
{
if (argc < 2){//menej ako dva parameter
  return INVALID_PARS;}

else if (strcmp(argv[1],"--help") == 0 && argc == 2) {//dva paramatere a druhy je --help
  help();
  return E_OK;} 

else if (strcmp(argv[1],"--tan") == 0 && argc == 5){//5 parametrov, druhy je --tan (./proj2 --tan A N M)
  double a = is_number(argv[2],1);
      if (a == FP_NAN) {return ISNT_NUM;}
  if (a < -HALF_PI || a > HALF_PI){//upravuje uhol od -HALF_PI po HALF_PI
    a = CHNG_D(a);}
  int n = is_number(argv[3],2);
      if (n == FP_NAN) {return INVALID_PARS;}
  int m = is_number(argv[4],2);
      if (m == FP_NAN) {return INVALID_PARS;}
  if (n < 1 || m > 13 || n > m){//osetruje vstup aby platilo 0 <= N <= M <= 13
    return INVALID_PARS;} 
  arg_tan(a,n,m);
  return E_OK;}

else if (strcmp(argv[1],"-m") == 0){//druhy argument je -m
  if (argc == 3 ) {//pocet argumentov je 3 (./proj2 -m A)
    double a = is_number(argv[2],1);
      if (a == FP_NAN) {return ISNT_NUM;}
    if (a <= 0 || a > 1.4) { //osteruje aby bol rozsah medzi 0 < A <= 1.4
      return INVALID_RANGE;}
    arg_m(1.5,a,0);
    return E_OK;}
  else if (argc == 4) {// pocet argumentov je 4 (./proj2 -m A B)
    double a = is_number(argv[2],1);
      if (a == FP_NAN) {return ISNT_NUM;}
    double b = is_number(argv[3],1);
      if (b == FP_NAN) {return ISNT_NUM;}
    if (a <= 0 || a > 1.4 || b <= 0 || b > 1.4 ) { //osteruje aby bol rozsah medzi 0 < A,B <= 1.4
      return INVALID_RANGE;}
    arg_m(1.5,a,b);
    return E_OK;}
  else {
        return INVALID_PARS;}
  }

else if (argc == 5 && strcmp(argv[1],"-c") == 0 && strcmp(argv[3],"-m") == 0 ){//pocet argumentov je 5,  druhy argument je -c, 4ty -m (./proj2 -c X -m A)
  double x = is_number(argv[2], 1);
      if (x == FP_NAN) {return INVALID_PARS;}
  double a = is_number(argv[4],1);
      if (a == FP_NAN) {return ISNT_NUM;}
  if (a <= 0 || a > 1.4 || x <= 0 || x > 100) {//osetruje aby bol rozsah 0 < A <= 1.4 a 0 < X <= 100
        return INVALID_RANGE;}
  arg_m(x,a,0);
  return E_OK;}

else if (argc == 6 && strcmp(argv[1],"-c") == 0 && strcmp(argv[3],"-m") == 0 ){//pocet argumentov je 6, druhy je -c, 4ty -m (./proj2 -c X -m A B)
  double x = is_number(argv[2], 1);
      if (x == FP_NAN) {return INVALID_PARS;}
  double a = is_number(argv[4],1);
      if (a == FP_NAN) {return ISNT_NUM;}
  double b = is_number(argv[5],1);
      if (b == FP_NAN) {return ISNT_NUM;}
  if (a <= 0 || a > 1.4 || b <= 0 || b > 1.4 || x <= 0 || x > 100) {//osteruje aby bol rozsah medzi 0 < A,B <= 1.4 a 0 < X <= 100
      return INVALID_RANGE;}
  arg_m(x,a,b);
  return E_OK;}

else {return(INVALID_PARS);}//ine argumenty
return E_DIFF;
}


/* Funkcia na vypis chyb.
 * Funkcia vypise chybu na zaklade e.
 * Po vypise chyby, program konci.
 */

int errs(int e)
{
switch(e){
  case ISNT_NUM: 
    fprintf(stderr, "Uhol nie je cislo.\n");
   return ISNT_NUM;
 
  case INVALID_PARS: 
    fprintf(stderr, "Nespravne argumenty na vstupe.\n");
 return INVALID_PARS;

  case INVALID_RANGE:
    fprintf(stderr, "Argumenty niesu v rozsahu. Vid --help.\n");
 return INVALID_RANGE;

 case E_DIFF:
 fprintf(stderr, "Neznama chyba.\n");
  return E_DIFF;

 default:
    ;
}
return E_OK;
}


/*Funkcia zistuje ci je zadany retazec cislo.
 * ma dva prepinace(pomocou stat)
 * -stat = 1 : cislo sa konvertuje na cislo typu double, ak obsahuje nenumericke znaky;
 *             vypisuje chybu.
 * -stt = 2 : cislo sa konvertuje na typ integer. Ak obsahuje nenumericke znaky, konci chybou.
 */           

double is_number(char nb[],int stat)
{
switch (stat){
  case 1://double
    {
    double j;
    char *a;
    j = strtod(nb,&a);
    if (*a != '\0'){
    j = FP_NAN;}
    return j;
    }
  break;
  
  case 2://int
    {
    int j;
    char c1[100] = "";
    j = atoi(nb);
    sprintf(c1, "%d",j); //riesi pripad ze zacina cislicou a pokracuje pismenom napr. 2aB1
    if (strlen(c1) != strlen(nb)){
      j = FP_NAN;}
    return j;
    }
    break;
  
  default:
    ;

}
return E_OK;
} 


/*Funkcia na vykonanie prepinaca --tan.
 * a je uhol medzi -HALF_PI az HALF_PI
 * n je zaciatok od kial sa budu vypisovat pocty iteracii
 * m je koniec po kade sa budu vypisovat pocty iteracii
 */

void arg_tan(double a, int n, int m)
{
double M = tan(a);
double T = 0;
double C = 0;
for (; n<=m; n++){
  T = taylor_tan(a,n);
  C = cfrac_tan(a,n); 
  printf("%d %e %e %e %e %e\n",n,M,T,absol(T-M),C,absol(C-M));
  }
}


/*Funkcia na vypocet absolutnej hodnoty.
 * c je cislo, z ktoreho ma byt urcena absolutna hodnota.
 */ 

double absol(double c)
{
return c<0 ? -c : c;
}


/* Implementacia vypoctu tangensu pomocou zretazelych zlomkov.
 * Funkcia nie je obmedzena na pocet zlomkov ktore vie spocitat
 * x je uhol medzi -HALF_PI az HALF_PI
 * n je pocet zlomkov ktore maju byt vypocitane
 */
 
double cfrac_tan(double x, unsigned int n)
{
double men = 0; 
double cit = x*x;
double pom = 2*n+1;
for(; n>1; n--){
pom -= 2;
men = cit / ((pom)-men);
}

return x / (1- men);
}


/*Implementacia vypoctu tangens pomocou Taylorovho polynomu. 
 * k_cit su indexi v citateloch zlomkov.
 * k_men su indexu v menovateloch zlomkov.
 * Funkcia je odmedzena na vypocet maximalne v 13tych cykloch.
 * x je uhol medzi -HALF_PI az HALF_PI
 * n je pocet zlomkov, ktore budu pouzite
 */

double taylor_tan(double x, unsigned int n)
{
long unsigned int k_cit[13] = {1, 1, 2, 17, 62, 1382, 21844, 929569, 6404582, 443861162, 18888466084, 113927491862, 58870668456604};
long unsigned int k_men[13] = {1, 3, 15, 315, 2835, 155925, 6081075, 638512875, 10854718875, 1856156927625, 194896477400625, 49308808782358125, 3698160658676859375};
long  double vys = x;
int pom = 3;
long double x_men = x*x*x; 
for(unsigned int i = 1; i < n; i++){
    vys = vys + (k_cit[i]*x_men / k_men[i]);
    x_men *= x*x;
    pom += 2;
  }
return vys;
}
