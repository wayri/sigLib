#include <stdio.h>
#include <math.h>
#include "../sigmain.h"

int main() {
    filter my_biquad;
    finit(&my_biquad, 3);
    
    // Design a 2nd order Low Pass Filter
    // Cutoff: 10 Hz, Sampling Rate: 100 Hz
    f_design_biquad(&my_biquad, FILTER_TYPE_LPF, 10.0f, 100.0f);
    
    printf("Biquad IIR Filter Coefficients:\n");
    printf("b: [%f, %f, %f]\n", my_biquad.b[0], my_biquad.b[1], my_biquad.b[2]);
    printf("a: [%f, %f, %f]\n\n", my_biquad.a[0], my_biquad.a[1], my_biquad.a[2]);

    printf("Simulating Filter with Noisy Sine Wave...\n");
    for (int step = 0; step < 20; step++) {
        // Create a slow 2Hz sine wave + high frequency noise
        float time = step * 0.01f;
        float clean_signal = sinf(2.0f * 3.14159f * 2.0f * time);
        float noise = ((step % 3) - 1) * 0.5f; // Artificial noise
        float raw_signal = clean_signal + noise;
        
        float filtered_signal = f_refresh(&my_biquad, raw_signal);
        
        printf("Time: %.2fs | Raw: %6.2f | Filtered: %6.2f | Clean: %6.2f\n", 
               time, raw_signal, filtered_signal, clean_signal);
    }
    return 0;
}
