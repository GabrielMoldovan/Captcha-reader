#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp_header.h"
#include "matrici.h"
#define NMAX 50

typedef struct {
    unsigned char b;
    unsigned char g;
    unsigned char r;
    int x,y;
    int flag;
} pixel;

typedef struct {
    struct bmp_fileheader file;
    struct bmp_infoheader info;
    unsigned int stride, padding;
    pixel **pixelMatrix;
    unsigned int w, h, bp;
} BMP;


//verifica daca pixelul este alb
int isWhite(pixel pixel){
    if(pixel.r == 255 && pixel.g == 255 && pixel.b == 255)
        return 1;
    else
        return 0;
}
//citirea imaginii
BMP read_bmp(const char *file_name) {
    BMP img;

    FILE *f = fopen(file_name, "rb");
    if (!f) {
        fprintf(stderr, "Eroare alocare");
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_SET);
    fread(&img.file.fileMarker1, 1, 1, f);
    fread(&img.file.fileMarker2, 1, 1, f);
    fread(&img.file.bfSize, 4, 1, f);
    fread(&img.file.unused1, 2, 1, f);
    fread(&img.file.unused2, 2, 1, f);
    fread(&img.file.imageDataOffset, 4, 1, f);

    fread(&img.info.biSize, 4, 1, f);
    fread(&img.info.width, 4, 1, f);
    fread(&img.info.height, 4, 1, f);
    fread(&img.info.planes, 2, 1, f);
    fread(&img.info.bitPix, 2, 1, f);
    fread(&img.info.biCompression, 4, 1, f);
    fread(&img.info.biSizeImage, 4, 1, f);
    fread(&img.info.biXPelsPerMeter, 4, 1, f);
    fread(&img.info.biYPelsPerMeter, 4, 1, f);
    fread(&img.info.biClrUsed, 4, 1, f);
    fread(&img.info.biClrImportant, 4, 1, f);

    img.w = img.info.width;
    img.h = img.info.height;
    img.bp = img.info.bitPix >> 3;
    img.stride = img.w * img.bp;

    //pana aici a fost citit header-ul

    //citirea imaginii in sine

    fseek(f, img.file.imageDataOffset, SEEK_SET);
    
    //generare matrice de pixeli
    img.pixelMatrix = (pixel**)malloc(img.info.height * sizeof(pixel*));
    for(int i = 0; i < img.info.height; i++){
        img.pixelMatrix[i] = (pixel*)malloc(img.info.width * sizeof(pixel));
    }

    unsigned char blue;
    unsigned char green;
    unsigned char red;
    img.padding = img.info.width % 4;

    for(int i = img.info.height - 1; i >= 0; i--) {
        for(int j = 0; j < img.info.width; j++) {
            fread(&blue, sizeof(char), 1, f);
            fread(&green, sizeof(char), 1, f);
            fread(&red, sizeof(char), 1, f);
            img.pixelMatrix[i][j].r = red;
            img.pixelMatrix[i][j].g = green;
            img.pixelMatrix[i][j].b = blue;
            if(isWhite(img. pixelMatrix[i][j])){
                img.pixelMatrix[i][j].flag = 1;
            }else{
                img.pixelMatrix[i][j].flag = 0;
            }
            
        }
        fseek(f, img.padding, SEEK_CUR);
    }

    fclose(f);
    return img;
}

//scrierea unui fisier bmp
void write_bmp(BMP img, const char *file_name) {
    FILE *f = fopen(file_name, "wb");
    if (!f) {
        fprintf(stderr, "Eroare la deschidere fisier");
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_SET);
    fwrite(&img.file.fileMarker1, 1, 1, f);
    fwrite(&img.file.fileMarker2, 1, 1, f);
    fwrite(&img.file.bfSize, 4, 1, f);
    fwrite(&img.file.unused1, 2, 1, f);
    fwrite(&img.file.unused2, 2, 1, f);
    fwrite(&img.file.imageDataOffset, 4, 1, f);

    fwrite(&img.info.biSize, 4, 1, f);
    fwrite(&img.info.width, 4, 1, f);
    fwrite(&img.info.height, 4, 1, f);
    fwrite(&img.info.planes, 2, 1, f);
    fwrite(&img.info.bitPix, 2, 1, f);
    fwrite(&img.info.biCompression, 4, 1, f);
    fwrite(&img.info.biSizeImage, 4, 1, f);
    fwrite(&img.info.biXPelsPerMeter, 4, 1, f);
    fwrite(&img.info.biYPelsPerMeter, 4, 1, f);
    fwrite(&img.info.biClrUsed, 4, 1, f);
    fwrite(&img.info.biClrImportant, 4, 1, f);

    fseek(f, img.file.imageDataOffset, SEEK_SET);
    img.padding = img.info.width % 4;
    char padding[3] = {0, 0, 0};
    
    unsigned char blue;
    unsigned char green;
    unsigned char red;

    for(int i = img.info.height - 1; i >= 0; i--){
        for(int j = 0; j < img.info.width; j++){
            blue = img.pixelMatrix[i][j].b;
            green = img.pixelMatrix[i][j].g;
            red = img.pixelMatrix[i][j].r;

            fwrite(&blue, sizeof(char), 1, f);
            fwrite(&green, sizeof(char), 1, f);
            fwrite(&red, sizeof(char), 1, f);
        }
        fwrite(&padding, sizeof(char), img.padding, f);
    }
    fclose(f);
}


