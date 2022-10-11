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
uint8_t err = 0;

typedef struct {
    float din, dout;    // controller in and out
    float P; // proportional
    float I; // integral   
    float D; // derivative

}controller;


float pid(float din, controller *control);
float pi(float din, controller *control);
float pi_d(float din, controller *control);
float p_controller(float din, controller *control);