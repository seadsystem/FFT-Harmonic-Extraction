//Henry Crute hcrute@ucsc.edu

#include <string.h>
#include <stdint.h>
#include <math.h>
#include <fftw3.h>

#include "fftlib.h"

//partition part of quicksort
int partition(double (*a)[2], int l, int r) {
   int i, j;   
   double pivot;
   double t1, t2;
   pivot = a[l][0];
   i = l; j = r+1;
   while(1) {
      do ++i; while( a[i][0] <= pivot && i <= r );
      do --j; while( a[j][0] > pivot );
      if( i >= j ) break;
      t1 = a[i][0]; a[i][0] = a[j][0]; a[j][0] = t1;
      t2 = a[i][1]; a[i][1] = a[j][1]; a[j][1] = t2;
   }
   t1 = a[l][0]; a[l][0] = a[j][0]; a[j][0] = t1;
   t2 = a[l][1]; a[l][1] = a[j][1]; a[j][1] = t2;
   return j;
}

//generic quicksort stolen from online
void quickSort(double (*a)[2], int l, int r) {
   int j;
   if( l < r ) {
      // divide and conquer
      j = partition(a, l, r);
      quickSort(a, l, j - 1);
      quickSort(a, j + 1, r);
   }
}

//prints array of numbers with specified length
void print_array(double *array, int length) {
   int i;
   for (i = 0; i < length; i++) {
      fprintf(stdout, "%f\n", array[i]);
   }
}
//prints complex array in order [real] [imaginary]
void print_fftw_complex(fftw_complex *array, int length) {
   int i;
   for (i = 0; i < length; i++) {
      fprintf(stdout, "%f\n", array[i][0]);
      fprintf(stdout, "%f\n", array[i][1]);
   }
}
//takes in array of complex numbers and converts it into decibels (assuming complex voltage)
void print_decibels(fftw_complex *array, int length) {
   int i;
   for (i = 0; i < length; i++) {
      fprintf(stdout, "%f\n", 10*log10(pow(array[i][0], 2) + pow(array[i][1], 2)));
   }
}
//takes in array of complex numbers and prints their magnitude
void print_magnitude(fftw_complex *array, int length) {
   int i;
   for (i = 0; i < length; i++) {
      fprintf(stdout, "%f\n", sqrt(pow(array[i][0], 2) + pow(array[i][1], 2)));
   }
}

//prints the k largest elements in array by using quicksort and grabbing
//largest k elements
void print_harmonic_frequencies(fftw_complex *array, int k, int length) {
   double maximums[length][2];
   int i;
   //stores magnitude in two dimentional array
   //first element is magnitude, second is index (for freq calc)
   for(i = 0; i < length; i++){
      maximums[i][0] = sqrt(pow(array[i][0], 2) + pow(array[i][1], 2));
      maximums[i][1] = i;
   }
   //quicksort to get first k maximums in array with indexes preserved
   quickSort(maximums, 0, length - 1);
   for (i = length - 1; i > length - 1 - k; i--) {
      fprintf(stdout, "%f is a maximum at %fHz\n", maximums[i][0],
              maximums[i][1] * (SAMPLE_RATE / 2.0) / (SAMPLE_SIZE / 2.0));
   }
}

//retreives data from file and inserts into buffer array
//returns size of array
//arguments, buffer array, and opened input file
int get_array(double *buffer, FILE *in, int bufferLength) {
   uint8_t inputString[32];
   int i = 0;
	while(fgets(inputString, sizeof(inputString), in)) {
      //replaces new line with null
      int length = strlen(inputString);
      if (length < 2) {
         continue;
      }
		inputString[length - 2] = '\0';
      //printf("doesn't fail here %s\n", inputString);
      //multiplied by constant to make numbers normalized to Volts???
      buffer[i] = strtol(inputString, NULL, 10) * 0.00000523972;
		//fprintf(stdout, "%f\n", buffer[i]);
      i++;
      if (i > bufferLength - 1) {
         break;
      }
	}
   return i;
}

