// "Copyright [2018] Gavan Adrian-George, 334CA"
#include "homework.h"

#define KERNELSUM 16

int num_threads;
int resize_factor;
int gaussKer[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};

// Functie pentru citit datele imaginii primite ca parametru.
void readInput(const char * fileName, image *img) {
  FILE *f = fopen(fileName, "r");
  if(f == NULL) {
    exit(1);
  }

  // Se citesc tipul imaginii, latimea, lungimea si valoarea maxima.
  fscanf(f,"%c%c\n", &(img->type[0]), &(img->type[1]));
  fscanf(f,"%d %d\n", &(img->width), &(img->height));
  fscanf(f,"%d\n", &(img->maxval));

  int height = img->height;
  int width = img->width;
  int i = 0;

  // Se aloca memorie pentru matricea pixelilor.
  // Cazul cand imaginea este de tipul "P5" (alb sau negru)
  if(img->type[1] == '5') {
    img->pixels_bw = (unsigned char**)malloc(height * sizeof(unsigned char*));
    if(img->pixels_bw == NULL) {
      exit(2);
    }

    for (i = 0; i < height; i++) {
      img->pixels_bw[i] = (unsigned char*)malloc(width  * sizeof(unsigned char));
      if(img->pixels_bw[i] == NULL) {
        exit(2);
      }
    }

    for (i = 0; i < height; i++) {
      fread(img->pixels_bw[i], 1, width, f);
    }

    // Cazul cand imaginea este de tipul "P6" (color)
  } else {
    img->pixels = (rgb**)malloc(height * sizeof(rgb*));
    if(img->pixels == NULL) {
      exit(2);
    }

    for (i = 0; i < height; i++) {
      img->pixels[i] = (rgb*)malloc(width  * sizeof(rgb));
      if(img->pixels[i] == NULL) {
        exit(2);
      }
    }

    for (i = 0; i < height; i++) {
      fread(img->pixels[i], 1, 3 * width, f);
    }
  }

  fclose(f);
}

// Functie pentru eliberarea memoriei cand poza este alb-negru.
void free_memory_BW(image* img) {
  int i = 0;
  int height = img->height;
  for(i = 0; i < height; i++) {
    free(img->pixels_bw[i]);
  }
  free(img->pixels_bw);
}

// Functie pentru eliberarea memoriei cand poza este color.
void free_memory_Color(image* img) {
  int i = 0;
  int height = img->height;
  for(i = 0; i < height; i++) {
    free(img->pixels[i]);
  }
  free(img->pixels);
}

// Functie pentru scrierea pixelilor unei imagini alb-negru.
void writeDataBW(FILE *f, image *img) {
  int i = 0;
  int height = img->height;
  int width = img->width;
  // Valorile pixelilor trebuie scrise pe un rand.
  // Se scrie fiecare linie din matricea de pixeli.
  for (i = 0; i < height; i++) {
    fwrite(img->pixels_bw[i], 1, width, f);
  }

}

// Functie pentru scrierea pixelilor unei imagini color.
void writeDataColor(FILE *f, image *img) {
  int i = 0;
  int height = img->height;
  int width = img->width;
  // Valorile pixelilor trebuie scrise pe un rand.
  // Se scrie fiecare linie din matricea de pixeli.
  for (i = 0; i < height; i++) {
    fwrite(img->pixels[i], 1, 3 * width, f);
  }
}

// Functie pentru scrierea datelor noii imagini in fisierul de iesire.
void writeData(const char * fileName, image *img) {
  FILE *f = fopen(fileName, "w");
  if(f == NULL) {
    exit(1);
  }

  // Se scriu mai intai tipul imaginii, latimea, lungimea si valoarea maxima.
  fprintf(f,"%c%c\n", img->type[0], img->type[1]);
  fprintf(f,"%d %d\n", img->width, img->height);
  fprintf(f,"%d\n", img->maxval);

  // Daca imaginea este alb-negru, se apeleaza functia pentru scrierea pixelilor
  // unei imagini alb-negru. Daca imaginea este color se apeleaza functia pentru
  // scrierea pixelilor unei imagini color.
  if(img->type[1] == '5') {
    writeDataBW(f, img);
  } else {
    writeDataColor(f, img);
  }

  // Se elibereaza memoria alocata pentru imaginea de iesire.
  if(img->type[1] == '5') {
    free_memory_BW(img);
  } else {
    free_memory_Color(img);
  }

  fclose(f);
}

