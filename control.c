#include "control.h"


/*
Implements a full PID controller
To tune the code, you can try brute force or standard practices

--> try later / auto tuner

user first needs to declare a control structure of type controller
then use the variables from that to modify parameters in it.

*/
float pid_sum = 0.0f;
float d_integral = 0.0f;
float d_diff = 0.0f;
float din_old = 0.0f;
float d_prop = 0.0f;

float pid(float din, controller *control){

    // Integration
    d_integral += din * control->dt;

    // differentiation
    d_diff = (din - din_old)/control->dt;
    din_old=din;

    // pid sum
    pid_sum = control->P + control->I * d_integral + control->D * d_diff;
    
    return pid_sum;
}


float pi(float din, controller *control){

    // Integration
    d_integral += din * control->dt;

    // pid sum
    pid_sum = control->P + control->I * d_integral;
    
    return pid_sum;
}

/*
    General 2DOF PID controller
    u = -P(br-y) + I * 1/s * (r-y) - D*s (cr-y)

    b = 1, c = 0 | PI-D
    b = 0, c = 0 | I-PD
*/

/*
I_PD controller | standalone

Takes in reference and final output value and internally computes error and gives out controller output u|PID_sum
*/
float i_pd(float dref, float dout, controller *control){

    /*
        u = Py + I * 1/s * (r-y) + D*s*y

        Error is computed in the loop itself;
    */

    // error
    err = dref - dout;

    // integral
    d_integral += err * control->dt;

    // differentiation of y aka output
    d_diff = (dout - din_old)/control->dt;
    din_old = dout;

    pid_sum = control->P * dout + control->I * d_integral * err + control->D * d_diff * dout;
    return pid_sum;
}


float pi_d(float dref, float dout, controller *control){

    /*
    u = -P(r-y) + I * 1/s * (r-y) + D*s*y)

    Error is computed in the loop itself;
    */

    // error
    err = dref - dout;

    // proportional
    d_prop = -1 * control->P * err;

    // integral
    d_integral += err * control->dt;

    // differentiation of y aka output
    d_diff = (dout - din_old)/control->dt;
    din_old = dout;

    pid_sum = d_prop + control->I * d_integral * err + control->D * d_diff * dout;


    return pid_sum;
}


float satrn(float din, float lb, float ub){

    return din>lb ? (din<ub ? din : ub) : lb ;

}
