/*
 * Soubor:  proj3.c
 * Datum:   2014/12/6
 * Autor:   Marušák Matej, xmarus06@stud.fit.vutbr.cz
 * Projekt: Prechod bludiskom, projekt č. 3 pro předmět IZP
 * Popis:   Program ...\n
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

typedef struct {int x; int y;} Change;


typedef struct {
  int rows;
  int cols;
  unsigned char *cells;
} Map;

int errs(int e);
void help(void);
int test(FILE *fp, Map *map);
bool isborder(Map *map, int r, int c, int border);
int start_border(Map *map, int x, int y, int leftright);
int open_file(FILE **fp, char *file);
int go_through(Map *map, int x, int y, int leftright);
int solve_maze(Map *map, int x, int y, int rightleft, FILE *file, char *name);

enum ERRORS {E_OK, NEF, INVALID, E_MEM, E_DIFF, WA, ISP};//chybove kody
enum WAYS {LEFT = 1, RIGHT, UPDOWN = 4};//smery


int main(int argc, char *argv[])
{

Map map;
int return_value = 0;
FILE *fp = NULL;

if (argc < 2) return_value = WA;
else if (argc == 2 && strcmp(argv[1],"--help") == 0){
  help();
  return_value = E_OK;}
else if (argc == 3 && strcmp(argv[1],"--test") == 0){

 return_value = open_file(&fp, argv[2]);
 if (return_value != NEF){
    return_value = test(fp, &map);
    fclose(fp);
    free(map.cells);
    if (return_value == INVALID){
      printf("Invalid\n");
      return_value = E_OK;}
    else printf("Valid\n");}
  }
else if (argc == 5){
  if (strcmp(argv[1],"--rpath") == 0){
    int x = atoi(argv[2]);
    int y = atoi(argv[3]);
    if ( y > 0 && x > 0) return_value = solve_maze(&map, x, y, RIGHT, fp, argv[4]);  
      else return_value = WA; 
    }
  else if (strcmp(argv[1],"--lpath") == 0){
    int x = atoi(argv[2]);
    int y = atoi(argv[3]);
    if ( y > 0 && x > 0) return_value = solve_maze(&map, x, y, LEFT, fp, argv[4]);
      else return_value = WA; 
    }
else return_value = WA;
} 
else return_value = WA;

if (return_value != E_OK) {
  errs(return_value);}

return return_value;
}

/* FUnkcia, ktorá sa stará o vyriešenie bludiska.
 * *map je ukazatel na struktúru mapy.
 * x a y sú indexy zacinajúceho políčka
 * rightleft je smer ktorou rukou sa riadime
 * *file je ukazatel na súbor
 * name je názov súbora
 * Funkcia sa stará o otvorenie, skontrolovanie súbora, jeho zavretie a uvoľnenie pameti
 */


int solve_maze(Map *map, int x, int y, int rightleft, FILE *file, char *name)
{
int return_value;
if (open_file(&file, name) == NEF) return NEF;
return_value = test(file, map);
if (return_value == E_OK) return_value = go_through(map, x, y, rightleft);
fclose(file);
free(map->cells);
return return_value;
}


/* Funkcia vypíše pomoc a skončí*/

void help(void)
{
printf(
  "----------------------------------------------------------------------------------------------------"
  "\nProgram Prechod bludiskom.\n"
  "Autor: Marušák Matej (c) 2014\n\n"
  "Program hľadá cestu cez bludisko.\n"
  "Ovládanie: \n"
  "         proj3 --help\n"
  "         proj3 --test subor.txt\n"
  "         proj3 --lapth R C bludisko.txt\n"
  "         proj3 --rpath R C bludisko.txt\n\n"
  "Argumenty:\n\n"
  "--help                   - Vypíše túto pomoc a skončí.\n"
  "--test subor.txt         - Kontroluje, či druhý operand je validná mapa.\n"
  "                         - Je validná ak popisuje všetky bunky a zdieľané hranice sú rovnaké.\n"
  "                         - Ak je validná, vypise sa Valid ak nie je Invalid. Program konci.\n"
  "--rpath R C bludisko.txt - hľadá priechod bludiskom zo vstupu na riadku R a stĺpci C.\n"
  "                           Priechod hľadá pomocou pravidla pravej ruky.\n"
  "                         - Výstupom sú všetky bunky, ktoré boli navštívene (v poradí)\n"
  "--lpath R C bludisko.txt - rovnako ako rpath, len za pomoci ľavej ruky.\n"
  "                    \n"
  "---------------------------------------------------------------------------------------------------\n"
);
}