// Functie pentru calcularea pixelilor unei
// imagini alb-negru cu resize_factor par.
void* resizeODD_BW(void* var) {
  info information = *(info*)var;
  image* in_aux = information.in_aux;
  image* out_aux = information.out_aux;
  int thread_id = information.thread_id;
  int height = out_aux->height;
  int width = out_aux->width;
  int res_pat = resize_factor * resize_factor;
  int resFact = resize_factor;
  int s_wb = 0, i_out = 0, j_out = 0, i_in = 0, j_in = 0;

  // Se calculeaza indicii de inceput si sfarsit ai thread-ului.
  int start = ceil((double)height / num_threads) * thread_id;
  int end = fmin((thread_id + 1) * ceil((double)height / num_threads), height);

  // Fiecare thread calculeaza valorile pixelilor pentru un interval
  // de linii din matricea pixelilor (pentru imaginea de iesire).
  for(i_out = start; i_out < end; i_out++) {
    for(j_out = 0; j_out < width; j_out++) {
      s_wb = 0;
      // Pentru fiecare pixel din matricea de iesire se calculeaza media
      // aritmetica a resize_factor^2 pixeli din poza originala.
      for(i_in = i_out * resFact; i_in < i_out * resFact + resFact; i_in++) {
        for(j_in = j_out * resFact; j_in < j_out * resFact + resFact; j_in++) {
          // Se aduna valorile pixelilor din imaginea originala.
          s_wb += in_aux->pixels_bw[i_in][j_in];
        }
      }
      // Se imparte suma obtinuta la resize_factor^2 si se atribuie rezultatul
      // pixelului curent din imaginea de iesire.
      out_aux->pixels_bw[i_out][j_out] = s_wb / res_pat;
    }
  }

  return NULL;
}

// Functie pentru calcularea pixelilor unei
// imagini color cu resize_factor par.
void* resizeODD_Color(void* var) {
  info information = *(info*)var;
  image* in_aux = information.in_aux;
  image* out_aux = information.out_aux;
  int thread_id = information.thread_id;
  int height = out_aux->height;
  int width = out_aux->width;
  int res_pat = resize_factor * resize_factor;
  int resFact = resize_factor;
  int s_r = 0, s_g = 0, s_b = 0, i_out = 0, j_out = 0, i_in = 0, j_in = 0;

  // Se calculeaza indicii de inceput si sfarsit ai thread-ului.
  int start = ceil((double)height / num_threads) * thread_id;
  int end = fmin((thread_id + 1) * ceil((double)height / num_threads), height);

  // Fiecare thread calculeaza valorile pixelilor pentru un interval
  // de linii din matricea pixelilor (pentru imaginea de iesire).
  for(i_out = start; i_out < end; i_out++) {
    for(j_out = 0; j_out < width; j_out++) {
      s_r = 0;
      s_g = 0;
      s_b = 0;
      // Pentru fiecare pixel din matricea de iesire se calculeaza media
      // aritmetica a resize_factor^2 pixeli din poza originala.
      // Fiecare pixel este format din 3 valori, asa ca media aritmetica se
      // va face pentru fiecare culoare in parte.
      for(i_in = i_out * resFact; i_in < i_out * resFact + resFact; i_in++) {
        for(j_in = j_out * resFact; j_in < j_out * resFact + resFact; j_in++) {
          // Se aduna valorile pixelilor din imaginea originala (pentru rosu,
          // verde si albastru separat).
          s_r += in_aux->pixels[i_in][j_in].red;
          s_g += in_aux->pixels[i_in][j_in].green;
          s_b += in_aux->pixels[i_in][j_in].blue;
        }
      }
      // Se imparte suma obtinuta la resize_factor^2 si se atribuie rezultatul
      // pixelului curent din imaginea de iesire (pentru fiecare culoare).
      out_aux->pixels[i_out][j_out].red = s_r / res_pat;
      out_aux->pixels[i_out][j_out].green = s_g / res_pat;
      out_aux->pixels[i_out][j_out].blue = s_b / res_pat;
    }
  }

  return NULL;
}

