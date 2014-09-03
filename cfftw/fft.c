//Henry Crute hcrute@ucsc.edu
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <fftw3.h>

#include "fftlib.h"

clock_t start, end;
double cpu_time_used;

//returns number of computations of size SAMPLE_SIZE
int computeFFT(double *buffer, double *in, fftw_complex *out, FILE *input, fftw_plan plan) {
   int i, j, size;
   //fft loop gets numbers from file, copies into in, executes, repeat
   size = get_array(in, input, SAMPLE_SIZE);
   for (j = 0; size == SAMPLE_SIZE; j++) {
      //for (i = 0; i < SAMPLE_SIZE; i++) {
      //   in[i] = buffer[i];
      //}
      fftw_execute(plan);
      if (j == 1) {
         //print_magnitude(out, SAMPLE_SIZE / 2 + 1);
         print_harmonic_frequencies(out, 4, SAMPLE_SIZE / 2 + 1);
      }
      size = get_array(in, input, SAMPLE_SIZE);
      //printf("size is %i\n", size);
   }
   return j + 1;
}


int main(int argc, char *argv[]) {
   //variable declarations
   FILE *input;
   double buffer[SAMPLE_SIZE];
   double *in = fftw_alloc_real(SAMPLE_SIZE);
   fftw_complex *out = fftw_alloc_complex(SAMPLE_SIZE / 2 + 1);
   
   if (argc > 2) {
		printf("Usage: fft input\r\n");
	} else {
	  input = fopen(argv[1], "r");
	  if (input == NULL) {
	    printf("Couldn't open %s for reading.\r\n", argv[1]);
		return(1);
	  }
	}

   //fprintf(stdout, "%i numbers\n", size);
   //real to complex plan
   
   fftw_plan plan = fftw_plan_dft_r2c_1d(SAMPLE_SIZE, in, out,
                           FFTW_MEASURE | FFTW_PRESERVE_INPUT);
   //measures how long it takes to do fft computations
   start = clock(); ///////////////////////////////Start Clock
   int count = computeFFT(buffer, in, out, input, plan);
   end = clock(); /////////////////////////////////End Clock

   fprintf(stdout, "computed fft of size %i, %i number of times\n", SAMPLE_SIZE, count);
   fprintf(stdout, "cpu time used = %f\n", ((double) (end - start)) / CLOCKS_PER_SEC);

   //destructors
   fftw_destroy_plan(plan);
   fftw_free(in);
   fftw_free(out);
   fftw_cleanup();
   fclose(input);
   return(0);
}
