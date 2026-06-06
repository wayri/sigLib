/**
 * @file control.h
 * @brief Control Library implements basic control constructs for implementing a simple closed loop system.
 * 
 * @author Yawar Badri (Wayri) 2022
 * 
 * Includes discrete PID implementations:
 *  - P
 *  - PI
 *  - PI-D
 *  - I-PD
 *  - PID
 */

#ifndef CONTROL_H
#define CONTROL_H

#include <stdint.h>

/**
 * @brief Controller structure containing tuning parameters and state variables.
 */
typedef struct {
    float P;  /**< Proportional gain */
    float I;  /**< Integral gain */
    float D;  /**< Derivative gain */
    float dt; /**< Time step (delta t) */

    // Anti-windup
    float int_max; /**< Maximum integral limit */
    float int_min; /**< Minimum integral limit */

    // State variables
    float integral; /**< Accumulated integral */
    float prev_val; /**< Previous value for derivative calculation */
} controller;

/**
 * @brief Initializes the controller parameters and resets state variables.
 * @param control Pointer to the controller struct.
 * @param p Proportional gain.
 * @param i Integral gain.
 * @param d Derivative gain.
 * @param dt Time delta between execution steps.
 */
void ctrl_init(controller *control, float p, float i, float d, float dt);

/**
 * @brief Configures the anti-windup limits for the integrator.
 * @param control Pointer to the controller struct.
 * @param int_max Maximum allowed integral value.
 * @param int_min Minimum allowed integral value.
 */
void ctrl_set_limits(controller *control, float int_max, float int_min);

/**
 * @brief Standard PID controller computation.
 * @param err The current error (setpoint - feedback).
 * @param control Pointer to the controller struct.
 * @return The computed control output.
 */
float pid(float err, controller *control);

/**
 * @brief Standard PI controller computation.
 * @param err The current error (setpoint - feedback).
 * @param control Pointer to the controller struct.
 * @return The computed control output.
 */
float pi(float err, controller *control);

/**
 * @brief I-PD controller computation (Integral on Error, Proportional & Derivative on Measurement).
 * @param dref The reference setpoint.
 * @param dout The current measured feedback output.
 * @param control Pointer to the controller struct.
 * @return The computed control output.
 */
float i_pd(float dref, float dout, controller *control);

/**
 * @brief PI-D controller computation (Proportional & Integral on Error, Derivative on Measurement).
 * @param dref The reference setpoint.
 * @param dout The current measured feedback output.
 * @param control Pointer to the controller struct.
 * @return The computed control output.
 */
float pi_d(float dref, float dout, controller *control);

/**
 * @brief Clamps a given value between lower and upper bounds (saturation).
 * @param din The input value.
 * @param lb Lower bound.
 * @param ub Upper bound.
 * @return Clamped value.
 */
float satrn(float din, float lb, float ub);

/**
 * @brief Level trigger with hysteresis.
 * @param val Current value.
 * @param threshold Threshold to trigger at.
 * @param hysteresis Hysteresis width.
 * @param state Current integer state of the trigger (0 or 1).
 * @return New state (0 or 1).
 */
int level_trigger(float val, float threshold, float hysteresis, int state);

/**
 * @brief Watchdog 2-level window monitor.
 * @param val Current value.
 * @param lower_bound Lower valid limit.
 * @param upper_bound Upper valid limit.
 * @return 0 if within bounds, -1 if below lower_bound, 1 if above upper_bound.
 */
int window_monitor(float val, float lower_bound, float upper_bound);

#endif
