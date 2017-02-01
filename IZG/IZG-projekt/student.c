/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id:$
 */

#include "student.h"
#include "transform.h"
#include "fragment.h"

#include <memory.h>
#include <math.h>

/*****************************************************************************
 * Globalni promenne a konstanty
 */

/* Typ/ID rendereru (nemenit) */
const int           STUDENT_RENDERER = 1;
double               GLOB_N = 0;
const S_Material    WHITE_AMBIENT  = { 1.0, 1.0, 1.0, 1.0 };
const S_Material    WHITE_DIFFUSE  = { 1.0, 1.0, 1.0, 1.0 };
const S_Material    WHITE_SPECULAR = { 1.0, 1.0, 1.0, 1.0 };
int fps = 0;
//*****************************************************************************
// Funkce vytvori vas renderer a nainicializuje jej
S_Renderer * studrenCreate()
{
    S_StudentRenderer * renderer = (S_StudentRenderer *)malloc(sizeof(S_StudentRenderer));
    IZG_CHECK(renderer, "Cannot allocate enough memory");

    /* inicializace default rendereru */
    renderer->base.type = STUDENT_RENDERER;
    renInit(&renderer->base);
    renderer->texture = loadBitmap(TEXTURE_FILENAME, &renderer->t_w, &renderer->t_h);
    /* nastaveni ukazatelu na upravene funkce */
    /* napr. renderer->base.releaseFunc = studrenRelease; */
    /* ??? */

    /* inicializace nove pridanych casti */
    /* ??? */
    renderer->base.releaseFunc     = studrenRelease;
    renderer->base.projectTriangleFunc = studrenProjectTriangle;
    /*
    pRenderer->createBuffersFunc   = renCreateBuffers;
    pRenderer->clearBuffersFunc    = renClearBuffers;
    pRenderer->calcReflectanceFunc = renLambertianReflectance;
*/
    return (S_Renderer *)renderer;
}

/*****************************************************************************
 * Funkce korektne zrusi renderer a uvolni pamet
 */

void studrenRelease(S_Renderer **ppRenderer)
{

    if( ppRenderer && *ppRenderer )
    {
        /* ukazatel na studentsky renderer */
        //renderer = (S_StudentRenderer *)(*ppRenderer);

        /* pripadne uvolneni pameti */
        /* ??? */
        free(((S_StudentRenderer *)ppRenderer)->texture);

        /* fce default rendereru */
        renRelease(ppRenderer);
    }
}

/******************************************************************************
 * Nova fce pro rasterizaci trojuhelniku s podporou texturovani
 * Upravte tak, aby se trojuhelnik kreslil s texturami
 * (doplnte i potrebne parametry funkce - texturovaci souradnice, ...)
 * v1, v2, v3 - ukazatele na vrcholy trojuhelniku ve 3D pred projekci
 * n1, n2, n3 - ukazatele na normaly ve vrcholech ve 3D pred projekci
 * x1, y1, ... - vrcholy trojuhelniku po projekci do roviny obrazovky
 */



