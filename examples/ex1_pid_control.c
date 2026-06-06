#include <stdio.h>
#include "../control.h"

int main() {
    // 1. Declare the controller
    controller my_pid;
    
    // 2. Initialize with P=2.0, I=0.5, D=0.1, dt=0.01s
    ctrl_init(&my_pid, 2.0f, 0.5f, 0.1f, 0.01f);
    
    float setpoint = 100.0f;
    float current_value = 0.0f; // Represents the system's current state

    printf("Starting PI-D control simulation...\n");
    
    // Simulate 20 control steps
    for (int step = 0; step < 20; step++) {
        // 3. Compute PI-D output (PI on error, D on measurement)
        float ctrl_output = pi_d(setpoint, current_value, &my_pid);
        
        // 4. Apply saturation (e.g., motor PWM limited to +/- 50.0)
        ctrl_output = satrn(ctrl_output, -50.0f, 50.0f);
        
        // (Simulate the system reacting to the control output)
        current_value += ctrl_output * 0.1f; // Simple linear plant model
        
        printf("Step %2d | Setpoint: %6.1f | Current: %6.2f | Ctrl Out: %6.2f\n", 
               step, setpoint, current_value, ctrl_output);
    }

    return 0;
}
