/*

Control Library implements basic control constructs for implementing a simple closed loop system

@author - Yawar Badri (Wayri) 2022

  - PID controller | discrete
    - P
    - PI
    - PI & D
    - PID


*/

#include <stdint.h>

#define awindup 2
float err = 0.0f;

typedef struct {
    float din, dout;    // controller in and out
    float P; // proportional
    float I; // integral   
    float D; // derivative
    float dt; // time delta
}controller;



float pid(float din, controller *control);
float pi(float din, controller *control);
float i_pd(float din, float dout, controller *control);
float pi_d(float din, float dout, controller *control);
float satrn(float din, float lb, float ub);
