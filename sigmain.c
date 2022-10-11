#include "sigmain.h"


static float h_filter[LEN_FILT] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};   // impulse response in discrete time domain

void finit(filter *f){

    // reset buffer
    for(uint8_t n = 0; n < LEN_FILT; n++){
        f->buffer[n] = 0.0f;
    }

    // reset index
    f->b_ind = 0;

    // reset output
    f->out = 0.0f;
}

float f_refresh(filter *f, float in){

    // update new sample
    f->buffer[f->b_ind] = in;

    // circular buffering bound checks and increment
    f->b_ind++;
    if (f->b_ind == LEN_FILT) f->b_ind = 0; 

    // convolution | filter operation

    f->out = 0.0f;
    uint8_t ci = f->b_ind;
    
    for(uint8_t n = 0; n < LEN_FILT; n++){

        // check index and shifting
        if(ci> 0){
            ci--;
        } else{
            ci = LEN_FILT-1; 
        }

        // conv
        f->out += h_filter[n] * f->buffer[ci]; //  sum(k= 0 -> M-1) h[k]*x[n-k], M is filter length
    }

    //return value
    return f->out;



}