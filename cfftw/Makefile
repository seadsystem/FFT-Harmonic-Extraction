all : fft

fft : fft.o fftlib.o
	gcc -o fft fft.o fftlib.o -lfftw3 -lm -I.

fft.o : fft.c fftlib.h
	gcc -c fft.c -lfftw3 -lm -I.

fftlib.o : fftlib.c fftlib.h
	gcc -c fftlib.c -lfftw3 -lm -I.

clean :
	rm -f fft *.o
