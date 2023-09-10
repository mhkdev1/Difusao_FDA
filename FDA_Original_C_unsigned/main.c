#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pgmfiles.h"
#include "diff2d.h"
#include <time.h>



//gcc -o fda pgmtolist.c pgmfiles.c diff2d.c main.c -lm

void main (int argc, char **argv) {
  char   row[80];
  float  **matrix;
  int i, j;
  FILE   *inimage, *outimage;
  long   imax;
  float  lambda;
  int result;
  eightBitPGMImage *PGMImage;

  // medidor de desempenho
    clock_t start, end, totalstart, totalend;
    double cpu_time_used, inputlag=0;
/*

    totalstart = clock();
    start = clock();
    // Chame a função que você deseja medir aqui
    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Tempo de execucao: %f segundos\n", cpu_time_used);
    
    */
    
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  /* ---- read image name  ---- */
  
  totalstart = clock(); //tempo inicial do programa
/////////////////////////////////
  PGMImage = (eightBitPGMImage *) malloc(sizeof(eightBitPGMImage));
	
  if (!argv[1])
  {
    
	  printf("name of input PGM image file (with extender): ");
    scanf("%s", PGMImage->fileName);
  }
  else
  {
    strcpy(PGMImage->fileName, argv[1]);
  }

  result = read8bitPGM(PGMImage);

  if(result < 0) 
    {
      printPGMFileError(result);
      exit(result);
    }

  /* ---- allocate storage for matrix ---- */
  
  matrix = (float **) malloc (PGMImage->x * sizeof(float *));
  if (matrix == NULL)
    { 
      printf("not enough storage available\n");
      exit(1);
    } 
  for (i=0; i<PGMImage->x; i++)
    {
      matrix[i] = (float *) malloc (PGMImage->y * sizeof(float));
      if (matrix[i] == NULL)
        { 
	  printf("not enough storage available\n");
	  exit(1);
        }
    }
  
  /* ---- read image data into matrix ---- */
  
 for (i=0; i<PGMImage->x; i++)
    for (j=0; j<PGMImage->y; j++)
      matrix[i][j] = (float) *(PGMImage->imageData + (i*PGMImage->y) + j); 
  
  /* ---- process image ---- */
  
  start = clock(); //inicia o tempo para o lag do usuario

  printf("contrast paramter lambda (>0) : ");
  //~ gets(row);  sscanf(row, "%f", &lambda);
  scanf("%f", &lambda);
  printf("number of iterations: ");
  //~ gets(row);  sscanf(row, "%ld", &imax);
  scanf("%ld", &imax);

//////////////////////////////////////////////////////////
  end = clock();                        //calcula o lag do input do usuario
  inputlag = ((double) (end - start))/ CLOCKS_PER_SEC;
//////////////////////////////////////////////////////////
  for (i=1; i<=imax; i++)
    {
      printf("iteration number: %3ld \n", i);
      diff2d (0.5, lambda, PGMImage->x, PGMImage->y, matrix); 
    }
  
  /* copy the Result Image to PGM Image/File structure */

  for (i=0; i<PGMImage->x; i++)
    for (j=0; j<PGMImage->y; j++)
      *(PGMImage->imageData + i*PGMImage->y + j) = (char) matrix[i][j];

  /* ---- write image ---- */
  
  if (!argv[2])
  {
    printf("name of output PGM image file (with extender): ");
    scanf("%s", PGMImage->fileName);
  }
  else
  {
    strcpy(PGMImage->fileName, argv[2]);
  }

  write8bitPGM(PGMImage);

  /* ---- disallocate storage ---- */
  
  for (i=0; i<PGMImage->x; i++)
    free(matrix[i]);
  free(matrix);

  free(PGMImage->imageData);
  free(PGMImage);


/// tempo final do programa completo
  totalend = clock();

  cpu_time_used = ((double) (totalend - totalstart)) / CLOCKS_PER_SEC;
  printf("Tempo TOTAL de execucao: %f segundos\n", cpu_time_used - inputlag);
}


