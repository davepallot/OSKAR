/*
 * Copyright (c) 2012, The University of Oxford
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the University of Oxford nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "station/cudak/oskar_cudak_evaluate_dipole_pattern.h"

#define M_2_SQRT_2LN2  2.35482004503094938202
#define M_2_SQRT_2LN2f 2.35482004503094938202f

// Single precision.
__global__
void oskar_cudak_evaluate_dipole_pattern_f(const int num_sources,
        const float* theta, const float* phi, const int cos_power,
        const float gaussian_fwhm_rad, const int return_x_dipole,
        float4c* pattern)
{
    // Source index being processed by the thread.
    const int s = blockIdx.x * blockDim.x + threadIdx.x;
    if (s >= num_sources) return;

    // Get (modified) source vector components, relative to a dipole along x.
    float theta_c, sin_phi, cos_phi, cos_theta;
    theta_c = theta[s];
    cos_theta = cosf(theta_c);
    sincosf(phi[s], &sin_phi, &cos_phi);

    // Evaluate vectors e_theta and e_phi in x-direction at source position.
    float e_theta = cos_theta * cos_phi;
    float e_phi = -sin_phi;

    // Modify by cosine tapering function if required.
    if (cos_power > 0)
    {
    	float f;
    	f = (cos_power > 1) ? powf(cos_theta, cos_power) : cos_theta;
    	e_theta *= f;
    	e_phi *= f;
    }

    // Modify by Gaussian tapering function if required.
    if (gaussian_fwhm_rad > 0.0f)
    {
    	float sigma, f;
    	sigma = gaussian_fwhm_rad / M_2_SQRT_2LN2f;
    	f = expf(theta_c * theta_c / (2.0f * sigma * sigma));
    	e_theta *= f;
    	e_phi *= f;
    }

    // Store components.
    if (return_x_dipole)
    {
        pattern[s].a.x = e_theta;
        pattern[s].a.y = 0.0f;
        pattern[s].b.x = e_phi;
        pattern[s].b.y = 0.0f;
    }
    else
    {
        pattern[s].c.x = e_theta;
        pattern[s].c.y = 0.0f;
        pattern[s].d.x = e_phi;
        pattern[s].d.y = 0.0f;
    }
}

// Double precision.
__global__
void oskar_cudak_evaluate_dipole_pattern_d(const int num_sources,
        const double* theta, const double* phi, const int cos_power,
        const double gaussian_fwhm_rad, const int return_x_dipole,
        double4c* pattern)
{
    // Source index being processed by the thread.
    const int s = blockIdx.x * blockDim.x + threadIdx.x;
    if (s >= num_sources) return;

    // Get (modified) source vector components, relative to a dipole along x.
    double theta_c, sin_phi, cos_phi, cos_theta;
    theta_c = theta[s];
    cos_theta = cos(theta_c);
    sincos(phi[s], &sin_phi, &cos_phi);

    // Evaluate vectors e_theta and e_phi in x-direction at source position.
    double e_theta = cos_theta * cos_phi;
    double e_phi = -sin_phi;

    // Modify by cosine tapering function if required.
    if (cos_power > 0)
    {
    	double f;
    	f = (cos_power > 1) ? pow(cos_theta, cos_power) : cos_theta;
    	e_theta *= f;
    	e_phi *= f;
    }

    // Modify by Gaussian tapering function if required.
    if (gaussian_fwhm_rad > 0.0)
    {
    	double sigma, f;
    	sigma = gaussian_fwhm_rad / M_2_SQRT_2LN2;
    	f = exp(theta_c * theta_c / (2.0 * sigma * sigma));
    	e_theta *= f;
    	e_phi *= f;
    }

    // Store components.
    if (return_x_dipole)
    {
        pattern[s].a.x = e_theta;
        pattern[s].a.y = 0.0;
        pattern[s].b.x = e_phi;
        pattern[s].b.y = 0.0;
    }
    else
    {
        pattern[s].c.x = e_theta;
        pattern[s].c.y = 0.0;
        pattern[s].d.x = e_phi;
        pattern[s].d.y = 0.0;
    }
}


// DEPRECATED

// Single precision.
__global__
void oskar_cudak_evaluate_dipole_pattern_f(const int num_sources,
        const float* l, const float* m, const float* n,
        const float cos_orientation_x, const float sin_orientation_x,
        const float cos_orientation_y, const float sin_orientation_y,
        float4c* pattern)
{
    // Source index being processed by the thread.
    const int s = blockIdx.x * blockDim.x + threadIdx.x;
    if (s >= num_sources) return;

    // Get source direction cosines.
    const float ll = l[s]; // Component along x-axis.
    const float lm = m[s]; // Component along y-axis.
    const float ln = n[s]; // Component along z-axis.

    // Evaluate phi, the angle (co-azimuth) from East (x) towards North (y).
    const float phi = atan2f(lm, ll);
    float sin_phi, cos_phi;
    sincosf(phi, &sin_phi, &cos_phi); // Cannot use direction cosines here.

    // Evaluate unit vectors e_theta and e_phi at source position.
    // cos_theta = ln
    const float e_theta_x = ln * cos_phi; // Component of e_theta in x.
    const float e_theta_y = ln * sin_phi; // Component of e_theta in y.
    // e_phi_x = -sin_phi;
    // e_phi_y = cos_phi;

    // Dot products:
    // g_phi_a   = a_x * e_phi_x   + a_y * e_phi_y;
    // g_theta_a = a_x * e_theta_x + a_y * e_theta_y;
    // g_phi_b   = b_x * e_phi_x   + b_y * e_phi_y;
    // g_theta_b = b_x * e_theta_x + b_y * e_theta_y;
    const float g_phi_a   = sin_orientation_x * -sin_phi
            + cos_orientation_x * cos_phi;
    const float g_theta_a = sin_orientation_x * e_theta_x
            + cos_orientation_x * e_theta_y;
    const float g_phi_b   = sin_orientation_y * -sin_phi
            + cos_orientation_y * cos_phi;
    const float g_theta_b = sin_orientation_y * e_theta_x
            + cos_orientation_y * e_theta_y;

    // Store components.
    pattern[s].a.x = g_theta_a;
    pattern[s].a.y = 0.0f;
    pattern[s].b.x = g_phi_a;
    pattern[s].b.y = 0.0f;
    pattern[s].c.x = g_theta_b;
    pattern[s].c.y = 0.0f;
    pattern[s].d.x = g_phi_b;
    pattern[s].d.y = 0.0f;
}

// Double precision.
__global__
void oskar_cudak_evaluate_dipole_pattern_d(const int num_sources,
        const double* l, const double* m, const double* n,
        const double cos_orientation_x, const double sin_orientation_x,
        const double cos_orientation_y, const double sin_orientation_y,
        double4c* pattern)
{
    // Source index being processed by the thread.
    const int s = blockIdx.x * blockDim.x + threadIdx.x;
    if (s >= num_sources) return;

    // Get source direction cosines.
    const double ll = l[s]; // Component along x-axis.
    const double lm = m[s]; // Component along y-axis.
    const double ln = n[s]; // Component along z-axis.

    // Evaluate phi, the angle (co-azimuth) from East (x) towards North (y).
    const double phi = atan2(lm, ll);
    double sin_phi, cos_phi;
    sincos(phi, &sin_phi, &cos_phi); // Cannot use direction cosines here.

    // Evaluate unit vectors e_theta and e_phi at source position.
    // cos_theta = ln
    const double e_theta_x = ln * cos_phi; // Component of e_theta in x.
    const double e_theta_y = ln * sin_phi; // Component of e_theta in y.
    // e_phi_x = -sin_phi;
    // e_phi_y = cos_phi;

    // Dot products:
    // g_phi_a   = a_x * e_phi_x   + a_y * e_phi_y;
    // g_theta_a = a_x * e_theta_x + a_y * e_theta_y;
    // g_phi_b   = b_x * e_phi_x   + b_y * e_phi_y;
    // g_theta_b = b_x * e_theta_x + b_y * e_theta_y;
    const double g_phi_a   = sin_orientation_x * -sin_phi
            + cos_orientation_x * cos_phi;
    const double g_theta_a = sin_orientation_x * e_theta_x
            + cos_orientation_x * e_theta_y;
    const double g_phi_b   = sin_orientation_y * -sin_phi
            + cos_orientation_y * cos_phi;
    const double g_theta_b = sin_orientation_y * e_theta_x
            + cos_orientation_y * e_theta_y;

    // Store components.
    pattern[s].a.x = g_theta_a;
    pattern[s].a.y = 0.0;
    pattern[s].b.x = g_phi_a;
    pattern[s].b.y = 0.0;
    pattern[s].c.x = g_theta_b;
    pattern[s].c.y = 0.0;
    pattern[s].d.x = g_phi_b;
    pattern[s].d.y = 0.0;
}