IZG_INLINE void studrenDrawTriangle(S_Renderer *pRenderer,
     S_Coords *v1, S_Coords *v2, S_Coords *v3,
     S_Coords *n1, S_Coords *n2, S_Coords *n3,
     S_Triangle *t,
     int x1, int y1,
     int x2, int y2,
     int x3, int y3,
     double h1, double h2, double h3


)
{
    int         minx, miny, maxx, maxy;
    int         a1, a2, a3, b1, b2, b3, c1, c2, c3;
    int         s1, s2, s3;
    int         x, y, e1, e2, e3;
    //int         xyfb;
    double      alpha, beta, gamma, u, v, z, zz;
    S_RGBA      col1, col2, col3, color;
    S_RGBA      col11;

    /* vypocet barev ve vrcholech */
    col1 = pRenderer->calcReflectanceFunc(pRenderer,  v1,  n1);
    col2 = pRenderer->calcReflectanceFunc(pRenderer,  v2,  n2);
    col3 = pRenderer->calcReflectanceFunc(pRenderer,  v3,  n3);



    /* obalka trojuhleniku */
    minx = MIN( x1, MIN( x2,  x3));
    maxx = MAX( x1, MAX( x2,  x3));
    miny = MIN( y1, MIN( y2,  y3));
    maxy = MAX( y1, MAX( y2,  y3));

    /* oriznuti podle rozmeru okna */
    miny = MAX(miny, 0);
    maxy = MIN(maxy, pRenderer->frame_h - 1);
    minx = MAX(minx, 0);
    maxx = MIN(maxx, pRenderer->frame_w - 1);

    /* Pineduv alg. rasterizace troj.
       hranova fce je obecna rovnice primky Ax + By + C = 0
       primku prochazejici body (x1, y1) a (x2, y2) urcime jako
       (y1 - y2)x + (x2 - x1)y + x1y2 - x2y1 = 0 */

    /* normala primek - vektor kolmy k vektoru mezi dvema vrcholy, tedy (-dy, dx) */
    a1 =  y1 -  y2;
    a2 =  y2 -  y3;
    a3 =  y3 -  y1;
    b1 =  x2 -  x1;
    b2 =  x3 -  x2;
    b3 =  x1 -  x3;

    /* koeficient C */
    c1 =  x1 *  y2 -  x2 *  y1;
    c2 =  x2 *  y3 -  x3 *  y2;
    c3 =  x3 *  y1 -  x1 *  y3;

    /* vypocet hranove fce (vzdalenost od primky) pro protejsi body */
    s1 = a1 *  x3 + b1 *  y3 + c1;
    s2 = a2 *  x1 + b2 *  y1 + c2;
    s3 = a3 *  x2 + b3 *  y2 + c3;

    if ( !s1 || !s2 || !s3 )
    {
        return;
    }

    /* normalizace, aby vzdalenost od primky byla kladna uvnitr trojuhelniku */
    if( s1 < 0 )
    {
        a1 = -a1;
        b1 = -b1;
        c1 = -c1;
    }
    if( s2 < 0 )
    {
        a2 = -a2;
        b2 = -b2;
        c2 = -c2;
    }
    if( s3 < 0 )
    {
        a3 = -a3;
        b3 = -b3;
        c3 = -c3;
    }

    /* koeficienty pro barycentricke souradnice */
    alpha = 1.0 / ABS(s2);
    beta = 1.0 / ABS(s3);
    gamma = 1.0 / ABS(s1);

    double menh;
    double hx1 =  t->t[0].x/ h1;
    double hx2 =  t->t[1].x/ h2;
    double hx3 =  t->t[2].x/ h3;
    double hy1 =  t->t[0].y/ h1;
    double hy2 =  t->t[1].y/ h2;
    double hy3 =  t->t[2].y/ h3;
    double d1h1 = 1.0/ h1;
    double d1h2 = 1.0/ h2;
    double d1h3 = 1.0/ h3;
    int ze1 = a1 * minx + c1  +  b1 * (miny -1);
    int ze2 = a2 * minx + c2  + b2 * (miny -1);
    int ze3 = a3 * minx + c3  + b3 * (miny -1);
    int wasIn;
    int frame_w = pRenderer->frame_w;
    //xyfb = miny * frame_w;
    //double *db = &(pRenderer->depth_buffer[0]);
    double *db = &(pRenderer->depth_buffer[miny * frame_w]);
    //S_RGBA *fb = &(pRenderer->frame_buffer[0]);
    S_RGBA *fb = &(pRenderer->frame_buffer[miny * frame_w]);
    double a2alpha = a2*alpha;
    double a3beta = a3*beta;
    double a1gamma = a1*gamma;
    double alphab2 = alpha * b2 ;
    double betab3 = beta * b3 ;
    double gammab1 = gamma * b1;
    double w1, w2, w3;
    double zw1 = ze2 *alpha;
    double zw2 = ze3 * beta;
    double zw3 = ze1 * gamma;
    double zDelta = a2alpha*v1->z + a3beta * v2->z + a1gamma * v3->z;
/*
    double uDelta = a2alpha * hx1 + a3beta * hx2 + a1gamma * hx3;
    double vDelta = a2alpha * hy1 + a3beta * hy2 + a1gamma * hy3;
    double menhDelta = a2alpha * d1h1 + a3beta * d1h2 + a1gamma * d1h3;
    double uIn, vIn;
    double redDelta = a2alpha * col1.red + a3beta * col2.red + a1gamma * col3.red;
    double greenDelta = a2alpha * col1.green + a3beta * col2.green + a1gamma * col3.green;
    double blueDelta = a2alpha * col1.blue + a3beta * col2.blue + a1gamma * col3.blue;
    double re1, gr1, bl1;
*/
    for( y = miny; y <= maxy; ++y )
    {
        // inicilizace hranove fce v bode (minx, y)
        e1 = (ze1 += b1);
        e2 = (ze2 += b2);
        e3 = (ze3 += b3);
        w1 = (zw1 += alphab2);
        w2 = (zw2 += betab3);
        w3 = (zw3 += gammab1);

        wasIn=0;
        for( x = minx; x <= maxx; ++x )
        {
            if( (e1 | e2 | e3) >= 0 )
            {
                if (!wasIn){//vstupujeme do trojuholnika
                  w1 = e2 * alpha;
                  w2 = e3 * beta;
                  w3 = e1 * gamma;
                  z = w1 * v1->z + w2 *  v2->z + w3 *  v3->z;
                  wasIn = 1;
                }
                else {//boli sme uz v trojuholniku
                  w1 += a2alpha;
                  w2 += a3beta;
                  w3 += a1gamma;
                  z += zDelta;
                }


                zz = *(db + x);
                if (z >= zz){
                //  wasIn = 1;
                  goto B1;
                }

/*
                if (!wasIn){
                  uIn = w1*hx1 + w2*hx2 + w3*hx3;
                  vIn = w1*hy1 + w2*hy2 + w3*hy3;
                  menh = w1*d1h1 + w2*d1h2 + w3*d1h3;
                  //re1 = w1 * col1.red + w2 * col2.red + w3 * col3.red;
                  //gr1 = w1 * col1.green + w2 * col2.green + w3 * col3.green;
                  //bl1 = w1 * col1.blue + w2 * col2.blue + w3 * col3.blue;
                  wasIn = 1;
                }
                else{
                  uIn += uDelta;
                  vIn += vDelta;
                  menh += menhDelta;
                  //re1 += redDelta;
                  //gr1 += greenDelta;
                  //bl1 += blueDelta;
                }
                u = uIn/menh;
                v = vIn/menh;
*/
                menh = w1*d1h1 + w2*d1h2 + w3*d1h3;
                u = (w1*hx1 + w2*hx2 + w3*hx3)/menh;//NAROCNE
                v = (w1*hy1 + w2*hy2 + w3*hy3)/menh;//NAROCNE

                color.red = ROUND2BYTE(w1 * col1.red + w2 * col2.red + w3 * col3.red);
                color.green = ROUND2BYTE(w1 * col1.green + w2 * col2.green + w3 * col3.green);
                color.blue = ROUND2BYTE(w1 * col1.blue + w2 * col2.blue + w3 * col3.blue);
                color.alpha = 255;

                col11 = studrenTextureValue((S_StudentRenderer *) pRenderer, u, v);
                /**(fb + x) = (S_RGBA){re1*col11.red*oneOver255,
                                 gr1*col11.green*oneOver255,
                                 bl1*col11.blue*oneOver255,
                                 255
                };
                */
                color = (S_RGBA){color.red*col11.red*oneOver255,
                                color.green*col11.green*oneOver255,
                                color.blue*col11.blue*oneOver255,
                                255
                          };
                    *(fb +x) = color;


                    *(db + x) = z;
            }
            else if (wasIn)
              break;
            B1:
            // hranova fce o pixel vedle
            e1 += a1;
            e2 += a2;
            e3 += a3;
        }
      db += frame_w;
      fb += frame_w;

    }
}

