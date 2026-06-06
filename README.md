# sigLib
![License](https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-blue.svg)

`sigLib` is a lightweight, pure C library tailored for digital signal processing and control systems. It is designed to be highly portable, reentrant, and simple to integrate into embedded systems (like microcontrollers) or PC-side simulations.

## ✨ Features

### 🎛️ Control Systems
- **PID Controller**: Standard implementation of Proportional, Integral, and Derivative control.
- **PI Controller**: Fast, simplified PI loop.
- **I-PD & PI-D Controllers**: Advanced variants that prevent *setpoint kick* on the derivative and proportional terms.
- **Anti-Windup**: Native integrator clamping to prevent windup during saturation.
- **Saturation (`satrn`)**: Clamps signals to physical bounds.
- **Level Trigger**: Integer state trigger with tunable hysteresis for noisy signals.
- **Watchdog Window Monitor**: 2-level window fault detection (Under-fault, OK, Over-fault).

### 📶 Signal Processing
- **Universal Filter Engine**: A robust Direct Form I difference equation engine that can execute both **FIR** (Feedforward) and **IIR** (Feedback) filters.
- **Native Biquad Designer**: Automatically compute 2nd-order Butterworth coefficients (`a` and `b` arrays) on the fly for Low-Pass (LPF) and High-Pass (HPF) filters using your cutoff and sampling frequencies.

---

## 🚀 Getting Started

### 1. Control Systems Example

Instantiate a controller and set its limits. Because `sigLib` uses a struct-based state (`controller`), it is 100% reentrant. You can run 50 PID loops concurrently just by creating 50 structs!

```c
#include "control.h"

controller my_pid;
ctrl_init(&my_pid, 2.0f, 0.5f, 0.1f, 0.01f); // P, I, D, dt

// Prevent integral windup
ctrl_set_limits(&my_pid, 50.0f, -50.0f);

// In your control loop:
float error = setpoint - sensor_value;
float output = pid(error, &my_pid);
output = satrn(output, 0.0f, 100.0f); // Clamp to 0-100% output
```

### 2. Native Biquad Filter Design

You don't need MATLAB to get filter coefficients anymore. `sigLib` can design them directly:

```c
#include "sigmain.h"

filter lpf;
finit(&lpf, 3); // 3 coefficients for a 2nd order biquad

// Design a Low-Pass Filter: 10 Hz cutoff, 100 Hz sample rate
f_design_biquad(&lpf, FILTER_TYPE_LPF, 10.0f, 100.0f);

// In your sampling loop:
float clean_data = f_refresh(&lpf, noisy_sensor_data);
```

### 3. Safety & Triggers

Easily monitor variables without writing complex state machines:

```c
// Trigger a warning state if temp hits 90, but don't untrigger until it cools to 85.
// (Threshold = 87.5, Hysteresis = 2.5)
warning_state = level_trigger(temperature, 87.5f, 2.5f, warning_state);

// Ensure a voltage rail stays between 4.5v and 5.5v
int status = window_monitor(voltage, 4.5f, 5.5f);
if (status == 1) printf("Overvoltage!");
if (status == -1) printf("Undervoltage!");
```

---

## 📂 Examples Directory
Check the `examples/` folder for compilable C programs demonstrating the library:
- `ex1_pid_control.c` - Basic PI-D loop with system simulation.
- `ex2_filter.c` - Basic FIR moving average implementation.
- `ex3_advanced_features.c` - Utilizing Anti-Windup, Level Triggers, and Window Monitors.
- `ex4_biquad_filter.c` - Designing and running an IIR Biquad Low-Pass Filter on noisy sine waves.

*Note: To compile the filter examples, link the math library (`-lm`).*

---

## ⚖️ Licensing 

This library is provided as-is without any warranty, guarantee or liability. Author is not responsible for the use of this code for any purpose, anywhere. However, the code here is provided under non-commercial usage terms. You as a user are permitted to use this code, make derivatives but proper accreditation is required along with inclusion of this license. 

Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