//rezolvare task 1
void task1(BMP img, char name[], FILE *f){
    int b, g, r;
    fscanf(f, "%d", &b);
    fscanf(f, "%d", &g);
    fscanf(f, "%d", &r);
    for(int i = 0; i < img.info.height; i++){
        for(int j = 0; j < img.info.width; j++){
            if(!(isWhite(img.pixelMatrix[i][j]))){
                    img.pixelMatrix[i][j].r = r;
                    img.pixelMatrix[i][j].g = g;
                    img.pixelMatrix[i][j].b = b;
            }
        }
    }
    write_bmp(img, name);
}

//functie ce verifica o matrice 5x5 si returneaza o cifra(sub forma de carcter)
char digit(int** matrix){
    if(sameMatrix(matrix, zero))
        return '0';
    if(sameMatrix(matrix, unu))
        return '1';
    if(sameMatrix(matrix, doi))
        return '2';
    if(sameMatrix(matrix, trei))
        return '3';
    if(sameMatrix(matrix, patru))
        return '4';
    if(sameMatrix(matrix, cinci))
        return '5';
    if(sameMatrix(matrix, sase))
        return '6';
    if(sameMatrix(matrix, sapte))
        return '7';
    if(sameMatrix(matrix, opt))
        return '8';
    if(sameMatrix(matrix, noua))
        return '9';

    return 'c';    
}

//functie ce marcheaza un numar ca find citit si ii construieste o matrice
// 5x5 pentru identificarea cifrei
char markNumber(BMP *img, int x, int y){
    int** matrix = (int**)malloc(5 * sizeof(int*));
    for(int i = 0; i < 5; i++){
        matrix[i] = (int*)malloc(5 * sizeof(int));
    }
    int t = 4;
    for(int i = x; i >= x - 4; i--){
        int k = 4;
        for(int j = y; j >= y - 4; j--){
            if(img->pixelMatrix[i][j].flag == 0){
                matrix[t][k] = 1;
            }else{
                matrix[t][k] = 0;
            }
            img->pixelMatrix[i][j].flag = 1; 
            k--;
        }
        t--;
    }
    return digit(matrix);
 }

//rezolvare task 2
void task2(char* fileName, BMP img){
    FILE *f = fopen(fileName, "w");
    char text[100];
    int counter = 0;
    for(int j = img.info.width - 1; j >= 0; j--){
        for(int i = img.info.height - 1; i >= 0; i--){
            if(img.pixelMatrix[i][j].flag == 0){
                text[counter] = markNumber(&img, i, j);
                counter++;
            }   
        }
    }

    for(int i = counter - 1; i >= 0; i--){
        fprintf(f, "%c", text[i]);   
    }

}

int main(){

    FILE *f = fopen("input.txt", "rt");

    if (!f) {
        fprintf(stderr, "Nu exista fisierul");
        exit(EXIT_FAILURE);
    }

    char name[15], copy[15];
    fscanf(f, "%s", name);
    strcpy(copy, name);
    int n = strlen(copy);
    for (int i = 0; i <= n - 7; i ++){  
        copy[i] = copy[i + 7];
    }
    copy[strlen(copy) - 4] = 0;
    char name1[21] = "captcha";
    char name2[21] = "captcha";
    strcat(name1, copy);
    strcat(name2, copy);
    strcat(name1, "_task1.bmp");
    strcat(name2, "_task2.txt");
    BMP img = read_bmp(name);
    task1(img, name1, f);
    task2(name2, img);
    fclose(f);
    return 0;
}