IZG_INLINE S_Coords interpoate(S_Coords *v1, S_Coords *v2, double n){
    S_Coords new;
    new.x = (*v1).x + n*((*v2).x-(*v1).x);
    new.y = (*v1).y + n*((*v2).y-(*v1).y);
    new.z = (*v1).z + n*((*v2).z-(*v1).z);
    return new;
}


/******************************************************************************
 * Vykresli i-ty trojuhelnik n-teho klicoveho snimku modelu
 * pomoci nove fce studrenDrawTriangle()
 * Pred vykreslenim aplikuje na vrcholy a normaly trojuhelniku
 * aktualne nastavene transformacni matice!
 * Upravte tak, aby se model vykreslil interpolovane dle parametru n
 * (cela cast n udava klicovy snimek, desetinna cast n parametr interpolace
 * mezi snimkem n a n + 1)
 * i - index trojuhelniku
 * n - index klicoveho snimku (float pro pozdejsi interpolaci mezi snimky)
 */

void studrenProjectTriangle(S_Renderer *pRenderer, S_Model *pModel, int i, double n)
{
    // TODO len skopirovane z renProjectTriangle()

    S_Coords    aa, bb, cc;             /* souradnice vrcholu po transformaci */
    S_Coords    naa, nbb, ncc;          /* normaly ve vrcholech po transformaci */
    S_Coords    nn;                     /* normala trojuhelniku po transformaci */
    int         u1, v1, u2, v2, u3, v3; /* souradnice vrcholu po projekci do roviny obrazovky */
    S_Triangle  * triangle;
    int         vertexOffset, normalOffset; /* offset pro vrcholy a normalove vektory trojuhelniku */
    int         vertexOffset1, normalOffset1; /* offset pro vrcholy a normalove vektory trojuhelniku */
    int         i0, i1, i2, in;             /* indexy vrcholu a normaly pro i-ty trojuhelnik n-teho snimku */
    int         i01, i11, i21, in1;             /* indexy vrcholu a normaly pro i-ty trojuhelnik n-teho snimku */

    n = GLOB_N;


    /* z modelu si vytahneme i-ty trojuhelnik */
    triangle = trivecGetPtr(pModel->triangles, i);

    /* ziskame offset pro vrcholy n-teho snimku */
    vertexOffset = (((int) n) % pModel->frames) * pModel->verticesPerFrame;

    /* ziskame offset pro normaly trojuhelniku n-teho snimku */
    normalOffset = (((int) n) % pModel->frames) * pModel->triangles->size;

    /* ziskame offset pro vrcholy n-teho snimku */
    vertexOffset1 = (((int) n+1) % pModel->frames) * pModel->verticesPerFrame;

    /* ziskame offset pro normaly trojuhelniku n-teho snimku */
    normalOffset1 = (((int) n+1) % pModel->frames) * pModel->triangles->size;

    n = fmod(n, 1);

    /* indexy vrcholu pro i-ty trojuhelnik n-teho snimku - pricteni offsetu */
    i0 = triangle->v[ 0 ] + vertexOffset;
    i1 = triangle->v[ 1 ] + vertexOffset;
    i2 = triangle->v[ 2 ] + vertexOffset;
    /* index normaloveho vektoru pro i-ty trojuhelnik n-teho snimku - pricteni offsetu */
    in = triangle->n + normalOffset;

    /* indexy vrcholu pro i-ty trojuhelnik n-teho snimku - pricteni offsetu */
    i01 = triangle->v[ 0 ] + vertexOffset1;
    i11 = triangle->v[ 1 ] + vertexOffset1;
    i21 = triangle->v[ 2 ] + vertexOffset1;
    /* index normaloveho vektoru pro i-ty trojuhelnik n-teho snimku - pricteni offsetu */
    in1 = triangle->n + normalOffset1;

    S_Coords *vv1 = cvecGetPtr(pModel->vertices, i0);
    S_Coords *vv2 = cvecGetPtr(pModel->vertices, i1);
    S_Coords *vv3 = cvecGetPtr(pModel->vertices, i2);

    S_Coords *vv01 = cvecGetPtr(pModel->vertices, i01);
    S_Coords *vv11 = cvecGetPtr(pModel->vertices, i11);
    S_Coords *vv21 = cvecGetPtr(pModel->vertices, i21);

    S_Coords ne1 = interpoate(vv1, vv01, n);
    S_Coords ne2 = interpoate(vv2, vv11, n);
    S_Coords ne3 = interpoate(vv3, vv21, n);

/*
    i0 = i0 + fmod(n,1)*(i01-i0);
    i1 = i1 + fmod(n,1)*(i11-i1);
    i2 = i2 + fmod(n,1)*(i21-i2);
    in = in + fmod(n,1)*(in1-in);
    */
    /* transformace vrcholu matici model */
  /*  trTransformVertex(&aa, cvecGetPtr(pModel->vertices, i0));
    trTransformVertex(&bb, cvecGetPtr(pModel->vertices, i1));
    trTransformVertex(&cc, cvecGetPtr(pModel->vertices, i2));
*/
    trTransformVertex(&aa, &ne1);
    trTransformVertex(&bb, &ne2);
    trTransformVertex(&cc, &ne3);

    /* promitneme vrcholy trojuhelniku na obrazovku */
    double h1 = trProjectVertex(&u1, &v1, &aa);
    double h2 = trProjectVertex(&u2, &v2, &bb);
    double h3 = trProjectVertex(&u3, &v3, &cc);

    vv1 = cvecGetPtr(pModel->normals, i0);
    vv2 = cvecGetPtr(pModel->normals, i1);
    vv3 = cvecGetPtr(pModel->normals, i2);

    vv01 = cvecGetPtr(pModel->normals, i01);
    vv11 = cvecGetPtr(pModel->normals, i11);
    vv21 = cvecGetPtr(pModel->normals, i21);

    ne1 = interpoate(vv1, vv01, n);
    ne2 = interpoate(vv2, vv11, n);
    ne3 = interpoate(vv3, vv21, n);



    /* pro osvetlovaci model transformujeme take normaly ve vrcholech */
    trTransformVector(&naa, &ne1);
    trTransformVector(&nbb, &ne2);
    trTransformVector(&ncc, &ne3);

    /* normalizace normal */
    coordsNormalize(&naa);
    coordsNormalize(&nbb);
    coordsNormalize(&ncc);


    vv1 = cvecGetPtr(pModel->trinormals, in);
    vv01 = cvecGetPtr(pModel->trinormals, in1);
    ne1 = interpoate(vv1, vv01, n);


    /* transformace normaly trojuhelniku matici model */
    trTransformVector(&nn, &ne1);

    /* normalizace normaly */
    coordsNormalize(&nn);

    /* je troj. privraceny ke kamere, tudiz viditelny? */
    if( !renCalcVisibility(pRenderer, &aa, &nn) )
    {
        /* odvracene troj. vubec nekreslime */
        return;
    }
    /* rasterizace trojuhelniku */
    studrenDrawTriangle(pRenderer,
                    &aa, &bb, &cc,
                    &naa, &nbb, &ncc,
                    triangle,
                    u1, v1, u2, v2, u3, v3,
                    h1, h2, h3
    );

}

