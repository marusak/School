#include "bmp.h"
#include <stdlib.h>//malloc
#include <stdio.h>//printf
#include <math.h>//round
#include <string.h>//memset
//x x x
//x O x
//x x x


#define s1  0.02040816327
#define s3  0.0612244898
#define s9  0.1836734694

//zaostroi obraz lineranym filtrom H
//okraje si rozsiruje o riaodk a stlpec 0
//vstupny obraz je v prvom parametry
//vystupny je v druhom
void focus(unsigned char *bitmapData, unsigned char *resultMap, BITMAPINFOHEADER *infoHeader){
    double H[][3]={ {-0.5, -0.5, -0.5},
                   {-0.5,  5.0, -0.5},
                   {-0.5, -0.5, -0.5}
                 };
    for (int i = 0; i < infoHeader->biHeight; i++){
        for (int j = 0; j <infoHeader->biWidth; j++){
            int k = 0;
            int kn = 3;
            int y = -1;
            if ( i == 0){//al sa robi prvy riadok
                y = 0;//zacneme na riadku 1+0;
                k = 1;//zacneme v matiici na strednom riadku
            }
            if ( i == infoHeader->biHeight -1){
                kn = 2;//skoncime porovnanie o riadok skor
            }
            double newVal = 0;
            for (; k < kn; k++){
                int x = -1;
                int l = 0;
                int ln = 3;
                if (j == 0){//prvy stlpec
                    x = 0;
                    l = 1;
                }
                if ( j == infoHeader->biWidth -1)//posledny stlpec
                    ln = 2;
                for (; l < ln; l++){
                    newVal += H[k][l]*bitmapData[(i+y)*infoHeader->biWidth+j+x];
                    x++;
                }
                y++;
            }
            
            resultMap[i*infoHeader->biWidth+j] = round(newVal)<0?0:round(newVal)>255?255:(unsigned
                    char)round(newVal);
        }
    }
}

//obrati obraz podla vertikalnej osi
//vstupny obraz je v prvom parametry
//vystupny je v druhom
void flip_vertical(unsigned char *bitmapData, unsigned char *resultMap, BITMAPINFOHEADER *infoHeader){
    for (int i = 0; i < infoHeader->biHeight; i++){
        for (int j = 0; j <infoHeader->biWidth; j++){
            resultMap[infoHeader->biHeight*i + infoHeader->biWidth-j-1]  = \
                bitmapData[i*infoHeader->biHeight+j];
        }
    }
}

//pomcona funkcia na zoradenie pola prvkov - funkcia je spolocna s projektom z IFj a IAL
void shellSort(unsigned char arr[], int size)
{
	int gap = size / 2;

	while(gap > 0)
	{
		for(int i = 0; i < gap; i++)	//opakuj tolikrat, kolik je listu
		{
			// printf("\nList %d\n", i+1);
			// serad kazdy list
			for(int j = i + gap; j < size; j = j + gap)
			{
				unsigned char value = arr[j];
				int k = j - gap;
				while(k >= 0 && value < arr[k])
				{
					// printf("%d je mensi nez %d\n", value, arr[k]);
					arr[k + gap] = arr[k];
					arr[k] = value;
					k = k - gap;
				}
			}
		}
		gap = gap / 2;	//zmensime mezeru
	}
}

void median_filter(unsigned char *bitmapData, unsigned char *resultMap, BITMAPINFOHEADER *infoHeader){
    unsigned char ftr[25];
    for (int i = 0; i < infoHeader->biHeight; i++){
        for (int j = 0; j <infoHeader->biHeight; j++){
            memset(ftr,0,25*sizeof(unsigned char));
            int counter = 0;
            int x  = -2;
            int xn = 3;
            if ( i == 0 || i == 1){
                x = -1*i;
            }
            if ( i == infoHeader->biHeight-1 || i == infoHeader->biHeight -2){
                xn = infoHeader->biHeight - i ;
            }

            for(;x<xn;x++){
                int y  = -2;
                int yn =  3;
                if (j == 0 || j == 1){
                    y = -1*j;
                }
                if ( j == infoHeader->biWidth -1 || j == infoHeader->biWidth -2){
                    yn = infoHeader->biWidth - j;
                }
                for (;y<yn;y++){
                    ftr[counter] = bitmapData[(i+x)*infoHeader->biHeight+j+y];
                    counter++;
                }
            }
            shellSort(ftr,25);
            resultMap[i*infoHeader->biWidth+j] = ftr[12];
        }
    }
}


