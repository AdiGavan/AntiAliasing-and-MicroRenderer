// "Copyright [2018] Gavan Adrian-George, 334CA"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#ifndef HOMEWORK_H1
#define HOMEWORK_H1

// Structura pentru imagine.
typedef struct {
  char type[2];
  int width;
  int height;
  int maxval;
  unsigned char** pixels_bw;
}image;

// Structura pentru informatii thread.
typedef struct {
    int thread_id;
    image* img;
} info;

void initialize(image *im);
void render(image *im);
void writeData(const char * fileName, image *img);

#endif /* HOMEWORK_H1 */