/******************************************************************************
* Vraci hodnotu v aktualne nastavene texture na zadanych
* texturovacich souradnicich u, v
* Pro urceni hodnoty pouziva bilinearni interpolaci
* Pro otestovani vraci ve vychozim stavu barevnou sachovnici dle uv souradnic
* u, v - texturovaci souradnice v intervalu 0..1, ktery odpovida sirce/vysce textury
*/


IZG_INLINE S_RGBA studrenTextureValue( S_StudentRenderer * ren, double u, double v )
{
    u = u * ren->t_w -0.5;
    v = v * ren->t_w -0.5;


    int x = u;
    int y = v;

    double ur = (u-x);
    double vr = (v-y);
    double uo = (1.0-u+x);
    double vo = (1.0-v+y);
    int t_w = ren->t_w;
    S_RGBA *first = &ren->texture[x * t_w + y];

    S_RGBA e11 = *first;
    S_RGBA e12 = *(first + 1);
    S_RGBA e21 = *(first + t_w);
    S_RGBA e22 = *(first + t_w + 1);

    int uovo = uo * vo *256;
    int urvo = ur * vo *256;
    int uovr = uo * vr *256;
    int urvr = ur * vr *256;

    return (S_RGBA){
    (e11.red * uovo + e21.red * urvo + e12.red *uovr + e22.red * urvr)>>8,
    (e11.green * uovo + e21.green * urvo + e12.green *uovr + e22.green * urvr)>>8,
    (e11.blue * uovo + e21.blue * urvo + e12.blue *uovr + e22.blue * urvr)>>8,
    255
  };
}

