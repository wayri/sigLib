#include "control.h"

void ctrl_init(controller *control, float p, float i, float d, float dt) {
    control->P = p;
    control->I = i;
    control->D = d;
    control->dt = dt;
    control->integral = 0.0f;
    control->prev_val = 0.0f;
    
    // Default anti-windup to essentially no limits
    control->int_max = 1e9f;
    control->int_min = -1e9f;
}

void ctrl_set_limits(controller *control, float int_max, float int_min) {
    control->int_max = int_max;
    control->int_min = int_min;
}

// Helper for anti-windup
static void apply_antiwindup(controller *control) {
    if (control->integral > control->int_max) {
        control->integral = control->int_max;
    } else if (control->integral < control->int_min) {
        control->integral = control->int_min;
    }
}

float pid(float din, controller *control) {
    float diff;
    
    control->integral += din * control->dt;
    apply_antiwindup(control);

    diff = (din - control->prev_val) / control->dt;
    control->prev_val = din;

    return control->P * din + control->I * control->integral + control->D * diff;
}

float pi(float din, controller *control) {
    control->integral += din * control->dt;
    apply_antiwindup(control);

    return control->P * din + control->I * control->integral;
}

float i_pd(float dref, float dout, controller *control) {
    float err = dref - dout;
    float diff;

    control->integral += err * control->dt;
    apply_antiwindup(control);

    diff = (dout - control->prev_val) / control->dt;
    control->prev_val = dout;

    return -control->P * dout + control->I * control->integral - control->D * diff;
}

float pi_d(float dref, float dout, controller *control) {
    float err = dref - dout;
    float diff;

    control->integral += err * control->dt;
    apply_antiwindup(control);

    diff = (dout - control->prev_val) / control->dt;
    control->prev_val = dout;

    return control->P * err + control->I * control->integral - control->D * diff;
}

float satrn(float din, float lb, float ub) {
    return din > lb ? (din < ub ? din : ub) : lb;
}

int level_trigger(float val, float threshold, float hysteresis, int state) {
    if (state == 0) {
        if (val >= threshold + hysteresis) {
            return 1;
        }
    } else {
        if (val <= threshold - hysteresis) {
            return 0;
        }
    }
    return state;
}

int window_monitor(float val, float lower_bound, float upper_bound) {
    if (val < lower_bound) {
        return -1; // Under fault
    } else if (val > upper_bound) {
        return 1;  // Over fault
    }
    return 0; // OK
}