// Functie pentru calcularea pixelilor unei
// imagini alb-negru cu resize_factor = 3.
void* resizeEVEN_BW(void* var) {
  info information = *(info*)var;
  image* in_aux = information.in_aux;
  image* out_aux = information.out_aux;
  int thread_id = information.thread_id;
  int height = out_aux->height;
  int width = out_aux->width;
  int resFact = resize_factor;
  int s_wb = 0, i_out = 0, j_out = 0, i_in = 0, j_in = 0;

  // Se calculeaza indicii de inceput si sfarsit ai thread-ului.
  int start = ceil((double)height / num_threads) * thread_id;
  int end = fmin((thread_id + 1) * ceil((double)height / num_threads), height);

  // Fiecare thread calculeaza valorile pixelilor pentru un interval
  // de linii din matricea pixelilor (pentru imaginea de iesire).
  for(i_out = start; i_out < end; i_out++) {
    for(j_out = 0; j_out < width; j_out++) {
      s_wb = 0;
      // Pentru fiecare pixel din matricea de iesire se inmultesc valorile
      // celor 9 pixeli corespunzatori din imaginea de intrare cu elementul
      // corespunator din kernel-ul Gaussian, se aduna valorile si se imparte
      // rezultatul la 16.
      for(i_in = i_out * resFact; i_in < i_out * resFact + resFact; i_in++) {
        for(j_in = j_out * resFact; j_in < j_out * resFact + resFact; j_in++) {
          // Se aduna valorile pixelilor din imaginea originala, inmultite
          // cu elementul corespunzator din Kernel-ul Gaussian.
          s_wb += in_aux->pixels_bw[i_in][j_in] * gaussKer[i_in % 3][j_in % 3];
        }
      }
      // Se imparte suma obtinuta la 16 si se atribuie rezultatul
      // pixelului curent din imaginea de iesire.
      out_aux->pixels_bw[i_out][j_out] = s_wb / KERNELSUM;
    }
  }

  return NULL;
}

// Functie pentru calcularea pixelilor unei
// imagini color cu resize_factor = 3.
void* resizeEVEN_Color(void* var) {
  info information = *(info*)var;
  image* in_aux = information.in_aux;
  image* out_aux = information.out_aux;
  int thread_id = information.thread_id;
  int height = out_aux->height;
  int width = out_aux->width;
  int resFact = resize_factor;
  int s_r = 0, s_g = 0, s_b = 0, i_out = 0, j_out = 0, i_in = 0, j_in = 0;

  // Se calculeaza indicii de inceput si sfarsit ai thread-ului.
  int start = ceil((double)height / num_threads) * thread_id;
  int end = fmin((thread_id + 1) * ceil((double)height / num_threads), height);

  // Fiecare thread calculeaza valorile pixelilor pentru un interval
  // de linii din matricea pixelilor (pentru imaginea de iesire).
  for(i_out = start; i_out < end; i_out++) {
    for(j_out = 0; j_out < width; j_out++) {
      s_r = 0;
      s_g = 0;
      s_b = 0;
      // Pentru fiecare pixel din matricea de iesire se inmultesc valorile
      // celor 9 pixeli corespunzatori din imaginea de intrare cu elementul
      // corespunator din kernel-ul Gaussian, se aduna valorile si se imparte
      // rezultatul la 16. Calculul se face separat pentru cele 3 culori.
      for (i_in = i_out * resFact; i_in < i_out * resFact + resFact; i_in++) {
        for (j_in = j_out * resFact; j_in < j_out * resFact + resFact; j_in++) {
          // Se aduna valorile pixelilor din imaginea originala, inmultite
          // cu elementul corespunzator din Kernel-ul Gaussian (pentru
          // fiecare culoare in parte).
          s_r += in_aux->pixels[i_in][j_in].red * gaussKer[i_in % 3][j_in % 3];
          s_g += in_aux->pixels[i_in][j_in].green * gaussKer[i_in % 3][j_in % 3];
          s_b += in_aux->pixels[i_in][j_in].blue * gaussKer[i_in % 3][j_in % 3];
        }
      }
      // Se imparte suma obtinuta la 16 si se atribuie rezultatul pixelului
      // curent din imaginea de iesire (pentru fiecare culoare in parte).
      out_aux->pixels[i_out][j_out].red = s_r / KERNELSUM;
      out_aux->pixels[i_out][j_out].green = s_g / KERNELSUM;
      out_aux->pixels[i_out][j_out].blue = s_b / KERNELSUM;
    }
  }

    return NULL;
}

