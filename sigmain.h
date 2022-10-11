/*

sigLib - A simple signal processing library

This library provides simple signal processing capability which can be integrated in microcontroller or other application environments.

This library contains following functions;
  - Filters with circular buffers | order adjusted
    - based on custom impulse response
    - Low pass
    - High pass
    - Band pass
    - Band stop
    - average
  - PID controller | discrete
    - P
    - PI
    - PI & D
    - PID
  
  
  
  ---> later
  
    mpc
    lqr
    feedforward
    fopid
    ? kalman integration ??
  ----
  
  

*/
#include <stdint.h>

/*
calculating coefficients depends upon a few things including, signal frequency, cutoff frequency, and sampling rate.
however we can take only the number of coefficents needed. The number depends upon complexity of filter and hardware speed.
A good result can be obtained between 10 and 100. where lower is preferred for embedded hardware and higher for more PC-like.


*/


#define LEN_FILT 10 // number of coefficients of the filter 

typedef struct{
  float buffer[LEN_FILT]; // circular buffer
  uint8_t b_ind;  // index for buffer

  float out; //filter output


}filter;

void finit(filter *f);
float f_refresh(filter *f, float in);

float mean_filter(filter *f, uint8_t len);
float derivative(filter *f);
