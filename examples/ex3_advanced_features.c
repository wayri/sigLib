#include <stdio.h>
#include "../control.h"

int main() {
    controller sys_pid;
    ctrl_init(&sys_pid, 1.5f, 0.2f, 0.05f, 0.01f);
    
    // 1. Anti-windup: Clamp the integral term between -50 and 50
    ctrl_set_limits(&sys_pid, 50.0f, -50.0f);
    
    float system_temp = 20.0f;
    float setpoint = 100.0f;
    int heater_state = 0; // 0 = off, 1 = on
    
    printf("Starting Advanced Control Simulation...\n");
    
    for (int step = 0; step < 25; step++) {
        // 2. PID control with anti-windup
        float ctrl_out = pid(setpoint - system_temp, &sys_pid);
        ctrl_out = satrn(ctrl_out, 0.0f, 100.0f); // Heater power 0-100%
        
        // Simulate system heating up
        system_temp += ctrl_out * 0.05f;
        
        // 3. Level Trigger: Turn on a warning LED if temp crosses 90, off if it drops below 85 
        // (Threshold = 87.5, Hysteresis = 2.5)
        heater_state = level_trigger(system_temp, 87.5f, 2.5f, heater_state);
        
        // 4. Window Monitor: Check if temperature is in safe operating zone [0, 110]
        int safety_status = window_monitor(system_temp, 0.0f, 110.0f);
        
        printf("Step %2d | Temp: %6.2f | Ctrl Out: %6.2f | Warning LED: %s | Safety: %s\n", 
               step, system_temp, ctrl_out, 
               heater_state ? "ON " : "OFF", 
               safety_status == 0 ? "OK" : (safety_status > 0 ? "OVER_TEMP" : "UNDER_TEMP"));
    }

    return 0;
}