void blur(unsigned char *bitmapData, unsigned char *resultMap, BITMAPINFOHEADER *infoHeader){
    double filter[][5] ={ {s1,s1,s1,s1,s1},
                         {s1,s3,s3,s3,s1},
                         {s1,s3,s9,s3,s1},
                         {s1,s3,s3,s3,s1},
                         {s1,s1,s1,s1,s1},
    };
    for (int i = 0; i < infoHeader->biHeight; i++){
        for (int j = 0; j <infoHeader->biHeight; j++){
            double res = 0;
            int x  = -2;
            int xn = 3;
            if ( i == 0 || i == 1){
                x = -1*i;
            }
            if ( i == infoHeader->biHeight-1 || i == infoHeader->biHeight -2){
                xn = infoHeader->biHeight - i ;
            }

            for(;x<xn;x++){
                int y  = -2;
                int yn =  3;
                if (j == 0 || j == 1){
                    y = -1*j;
                }
                if ( j == infoHeader->biWidth -1 || j == infoHeader->biWidth -2){
                    yn = infoHeader->biWidth - j;
                }
                for (;y<yn;y++){
                    res += filter[x+2][y+2]*bitmapData[(i+x)*infoHeader->biHeight+j+y];
                }
            }
            resultMap[i*infoHeader->biWidth+j] = round(res)<0?0:round(res)>255?255:(unsigned
                    char)round(res);
        }
    }
}
    
                             
void error_value(unsigned char *bitmapData, unsigned char *resultMap, BITMAPINFOHEADER *infoHeader){
    double res = 0;
    for (int i = 0; i < infoHeader->biHeight; i++){
        for (int j = 0; j <infoHeader->biHeight; j++){
            double h =
                bitmapData[i*infoHeader->biHeight+j]-resultMap[i*infoHeader->biHeight+j];
            res += h<0?-h:h;
        }
    }
    double m = 512*512;
    printf("chyba=%.5g\n",res/m);
}


void extend_his(unsigned char *bitmapData, unsigned char *resultMap, BITMAPINFOHEADER *infoHeader){
    unsigned char max_v = 0;
    unsigned char min_v = 255;
    for (int i = 0; i < infoHeader->biHeight; i++){
        for (int j = 0; j <infoHeader->biHeight; j++){
            if (bitmapData[i*infoHeader->biHeight+j] > max_v)
                max_v = bitmapData[i*infoHeader->biHeight+j];
            if (bitmapData[i*infoHeader->biHeight+j] < min_v)
                min_v = bitmapData[i*infoHeader->biHeight+j];
        }
    }
    for (int i = 0; i < infoHeader->biHeight; i++){
        for (int j = 0; j <infoHeader->biHeight; j++){
            resultMap[i*infoHeader->biHeight+j] =
                round((bitmapData[i*infoHeader->biHeight+j]-min_v)*((255.0)/(255-(255-max_v)-min_v)));
        }
    }
}


double mean(unsigned char *resultMap, BITMAPINFOHEADER *infoHeader){
    int res = 0;
    for (int i = 0; i < infoHeader->biHeight; i++)
        for (int j = 0; j <infoHeader->biHeight; j++)
            res += resultMap[i*infoHeader->biHeight+j];
    return (double)res/(512*512);
}


double std(unsigned char *resultMap, BITMAPINFOHEADER *infoHeader){
    double x_ = mean(resultMap,infoHeader);
    double res = 0.0;
    double in = 0.0;
    for (int i = 0; i < infoHeader->biHeight; i++)
        for (int j = 0; j <infoHeader->biHeight; j++){
            in = resultMap[i*infoHeader->biHeight+j]-x_;
            in = in * in;
            res += in;
        }
    res = res / (512*512);
    return sqrt(res);
}

    
unsigned char log_shift(unsigned char x){
    unsigned char n = x <<2;
    unsigned char m1 = 0b00001100;
    unsigned char m2 = 0b00110000;
    unsigned char m3 = 0b11000000;

    x = x | ( m1 & n);
    n = n <<2;
    x = x | ( m2 & n);
    n = n <<2;
    x = x | ( m3 & n);

    return x;
}



