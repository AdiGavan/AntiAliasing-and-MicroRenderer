// "Copyright [2018] Gavan Adrian-George, 334CA"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#ifndef HOMEWORK_H
#define HOMEWORK_H

// Structura pentru a retine fiecare culoare a pixelului.
typedef struct {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
}rgb;

// Structura pentru a retine datele unei imagini.
typedef struct {
  char type[2];
  int width;
  int height;
  int maxval;
  unsigned char** pixels_bw;  // Folosit pentru imaginile alb-negru.
  rgb** pixels; // Folosit pentru imaginile color.
}image;

// Structura pentru informatii thread.
typedef struct {
    int thread_id;
    image* in_aux;
    image* out_aux;
} info;

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

#endif /* HOMEWORK_H */
