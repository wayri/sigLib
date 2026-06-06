#include <stdio.h>
#include "sigmain.h"
#include "control.h"

int main()
{
    controller pid_ctrl;
    ctrl_init(&pid_ctrl, 1.0f, 0.1f, 0.01f, 0.01f); // P, I, D, dt

    filter my_filter;
    finit(&my_filter);

    printf("Testing PID and Filter...\n");
    for(int i = 0; i < 10; i++) {
        float error = 1.0f - (i * 0.1f); // fake error
        float ctrl_out = pid(error, &pid_ctrl);
        float filt_out = f_refresh(&my_filter, ctrl_out);
        printf("Step %d: err=%.2f, ctrl_out=%.2f, filt_out=%.2f\n", i, error, ctrl_out, filt_out);
    }
    
    return 0;
}