/******************************************************************************
 ******************************************************************************
 * Funkce pro vyrenderovani sceny, tj. vykresleni modelu
 * Upravte tak, aby se model vykreslil animovane
 * (volani renderModel s aktualizovanym parametrem n)
 */

void renderStudentScene(S_Renderer *pRenderer, S_Model *pModel)
{
    fps++;
    // TODO len skopirovane z renderDefualtScene()
    /* test existence frame bufferu a modelu */

    /* nastavit projekcni matici */
    trProjectionPerspective(pRenderer->camera_dist, pRenderer->frame_w, pRenderer->frame_h);

    /* vycistit model matici */
    trLoadIdentity();

    /* nejprve nastavime posuv cele sceny od/ke kamere */
    trTranslate(0.0, 0.0, pRenderer->scene_move_z);

    /* nejprve nastavime posuv cele sceny v rovine XY */
    trTranslate(pRenderer->scene_move_x, pRenderer->scene_move_y, 0.0);

    /* natoceni cele sceny - jen ve dvou smerech - mys je jen 2D... :( */
    trRotateX(pRenderer->scene_rot_x);
    trRotateY(pRenderer->scene_rot_y);



    /* nastavime material */
    renMatAmbient(pRenderer, &WHITE_AMBIENT);
    renMatDiffuse(pRenderer, &WHITE_DIFFUSE);
    renMatSpecular(pRenderer, &WHITE_SPECULAR);

    /* a vykreslime nas model (ve vychozim stavu kreslime pouze snimek 0) */
    renderModel(pRenderer, pModel, GLOB_N);


}

/* Callback funkce volana pri tiknuti casovace
 * ticks - pocet milisekund od inicializace */
int called = 0;
 void onTimer( int ticks )
{
      called++;
      if (called %30 == 0){
        printf("%d\n",fps);
        fps = 0;
      }
      GLOB_N = ticks / 100.0;
}



/*****************************************************************************
 *****************************************************************************/
