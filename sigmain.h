/*
    This library is based on github.com/pms67 -> https://github.com/pms67/HadesFCS/tree/master/Filtering

    So for filter implementation please use the vscode program developed by him for now.


    Later iterations of this library will hopefully become standalone and will not need external computation.
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