// Functie pentru calcularea pixelilor unei imagini cu resize_factor par.
void resizeODD(image *in, image *out) {
  pthread_t tid[num_threads];
  info information_vec[num_threads];

  // Fiecare thread va primi o structura de tipul "info",
  // care contine imaginea de intrare, iesire si id-ul thread-ului.
  for(int i = 0; i < num_threads; i++) {
    information_vec[i].thread_id = i;
    information_vec[i].in_aux = in;
    information_vec[i].out_aux = out;
  }

  // Se creeaza thread-urile. Daca imaginea este alb-negru,
  // se utilizeaza functia resizeODD_BW. Daca imaginea este color
  // se utilizeaza functia resizeODD_Color.
  if(out->type[1] == '5') {
    for(int i = 0; i < num_threads; i++) {
      pthread_create(&(tid[i]), NULL, resizeODD_BW, &(information_vec[i]));
    }
  } else {
    for(int i = 0; i < num_threads; i++) {
      pthread_create(&(tid[i]), NULL, resizeODD_Color, &(information_vec[i]));
    }
  }

  for(int i = 0; i < num_threads; i++) {
    pthread_join(tid[i], NULL);
  }
}

// Functie pentru calcularea pixelilor unei imagini cu resize_factor = 3.
void resizeEVEN(image *in, image *out) {
  pthread_t tid[num_threads];
  info information_vec[num_threads];

  // Fiecare thread va primi o structura de tipul "info",
  // care contine imaginea de intrare, iesire si id-ul thread-ului.
  for(int i = 0; i < num_threads; i++) {
    information_vec[i].thread_id = i;
    information_vec[i].in_aux = in;
    information_vec[i].out_aux = out;
  }

  // Se creeaza thread-urile. Daca imaginea este alb-negru,
  // se utilizeaza functia resizeEVEN_BW. Daca imaginea este color
  // se utilizeaza functia resizeEVEN_Color.
  if(out->type[1] == '5') {
    for(int i = 0; i < num_threads; i++) {
      pthread_create(&(tid[i]), NULL, resizeEVEN_BW, &(information_vec[i]));
    }
  } else {
    for(int i = 0; i < num_threads; i++) {
      pthread_create(&(tid[i]), NULL, resizeEVEN_Color, &(information_vec[i]));
    }
  }

  for(int i = 0; i < num_threads; i++) {
    pthread_join(tid[i], NULL);
  }
}

// Functia care calculeaza valoarea pixelilor pentru noua imagine.
void resize(image *in, image * out) {
  // Imaginea de iesire primeste valorile pentru tip, latime, lungime
  // si valoare maxima.
  out->type[0] = in->type[0];
  out->type[1] = in->type[1];
  out->height = in->height / resize_factor;
  out->width = in->width / resize_factor;
  out->maxval = in->maxval;

  int height = out->height;
  int width = out->width;
  int i = 0;

  // Se aloca memorie pentru imaginea de iesire.
  // Cazul cand imaginea este alb-negru.
  if(out->type[1] == '5') {
    out->pixels_bw = (unsigned char**)malloc(height * sizeof(unsigned char*));
    if(out->pixels_bw == NULL) {
      exit(2);
    }
    for (i = 0; i < height; i++) {
      out->pixels_bw[i] = (unsigned char*)malloc(width * sizeof(unsigned char));
      if(out->pixels_bw[i] == NULL) {
        exit(2);
      }
    }

    // Cazul cand imaginea este color.
  } else {
    out->pixels = (rgb**)malloc(height * sizeof(rgb*));
    if(out->pixels == NULL) {
      exit(2);
    }
    for (i = 0; i < height; i++) {
      out->pixels[i] = (rgb*)malloc(width * sizeof(rgb));
      if(out->pixels[i] == NULL) {
        exit(2);
      }
    }
  }

  // Daca resize_factor este un numar par, se apeleaza functia care
  // calculeaza valoarea pixelilor pentru un resize_factor par.
  // Daca resize_factor este 3, se apeleaza functia care
  // calculeaza valoarea pixelilor pentru resize_factor 3.
  if(resize_factor % 2 == 0) {
    resizeODD(in, out);
  } else {
    resizeEVEN(in, out);
  }

  // Se elibereaza memoria folosita de imaginea de intrare.
  if(in->type[1] == '5') {
    free_memory_BW(in);
  } else {
    free_memory_Color(in);
  }

}