void quant(unsigned char *bitmapData, unsigned char *resultMap, BITMAPINFOHEADER *infoHeader){
    for (int i = 0; i < infoHeader->biHeight; i++){
        for (int j = 0; j <infoHeader->biWidth; j++){
            unsigned char val = bitmapData[i*infoHeader->biHeight+j];
            val = round((val/255.0)*3.0);
            resultMap[infoHeader->biHeight*i + j] = log_shift(val);
            
        }
    }
}






int main(void){
    BITMAPINFOHEADER bitmapInfoHeader;
    BITMAPFILEHEADER bitmapFileHeader;
    COLORTABLE       colorTable[256]; //256 ak je info.bitOCunt==8 inak vid
     //http://www.ue.eti.pg.gda.pl/fpgalab/zadania.spartan3/zad_vga_struktura_pliku_bmp_en.html
    unsigned char *bitmapData;
    bitmapData = LoadBitmapFile("bmp.bmp",&bitmapInfoHeader,
            &bitmapFileHeader, colorTable);



    unsigned char *resultMap = (unsigned char *)malloc(bitmapInfoHeader.biSizeImage);
    
    focus(bitmapData,resultMap,&bitmapInfoHeader);
    
    saveBmp("step1.bmp",resultMap,bitmapInfoHeader, bitmapFileHeader,
            colorTable);

    unsigned char *ptr = bitmapData;
    bitmapData = resultMap;
    resultMap = ptr;

    flip_vertical(bitmapData,resultMap, &bitmapInfoHeader);
    //flip_vertical(bitmapData,resultMap, &bitmapInfoHeader);
    
    saveBmp("step2.bmp",resultMap,bitmapInfoHeader, bitmapFileHeader,
            colorTable);
    
    ptr = bitmapData;
    bitmapData = resultMap;
    resultMap = ptr;

    median_filter(bitmapData,resultMap, &bitmapInfoHeader);
    
    saveBmp("step3.bmp",resultMap,bitmapInfoHeader, bitmapFileHeader,
            colorTable);

    
    ptr = bitmapData;
    bitmapData = resultMap;
    resultMap = ptr;

    blur(bitmapData,resultMap, &bitmapInfoHeader);
    
    saveBmp("step4.bmp",resultMap,bitmapInfoHeader, bitmapFileHeader,
            colorTable);

    
    bitmapData = LoadBitmapFile("bmp.bmp",&bitmapInfoHeader,
            &bitmapFileHeader, colorTable);

    unsigned char *Original = (unsigned char *)malloc(bitmapInfoHeader.biSizeImage);
    flip_vertical(bitmapData,Original, &bitmapInfoHeader);
    error_value(Original,resultMap, &bitmapInfoHeader);
    
    ptr = bitmapData;
    bitmapData = resultMap;
    resultMap = ptr;

    printf("mean_no_his=%.7g\n",mean(bitmapData, &bitmapInfoHeader));
    printf("std_no_his=%.6g\n",std(bitmapData, &bitmapInfoHeader));
    
    extend_his(bitmapData,resultMap, &bitmapInfoHeader);
    saveBmp("step5.bmp",resultMap,bitmapInfoHeader, bitmapFileHeader,
            colorTable);
    printf("mean_his=%.7g\n",mean(resultMap, &bitmapInfoHeader));
    printf("mean_his=%.7g\n",std(resultMap, &bitmapInfoHeader));
    
    
    ptr = bitmapData;
    bitmapData = resultMap;
    resultMap = ptr;

    quant(bitmapData,resultMap, &bitmapInfoHeader);
    saveBmp("step6.bmp",resultMap,bitmapInfoHeader, bitmapFileHeader,
            colorTable);

}

