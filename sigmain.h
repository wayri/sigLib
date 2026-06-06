/**
 * @file sigmain.h
 * @brief Digital filter implementations (FIR and IIR Biquad).
 */

#ifndef SIGMAIN_H
#define SIGMAIN_H

#include <stdint.h>

#define MAX_FILT_ORDER 10 /**< Maximum order for FIR/IIR filters */

typedef enum {
    FILTER_TYPE_LPF,
    FILTER_TYPE_HPF
} filter_type_t;

/**
 * @brief Filter structure using Direct Form I for IIR/FIR.
 */
typedef struct{
  float x_buf[MAX_FILT_ORDER]; /**< Circular buffer for input history */
  float y_buf[MAX_FILT_ORDER]; /**< Circular buffer for output history */
  
  float b[MAX_FILT_ORDER]; /**< Feedforward coefficients (Numerator) */
  float a[MAX_FILT_ORDER]; /**< Feedback coefficients (Denominator), a[0]=1.0 implicit */
  
  uint8_t order;           /**< Filter length (e.g., 3 for a 2nd order biquad) */
  uint8_t ind;             /**< Current index in circular buffer */

  float out;               /**< Latest filter output */
} filter;

/**
 * @brief Initializes the filter structure, clearing buffers and setting default pass-through coefficients.
 * @param f Pointer to the filter structure.
 * @param order Number of coefficients to use (e.g., 3 for biquad).
 */
void finit(filter *f, uint8_t order);

/**
 * @brief Sets custom coefficients for the filter (FIR or IIR).
 * @param f Pointer to the filter structure.
 * @param b Array of feedforward coefficients.
 * @param a Array of feedback coefficients (can be NULL for FIR).
 */
void f_set_coeffs(filter *f, const float *b, const float *a);

/**
 * @brief Computes biquad filter coefficients based on design parameters (Butterworth 2nd Order).
 * @param f Pointer to the filter structure.
 * @param type FILTER_TYPE_LPF or FILTER_TYPE_HPF.
 * @param cutoff_freq Cutoff frequency in Hz.
 * @param sample_freq Sampling frequency in Hz.
 */
void f_design_biquad(filter *f, filter_type_t type, float cutoff_freq, float sample_freq);

/**
 * @brief Ingests a new sample and computes the next filter output via Difference Equation.
 * @param f Pointer to the filter structure.
 * @param in The new incoming data sample.
 * @return The computed filtered output.
 */
float f_refresh(filter *f, float in);

#endif
