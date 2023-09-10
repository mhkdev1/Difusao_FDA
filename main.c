#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pgmfiles.h"
#include "diff2d.h"
#include <omp.h>
#include <time.h>
// gcc -o fda pgmtolist.c pgmfiles.c diff2d.c main.c -lm -fopenmp

int main(int argc, char **argv) {
  char row[80];
  float **matrix;
  int i, j;
  FILE *inimage, *outimage;
  long imax;
  float lambda;
  int result;
  eightBitPGMImage *PGMImage;
  clock_t start, end,USER_start,USER_end;
  double cpu_time_used, inputlag=0;
  
  start = clock(); //tempo inicial do programa
  /* ---- read image name  ---- */

  PGMImage = (eightBitPGMImage *)malloc(sizeof(eightBitPGMImage));

  USER_start = clock();
  if (!argv[1]) {
    printf("name of input PGM image file (with extender): ");
    scanf("%s", PGMImage->fileName);
  } else {
    strcpy(PGMImage->fileName, argv[1]);
  }
  USER_end = clock();
  inputlag = ((double) (USER_end - USER_start))/ CLOCKS_PER_SEC;

  result = read8bitPGM(PGMImage);

  if (result < 0) {
    printPGMFileError(result);
    exit(result);
  }

  /* ---- allocate storage for matrix ---- */

  matrix = (float **)malloc(PGMImage->x * sizeof(float *));
  if (matrix == NULL) {
    printf("not enough storage available\n");
    exit(1);
  }
  for (i = 0; i < PGMImage->x; i++) {
    matrix[i] = (float *)malloc(PGMImage->y * sizeof(float));
    if (matrix[i] == NULL) {
      printf("not enough storage available\n");
      exit(1);
    }
  }

  /* ---- read image data into matrix ---- */

  for (i = 0; i < PGMImage->x; i++)
    for (j = 0; j < PGMImage->y; j++)
      matrix[i][j] = (float)*(PGMImage->imageData + (i * PGMImage->y) + j);

  /* ---- process image ---- */
  USER_start = clock();
  printf("contrast paramter lambda (>0) : ");
  scanf("%f", &lambda);
  printf("number of iterations: ");
  scanf("%ld", &imax);
  USER_end = clock();
  inputlag = inputlag + ((double) (USER_end - USER_start))/ CLOCKS_PER_SEC;

  #pragma omp parallel for private(i)
  for (i = 1; i <= imax; i++) {
    printf("iteration number: %3ld \n", i);
    // You can call your diff2d function here in parallel
    diff2d(0.5, lambda, PGMImage->x, PGMImage->y, matrix);
  }

  /* copy the Result Image to PGM Image/File structure */

  for (i = 0; i < PGMImage->x; i++)
    for (j = 0; j < PGMImage->y; j++)
      *(PGMImage->imageData + i * PGMImage->y + j) = (char)matrix[i][j];

  /* ---- write image ---- */

  if (!argv[2]) {
    printf("name of output PGM image file (with extender): ");
    scanf("%s", PGMImage->fileName);
  } else {
    strcpy(PGMImage->fileName, argv[2]);
  }

  write8bitPGM(PGMImage);

  /* ---- deallocate storage ---- */

  for (i = 0; i < PGMImage->x; i++)
    free(matrix[i]);
  free(matrix);

  free(PGMImage->imageData);
  free(PGMImage);

  /// tempo final do programa completo
  end = clock();

  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("Tempo de entrada de usuario: %f s\n", inputlag);
  printf("Tempo TOTAL de execucao: %f segundos\n", cpu_time_used );
  printf("Tempo de processamento: %f segundos\n", cpu_time_used - inputlag );

  return 0;
}