/* Funkcia prechádyajúca bludiskom.
 * *map je ukazatal na strukturu mapy. x a y sú vstupné síradnice.
 * leftright je informacia, ktorou rukou sa riadni pri výbere
 */

int go_through(Map *map, int x, int y, int leftright)
{

Change d[] = {//zmeny súradníc
{-1,0},//UP
{0,-1},//LEFT
{0,1},//RIGHT
{1,0},//DOWN
};

int h_dir = -1;//pomocná premmenná smeru
int act_x = x;
int act_y = y;
int act_dir = start_border(map, x, y, leftright);
if (act_dir == ISP) return ISP;
while(act_x > 0 && act_x <= map->rows && act_y > 0 && act_y <= map->cols){
printf("%d,%d\n",act_x, act_y);

if (!isborder(map, act_x, act_y, RIGHT) && !isborder(map, act_x, act_y, LEFT) && !isborder(map, act_x, act_y, UPDOWN)){
  if ( (leftright == LEFT && ((act_x^act_y) & 1)) || (leftright == RIGHT && (!( (act_x^act_y) & 1))) ){
    if (act_dir == LEFT) act_dir = UPDOWN;//druhy pripad
    else if (act_dir == UPDOWN) act_dir = LEFT;}
  else {
    if (act_dir == RIGHT) act_dir = UPDOWN;
    else if (act_dir == UPDOWN) act_dir = RIGHT;}
  }
else if (!isborder(map, act_x, act_y, RIGHT) && isborder(map, act_x, act_y, LEFT) && !isborder(map, act_x, act_y, UPDOWN))
 if (act_dir == LEFT) act_dir = UPDOWN;
   else act_dir = RIGHT;
else if (isborder(map, act_x, act_y, RIGHT) && !isborder(map, act_x, act_y, LEFT) && !isborder(map, act_x, act_y, UPDOWN))
 if (act_dir == RIGHT) act_dir = UPDOWN;
   else act_dir = LEFT; 
else if (!isborder(map, act_x, act_y, RIGHT) && isborder(map, act_x, act_y, LEFT) && isborder(map, act_x, act_y, UPDOWN))
 act_dir = RIGHT;
else if (isborder(map, act_x, act_y, RIGHT) && !isborder(map, act_x, act_y, LEFT) && isborder(map, act_x, act_y, UPDOWN))
act_dir = LEFT;

if (act_dir == UPDOWN){
  if ((act_x ^ act_y) & 1) /*hore*/ h_dir = 3;
  else  h_dir = 0;
  }
else h_dir = act_dir;
act_x = act_x + d[h_dir].x;
act_y = act_y + d[h_dir].y;
}
return E_OK;
}


/*Funkcia otvorí súbor daný argumentom a skontroluje ci sa správne otvoril*/

int open_file(FILE **fp, char *file)
{
*fp = fopen(file, "r");
if (*fp == NULL)
  return NEF;
else return E_OK;
}


/* funkcia vracia polohu z ktorej sa vchadza na bunku.
 * map je ukazatel na strukturu mapy
 * x a y je vstupna bunka
 * funkcia rozhoduje na zaklade smeru z ktoreho ideme , ci je tam otvorena hranica.
 * Ak nie je vracia, ISP (invladit starting point)
 */

int start_border(Map *map, int x, int y, int leftright){

(void) leftright;

if ( x == map->rows || x == 1 || y == 1 || y == map->cols ){
//musi byt prvy alebo psoeldy riadok aj stlpec

if (x ==1){ //prvy riadok
      if (y == 1) {//prvy stlpec
         if(!(isborder(map, x, y, LEFT))) return RIGHT; 
           else if(!(isborder(map, x, y, UPDOWN))) return UPDOWN;
           else return ISP;
           }
       else if (y == map->cols){
         if (map->cols & 1){//neparne
           if(!(isborder(map, x, y, UPDOWN))) return UPDOWN; 
           else if(!(isborder(map, x, y, RIGHT))) return LEFT;
           else return ISP;}
         else if(!(isborder(map, x, y, RIGHT))) return LEFT;
        else return ISP; 
        }               
      else if ((y & 1) && !isborder(map, x, y, UPDOWN))  return UPDOWN;
      else return ISP;
    }

else if (x == map->rows)//posledny riadok
  {
  if (map->rows & 1){
    if (y == 1){
      if (!(isborder(map, x,y, LEFT))) return RIGHT;
        else return ISP;
      }
    else if (y == map->cols){
      if (!(map->cols & 1)){
        if (!(isborder(map, x,y, RIGHT))) return LEFT;
        else if(!(isborder(map, x,y, UPDOWN))) return UPDOWN;
        else return ISP;
        }
      else if (!(isborder(map, x,y, RIGHT))) return LEFT;
      }
    else if (y & 1) return ISP;
    else if (!(isborder ( map, x, y, UPDOWN))) return UPDOWN;
    else return ISP;} 
  else{
    if (y == 1){
      if (!(isborder(map, x,y, LEFT))) return RIGHT;
        else if(!(isborder(map, x,y, UPDOWN))) return UPDOWN;
        else return ISP;
      }
    else if (y == map->cols){
      if (map->cols & 1){
        if (!(isborder(map, x,y, RIGHT))) return LEFT;
        else if(!(isborder(map, x,y, UPDOWN))) return UPDOWN;
        else return ISP;
        }
      else if (!(isborder(map, x,y, RIGHT))) return LEFT;
      }
    else if (!(y & 1)) return ISP;
    else if (!(isborder(map, x, y, UPDOWN))) return UPDOWN; 
    else return ISP;} 
  }
else if (x < map->rows) { 
  if (y == 1 && !(isborder(map, x,y, LEFT))) return RIGHT;
  else if (y > 1 && y<= map->cols && !(isborder(map, x, y, RIGHT))) return LEFT; 
  else return ISP;} 
else return ISP;
}
else return ISP;
return E_DIFF;
}




