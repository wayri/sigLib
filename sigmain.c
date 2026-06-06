#include "sigmain.h"
#include <math.h>
#include <stddef.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void finit(filter *f, uint8_t order){
    f->order = order > MAX_FILT_ORDER ? MAX_FILT_ORDER : order;
    for(uint8_t n = 0; n < MAX_FILT_ORDER; n++){
        f->x_buf[n] = 0.0f;
        f->y_buf[n] = 0.0f;
        f->b[n] = 0.0f;
        f->a[n] = 0.0f;
    }
    f->b[0] = 1.0f; // Default pass-through
    f->ind = 0;
    f->out = 0.0f;
}

void f_set_coeffs(filter *f, const float *b, const float *a){
    for(uint8_t n = 0; n < f->order; n++){
        f->b[n] = b[n];
        if (a != NULL) {
            f->a[n] = a[n];
        } else {
            f->a[n] = 0.0f;
        }
    }
}

void f_design_biquad(filter *f, filter_type_t type, float cutoff_freq, float sample_freq) {
    f->order = 3; // 2nd order biquad uses 3 coefficients (0, 1, 2)
    
    float omega = 2.0f * (float)M_PI * cutoff_freq / sample_freq;
    float alpha = sinf(omega) / 1.41421356f; // Q = sqrt(2)/2 for Butterworth
    float cosw = cosf(omega);

    float b0, b1, b2, a0, a1, a2;

    if (type == FILTER_TYPE_LPF) {
        b0 = (1.0f - cosw) / 2.0f;
        b1 = 1.0f - cosw;
        b2 = (1.0f - cosw) / 2.0f;
    } else { // HPF
        b0 = (1.0f + cosw) / 2.0f;
        b1 = -(1.0f + cosw);
        b2 = (1.0f + cosw) / 2.0f;
    }

    a0 = 1.0f + alpha;
    a1 = -2.0f * cosw;
    a2 = 1.0f - alpha;

    // Normalize and set
    f->b[0] = b0 / a0;
    f->b[1] = b1 / a0;
    f->b[2] = b2 / a0;
    
    // a0 is implicitly 1.0, store a1 and a2
    f->a[1] = a1 / a0;
    f->a[2] = a2 / a0;
    f->a[0] = 1.0f; // Not used in convolution but good for reference
}

float f_refresh(filter *f, float in){
    // Update circular buffer with new input
    f->x_buf[f->ind] = in;

    float acc = 0.0f;
    
    // Convolution for Direct Form I
    for(uint8_t n = 0; n < f->order; n++){
        int8_t x_idx = f->ind - n;
        if(x_idx < 0) x_idx += MAX_FILT_ORDER;
        
        acc += f->b[n] * f->x_buf[x_idx];
        
        if (n > 0) {
            int8_t y_idx = f->ind - n;
            if(y_idx < 0) y_idx += MAX_FILT_ORDER;
            acc -= f->a[n] * f->y_buf[y_idx];
        }
    }

    // Save output
    f->y_buf[f->ind] = acc;
    f->out = acc;

    // Increment index
    f->ind++;
    if (f->ind >= MAX_FILT_ORDER) f->ind = 0; 

    return f->out;
}