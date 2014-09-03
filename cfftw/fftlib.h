//Henry Crute hcrute@ucsc.edu

#ifndef __FFTLIB_H__
#define __FFTLIB_H__

//defines the array size of the sample at which we do fft
#define SAMPLE_SIZE 2048
//defines the speed at which we are sampling (need for accurate frequencies)
#define SAMPLE_RATE 3500

//prints array of numbers with specified length
void print_array(double *, int);

//prints complex array in order [real] [imaginary]
void print_fftw_complex(fftw_complex *, int);

//takes in array of complex numbers and converts it into decibels (assuming complex voltage)
void print_decibels(fftw_complex *, int);

//takes in array of complex numbers and prints their magnitude
void print_magnitude(fftw_complex *, int);

//prints the k largest elements in array
void print_harmonic_frequencies(fftw_complex *, int, int);

//retreives data from file and inserts into buffer array
//returns size of array
//arguments, buffer array, and opened input file
int get_array(double *, FILE *, int);

#endif
