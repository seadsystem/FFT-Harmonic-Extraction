#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "mailbox.h"
#include "gpu_fft.h"

char Usage[] =
        "Usage: hello_fft.bin log2_N [jobs [loops]]\n"
                "log2_N = log2(FFT_length),       log2_N = 8...22\n"
                "jobs   = transforms per batch,   jobs>0,        default 1\n"
                "loops  = number of test repeats, loops>0,       default 1\n";

unsigned Microseconds(void) {
     struct timespec ts;
     clock_gettime(CLOCK_REALTIME, &ts);
     return ts.tv_sec*1000000 + ts.tv_nsec/1000;
}

int main(int argc, char *argv[]) {
     unsigned t[2];
     struct GPU_FFT_COMPLEX *dataIn,*dataOut;
     struct GPU_FFT *fftinfo;

     int mb = mbox_open();
     gpu_fft_prepare(mb,12,GPU_FFT_FWD,1,&fftinfo);

     dataIn = fftinfo->in;
     dataOut = fftinfo->out;

     usleep(1); // Yield to OS
     t[0] = Microseconds();
     gpu_fft_execute(fftinfo); // call one or many times
     t[1] = Microseconds();

     printf("usecs = %d\n", (t[1]-t[0]));

     gpu_fft_release(fftinfo); // Videocore memory lost if not freed !
     return 0;
}