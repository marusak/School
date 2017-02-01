/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id:$
 */

#ifndef Student_H
#define Student_H

/******************************************************************************
 * Includes
 */

#include "render.h"
#include "fragment.h"
#include "vector_types.h"
#include "bmp.h"
#include "transform.h"
#include "coords.h"



#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Studentsky renderer, do ktereho muzete dopisovat svuj kod
 */
/* Typ/ID rendereru */
extern const int STUDENT_RENDERER;

/* Nazev textury pro nacteni (nastavovan z main.c) */
char * TEXTURE_FILENAME;

/* Jadro vaseho rendereru */
typedef struct S_StudentRenderer
{
    /* Kopie default rendereru
     * Typicky trik jak implementovat "dedicnost" znamou z C++
     * Ukazatel na strukturu lze totiz pretypovat na ukazatel
     * na prvni prvek struktury a se strukturou S_StudentRenderer
     * tak pracovat jako s S_Renderer... */
    S_Renderer  base;
    S_RGBA *texture;
    int t_w;
    int t_h;

    /* Zde uz muzete doplnovat svuj kod dle libosti */
    /* ??? */

} S_StudentRenderer;


/******************************************************************************
 * Nasledujici fce budete nejspis muset re-implementovat podle sveho
 */

/*typedef struct {
     S_Coords *v1; S_Coords *v2; S_Coords *v3;
     S_Coords *n1; S_Coords *n2; S_Coords *n3;
     S_Triangle *t;
     int x1; int y1;
     int x2; int y2;
     int x3; int y3;
     double h1; double h2; double h3;
}drawTS;
*/


/* Funkce vytvori vas renderer a nainicializuje jej */
S_Renderer * studrenCreate();

/* Funkce korektne zrusi renderer a uvolni pamet */
void studrenRelease(S_Renderer **ppRenderer);

/* Nova fce pro rasterizaci trojuhelniku s podporou texturovani
 * Upravte tak, aby se trojuhelnik kreslil s texturami
 * (doplnte i potrebne parametry funkce)
 * v1, v2, v3 - ukazatele na vrcholy trojuhelniku ve 3D pred projekci
 * n1, n2, n3 - ukazatele na normaly ve vrcholech ve 3D pred projekci
 * x1, y1, ... - vrcholy trojuhelniku po projekci do roviny obrazovky */
IZG_INLINE void studrenDrawTriangle(S_Renderer *pRenderer,
     S_Coords *v1, S_Coords *v2, S_Coords *v3,
     S_Coords *n1, S_Coords *n2, S_Coords *n3,
     S_Triangle *t,
     int x1, int y1,
     int x2, int y2,
     int x3, int y3,
     double h1, double h2, double h3
);
/* Vykresli i-ty trojuhelnik n-teho klicoveho snimku modelu
 * pomoci nove fce studrenDrawTriangle()
 * Pred vykreslenim aplikuje na vrcholy a normaly trojuhelniku
 * aktualne nastavene transformacni matice!
 * Upravte tak, aby se model vykreslil interpolovane dle parametru n
 * (cela cast n udava klicovy snimek, desetinna cast n parametr interpolace
 * mezi snimkem n a n + 1)
 * i - index trojuhelniku
 * n - index klicoveho snimku (float pro pozdejsi interpolaci mezi sn�mky) */
void studrenProjectTriangle(S_Renderer *pRenderer, S_Model *pModel, int i, double n);

/* Vraci hodnotu v aktualne nastavene texture na zadanych
* texturovacich souradnicich u, v
* Pro urceni hodnoty pouziva bilinearni interpolaci
* u, v - texturovaci souradnice v intervalu 0..1, ktery odpovida sirce/vysce textury */
IZG_INLINE S_RGBA studrenTextureValue( S_StudentRenderer * pRenderer, double u, double v );

/* Upravena funkce pro vyrenderovani cele sceny, tj. vykresleni modelu
 * Upravte tak, aby se model vykreslil animovane
 * (volani renderModel s aktualizovanym parametrem n) */
void renderStudentScene( S_Renderer *pRenderer, S_Model *pModel );

/* Callback funkce volana pri tiknuti casovace
 * ticks - pocet milisekund od inicializace */
void onTimer( int ticks );

/* Funkce pro vyrenderovani modelu slozeneho z trojuhelniku (viz main.c)
 * n - index klicoveho snimku (float pro pozdejsi interpolaci mezi snimky) */
void renderModel(S_Renderer *pRenderer, S_Model *pModel, float n);

#define oneOver255 0.003921569

#ifdef __cplusplus
}
#endif

#endif /* Student_H */

/*****************************************************************************/
/*****************************************************************************/
