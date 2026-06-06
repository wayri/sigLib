#include <stdio.h>
#include "../sigmain.h"

int main() {
    // 1. Declare and initialize the filter
    filter signal_filter;
    finit(&signal_filter);
    
    // 2. (Optional) Set custom coefficients. 
    // Here we make a simple 10-point moving average by setting all coeffs to 0.1.
    float ma_coeffs[LEN_FILT] = {0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f};
    f_set_coeffs(&signal_filter, ma_coeffs);
    
    printf("Starting Filter simulation...\n");
    
    // A step function input that jumps to 10.0, with some fake noise
    for (int step = 0; step < 15; step++) {
        float raw_signal = 10.0f;
        
        // Introduce artificial noise on step 5
        if (step == 5) raw_signal = 25.0f; 
        
        // 3. Refresh filter with new data point
        float filtered_signal = f_refresh(&signal_filter, raw_signal);
        
        printf("Step %2d | Raw Signal: %6.2f | Filtered: %6.2f\n", 
               step, raw_signal, filtered_signal);
    }

    return 0;
}