/* Vracia 0 ak je hranica prechodna, inak 0.
 * Bunka je zadana pomocou:
 *   *map je ukazatel na struktuu mapy
 *   r je riadok
 *   c je stlpec
 *   border je hranica ktoru uvazujeme
 */

bool isborder(Map *map, int r, int c, int border)
{
return (map->cells[map->cols * (r - 1) + c - 1] & border);
}


/*Funkcia test
 * na vstupe ma ukazatej na subor a na strukturu
 * najskor precita 2 znaky, ak su to cisla tka ich ulozi do rows a cols v map. do count ulozi ich sucin
 * allokuje sa pamet pre count usigned charov
 * nacita sa count hhu, zaroven sa kontrouju ci su cisla a su od 0 do 7
 * kontorluje sa ci susedne hrany na riadku su v poriadku. prva bunak potrebuje mat naprvo, k nej nalavo, 
 *   druha opat napravo a k nej nalavo az po n-1
 *kontroluju sa hrany cez stlpce, ide sa od druheho riadku po predposledny, ak oba indexy su parne, potrebujeme
 *   potrebujeme kontorlovat s bunkou nad nou, inak s bunkou pod nou
 */


int test(FILE *fp, Map *map)
{
if (fscanf(fp, "%d %d", &map->rows, &map->cols) == 2){
  int count = map->rows * map->cols;
  map->cells = (unsigned char *) malloc(count * sizeof (unsigned char));
  if (map->cells == NULL) return E_MEM;
  for (int i = 0; i < count; i++){
  unsigned char ah = 0;
    if (fscanf(fp, "%hhu", &ah) == 1){
      if (ah > 7) {return INVALID;}
        else {map->cells[i] = ah;}
      }
    else return INVALID;}
   }
else return INVALID;
for (int i = 1; i <= map->rows; i++){
  for (int j = 1; j < map->cols; j++){
     if (isborder(map, i, j, RIGHT) != isborder(map, i, j+1, LEFT)) return INVALID;//prva musi mat napravo, vedla nej nalavo
  }
}
for (int i = 2; i < map->rows; i++){
  for (int j = 1; j <= map->cols; j++){
    if ( (i&1)^(j&1) ) { //ak maju roznu paritu tak maju dole spolocnu hranu
      if (isborder(map, i, j, UPDOWN) != isborder(map, i+1, j , UPDOWN)) return INVALID;}
    else if (isborder(map, i, j, UPDOWN) != isborder(map, i-1, j , UPDOWN)) return INVALID;
  }
}
return E_OK;
}


/*Funkcia vytlačí chybu podľa chybového kódu*/

int errs(int e)
{
switch(e){
 
 case NEF: 
   fprintf(stderr, "Not existing file.\n");
 return NEF;

 case INVALID:
   fprintf(stderr, "Invalid\n");
 return INVALID;
 
 case E_MEM:
   fprintf(stderr, "Alloc memory error");
  return E_MEM;

 case WA:
  fprintf(stderr, "Wrong arguments.\n");
  return WA;

 case ISP:
   fprintf(stderr, "Invalid starting point.\n");
  return ISP;

case E_DIFF:
 fprintf(stderr, "Neznama chyba.\n");
  return E_DIFF;

 default:
    ;
}
return E_DIFF;
}
