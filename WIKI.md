# sigLib Wiki: Mathematics and Workings

Welcome to the `sigLib` technical wiki. This document explains the underlying mathematics, numerical methods, and structural design used in the library's control loops and digital filters.

---

## 1. Control Systems

The control library utilizes a discrete-time approximation of the standard continuous Laplace-domain PID controller.

### 1.1 The Standard PID Algorithm
The continuous, ideal form of a PID controller is:
$$ u(t) = P \cdot e(t) + I \int_0^t e(\tau) d\tau + D \frac{de(t)}{dt} $$
Where:
- $u(t)$ is the control signal output.
- $e(t) = r(t) - y(t)$ is the error (Setpoint $r$ minus Measurement $y$).

In `sigLib`, this is discretized using **Backward Euler** for the derivative and **Rectangular Integration** for the integral, driven by the loop time step $\Delta t$ (`dt`):
$$ I_{sum}[k] = I_{sum}[k-1] + e[k] \cdot \Delta t $$
$$ \text{diff}[k] = \frac{e[k] - e[k-1]}{\Delta t} $$
$$ u[k] = P \cdot e[k] + I \cdot I_{sum}[k] + D \cdot \text{diff}[k] $$

### 1.2 Derivative on Measurement (PI-D)
A common problem with the standard PID is **Derivative Kick**. If the setpoint $r(t)$ changes as a step function, the derivative of the error approaches infinity, causing an aggressive spike in the control output. 

Assuming $r(t)$ is mostly constant, the derivative of the error is simply the negative derivative of the measurement:
$$ \frac{de(t)}{dt} = \frac{d(r(t) - y(t))}{dt} = -\frac{dy(t)}{dt} $$

The **PI-D** controller uses this substitution, computing the derivative solely on the feedback measurement $y[k]$:
$$ u[k] = P \cdot e[k] + I \cdot I_{sum}[k] - D \cdot \frac{y[k] - y[k-1]}{\Delta t} $$

### 1.3 Integral-Proportional-Derivative (I-PD)
Similar to derivative kick, **Proportional Kick** occurs when a setpoint change causes a massive instantaneous proportional jump. **I-PD** moves the proportional action to the measurement as well, leaving only the integrator to respond to the error $e[k]$. This ensures completely smooth control outputs regardless of aggressive setpoint changes.
$$ u[k] = -P \cdot y[k] + I \cdot I_{sum}[k] - D \cdot \frac{y[k] - y[k-1]}{\Delta t} $$

### 1.4 Integrator Anti-Windup
If the physical actuator saturates (e.g., a valve is 100% open but the temperature is still too low), the error remains positive. Without protection, $I_{sum}$ will continue to accumulate ("wind up") to infinity. When the error finally reverses, it takes a long time for the massive integral sum to unwind, causing massive overshoot.

`sigLib` prevents this by clamping the integrator state:
$$ I_{sum}[k] = \max(I_{min}, \min(I_{max}, I_{sum}[k])) $$
You configure these bounds via `ctrl_set_limits()`.

---

## 2. Signal Processing (Digital Filters)

The signal processing core is built around a generalized difference equation, allowing it to act as either an FIR (Finite Impulse Response) or IIR (Infinite Impulse Response) filter.

### 2.1 Direct Form I Difference Equation
The filter evaluates the standard linear constant-coefficient difference equation:
$$ y[n] = \sum_{i=0}^{M} b_i x[n-i] - \sum_{j=1}^{N} a_j y[n-j] $$
- $x[n-i]$ are the current and past inputs (Feedforward).
- $y[n-j]$ are the past outputs (Feedback).
- $b_i$ are the feedforward coefficients (Numerator).
- $a_j$ are the feedback coefficients (Denominator). *(Note: $a_0$ is always normalized to 1.0)*.

For a simple **FIR Moving Average** filter, you simply set $a_j = 0$, and populate $b_i$ with $\frac{1}{M+1}$.

### 2.2 Native Biquad Designer (Butterworth IIR)
`sigLib` includes an on-board coefficient calculator (`f_design_biquad`) that generates 2nd-order IIR Biquad filters. It specifically computes a **Butterworth** topology, which guarantees a maximally flat passband with no ripple.

The calculations are performed using the **Bilinear Transform** to map from the continuous $s$-domain to the discrete $z$-domain.

1. **Pre-warping the Frequency:**
   $$ \omega = 2 \pi \frac{f_c}{f_s} $$
   *(Where $f_c$ is the cutoff frequency and $f_s$ is the sampling frequency).*

2. **Damping Factor:**
   For a Butterworth filter, the Quality factor $Q = \frac{1}{\sqrt{2}}$.
   $$ \alpha = \frac{\sin(\omega)}{2 Q} = \frac{\sin(\omega)}{\sqrt{2}} $$

3. **Coefficient Calculation:**
   Based on the filter type, the unnormalized coefficients are derived:

   **Low-Pass Filter (LPF):**
   - $b_0 = \frac{1 - \cos(\omega)}{2}$
   - $b_1 = 1 - \cos(\omega)$
   - $b_2 = \frac{1 - \cos(\omega)}{2}$

   **High-Pass Filter (HPF):**
   - $b_0 = \frac{1 + \cos(\omega)}{2}$
   - $b_1 = -(1 + \cos(\omega))$
   - $b_2 = \frac{1 + \cos(\omega)}{2}$

   **Common Feedback Coefficients:**
   - $a_0 = 1 + \alpha$
   - $a_1 = -2 \cos(\omega)$
   - $a_2 = 1 - \alpha$

4. **Normalization:**
   To make $a_0 = 1$, all generated coefficients are divided by $a_0$:
   $$ b_i = \frac{b_i}{a_0}, \quad a_j = \frac{a_j}{a_0} $$

These coefficients are instantly loaded into the `filter` struct's circular buffers, ready for real-time processing via `f_refresh()`.

---

## 3. Triggers & Monitors

### 3.1 Level Trigger with Hysteresis
A simple comparative trigger `if (x > Threshold)` will toggle on and off rapidly if the signal $x$ contains noise. `level_trigger()` solves this using a stateful threshold loop (Hysteresis):
- If `State == 0`: It requires $x \ge \text{Threshold} + \text{Hysteresis}$ to turn `ON` (1).
- If `State == 1`: It requires $x \le \text{Threshold} - \text{Hysteresis}$ to turn `OFF` (0).
This creates a "dead-band" where noise cannot flip the state.

### 3.2 Watchdog Window Monitor
The window monitor evaluates safe operating boundaries. Given an interval $[L, U]$:
$$ \text{State} = \begin{cases} 
1 & \text{if } x > U \\
-1 & \text{if } x < L \\
0 & \text{if } L \le x \le U 
\end{cases} $$
This allows for discrete switch statements in user code to handle "over-voltage" vs "under-voltage" faults efficiently.
