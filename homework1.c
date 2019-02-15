// "Copyright [2018] Gavan Adrian-George, 334CA"
#include "homework1.h"

#define SPACE 100 // Dimensiune latura spatiu patratic
#define WHITE 255 // Culoarea alba a unui pixel
#define BLACK 0 // Culoarea neagra a unui pixel
#define MAXVAL 255  // Valoarea maxima a unui pixel
#define A -1  // Coeficient X din formula
#define B 2   // Coeficient Y din formula
#define DISTANCE 3  // Distanta maxima punct-linie
#define DENOM 5   // Suma a^2 + b^2

int num_threads;
int resolution;

// Fuctie pentru initializarea imaginii.
void initialize(image *im) {
  // Se initializeaza valorile pentru tipul imaginii,
  // latimea, inaltimea si valoarea maxima.
  im->type[0] = 'P';
  im->type[1] = '5';
  im->width = resolution;
  im->height = resolution;
  im->maxval = MAXVAL;

  int height = im->height;
  int width = im->width;

  // Se aloca memorie pentru matricea pixelilor.
  im->pixels_bw = (unsigned char**)malloc(height * sizeof(unsigned char*));
  if(im->pixels_bw == NULL) {
    exit(2);
  }
  for (int i = 0; i < height; i++) {
    im->pixels_bw[i] = (unsigned char*)malloc(width * sizeof(unsigned char));
    if(im->pixels_bw[i] == NULL) {
      exit(2);
    }
  }
}

// Functia utilizata de fiecare thread.
void* render_thread(void* var) {
  info information = *(info*)var;
  image* img = information.img;
  int thread_id = information.thread_id;
  int height = img->height;
  int width = img->width;
  float scale = (float)SPACE / resolution;

  // Se calculeaza indicele de start si final al fiecarui thread.
  int start = ceil((double)height / num_threads) * thread_id;
  int end = fmin((thread_id + 1) * ceil((double)height / num_threads), height);

  // Fiecare thread calculeaza valorile pixelilor pentru un interval
  // de linii din matricea pixelilor.
  for(int i = start; i < end; i++) {
    for (int j = 0; j < width; j++) {
      // Se calculeaza centrul pixelului si se tine cont ca
      // coordonatele x = 0 si y = 0 sunt in stanga jos a ecranului.
      float x = j + 0.5;
      float y = height - i - 0.5;

      // Se calculeaza distanta dintre centrul pixelului si linie,
      // iar in functie de rezultat se atribuie valoarea corespunzatoare
      // pixelului respectiv.
      // Coordonatele sunt scalate, pentru ca in functie de "resolution",
      // pot fi mai multi pixeli pe cm^2.
      if( (abs(A * x * scale + B * y * scale) / sqrt(DENOM))  <= DISTANCE ) {
        img->pixels_bw[i][j] = BLACK;
      } else {
        img->pixels_bw[i][j] = WHITE;
      }
    }
  }

  return NULL;
}

// Functia calculeaza valoarea fiecarui pixel.
void render(image *im) {
  pthread_t tid[num_threads];
  info information_vec[num_threads];

  // Fiecare thread va primi o structura de tipul "info",
  // care contine imaginea si id-ul thread-ului.
	for(int i = 0; i < num_threads; i++) {
		information_vec[i].thread_id = i;
    information_vec[i].img = im;
  }

  // Se creeaza thread-urile.
  for(int i = 0; i < num_threads; i++) {
  	pthread_create(&(tid[i]), NULL, render_thread, &(information_vec[i]));
  }

  for(int i = 0; i < num_threads; i++) {
    pthread_join(tid[i], NULL);
  }
}

// Functia care scrie datele in fisierul de iesire.
void writeData(const char * fileName, image *img) {
  FILE* f = fopen(fileName, "w");
  if (f == NULL) {
    exit(1);
  }

  // Se scriu mai intai tipul imaginii, latimea,
  // inaltimea si valoarea maxima.
  fprintf(f, "%c%c\n", img->type[0], img->type[1]);
  fprintf(f, "%d %d\n", img->width, img->height);
  fprintf(f, "%d\n", img->maxval);

  // Se scriu toti pixelii in fisier pe o singura linie.
  // Se scrie pe rand fiecare linie din matricea de pixeli in fisier.
  int height = img->height;
  int width = img->width;
  for (int i = 0; i < height; i++) {
    fwrite(img->pixels_bw[i], sizeof(unsigned char), width, f);
  }

  // Se elibereaza memoria.
  for (int i = 0; i < height - 1; i++) {
    free(img->pixels_bw[i]);
  }
  free(img->pixels_bw);

  fclose(f);
}
