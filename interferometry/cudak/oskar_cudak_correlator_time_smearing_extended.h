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

#ifndef OSKAR_CUDAK_CORRELATOR_TIME_SMEARING_EXTENDED_H_
#define OSKAR_CUDAK_CORRELATOR_TIME_SMEARING_EXTENDED_H_

/**
 * @file oskar_cudak_correlator_time_smearing_extended.h
 */

#include "oskar_global.h"
#include "utility/oskar_vector_types.h"

/**
 * @brief
 * Correlator kernel for extended gaussian sources.
 *
 * @details
 * Gaussian parameters a, b, and c are assumed to be evaluated when the
 * sky model is loaded.
 *
 * Note that the station x, y, z coordinates must be in the ECEF frame.
 *
 * @param[in] num_sources   Number of sources.
 * @param[in] num_stations  Number of stations.
 * @param[in] jones         Matrix of Jones matrices to correlate.
 * @param[in] source_I      Source Stokes I values, in Jy.
 * @param[in] source_Q      Source Stokes Q values, in Jy.
 * @param[in] source_U      Source Stokes U values, in Jy.
 * @param[in] source_V      Source Stokes V values, in Jy.
 * @param[in] source_l      Source l-direction cosines from phase centre.
 * @param[in] source_m      Source m-direction cosines from phase centre.
 * @param[in] source_n      Source n-direction cosines from phase centre.
 * @param[in] source_a      Source Gaussian parameter a.
 * @param[in] source_b      Source Gaussian parameter b.
 * @param[in] source_c      Source Gaussian parameter c.
 * @param[in] station_u     Station u-coordinates multiplied by the wavenumber.
 * @param[in] station_v     Station v-coordinates multiplied by the wavenumber.
 * @param[in] station_x     Station x-coordinates multiplied by the wavenumber.
 * @param[in] station_y     Station y-coordinates multiplied by the wavenumber.
 * @param[in] freq_hz       Frequency, in Hz.
 * @param[in] bandwidth_hz  Channel bandwidth, in Hz.
 * @param[in] time_int_sec  Time averaging interval, in seconds.
 * @param[in] gha0_rad      Greenwich Hour Angle of phase centre, in radians.
 * @param[in] dec0_rad      Declination of phase centre, in radians.
 * @param[in,out] vis       Modified output complex visibilities.
 */
__global__
void oskar_cudak_correlator_time_smearing_extended_f(const int num_sources,
        const int num_stations, const float4c* jones, const float* source_I,
        const float* source_Q, const float* source_U, const float* source_V,
        const float* source_l, const float* source_m, const float* source_n,
        const float* source_a, const float* source_b, const float* source_c,
        const float* station_u, const float* station_v,
        const float* station_x, const float* station_y, const float freq_hz,
        const float bandwidth_hz, const float time_int_sec,
        const float gha0_rad, const float dec0_rad, float4c* vis);

/**
 * @brief
 * Correlator kernel for extended gaussian sources.
 *
 * @details
 * Gaussian parameters a,b, and c are assumed to be evaluated when the
 * sky model is loaded.
 *
 * @param[in] num_sources   Number of sources.
 * @param[in] num_stations  Number of stations.
 * @param[in] jones         Matrix of Jones matrices to correlate.
 * @param[in] source_I      Source Stokes I values, in Jy.
 * @param[in] source_Q      Source Stokes Q values, in Jy.
 * @param[in] source_U      Source Stokes U values, in Jy.
 * @param[in] source_V      Source Stokes V values, in Jy.
 * @param[in] source_l      Source l-direction cosines from phase centre.
 * @param[in] source_m      Source m-direction cosines from phase centre.
 * @param[in] source_n      Source n-direction cosines from phase centre.
 * @param[in] source_a      Source Gaussian parameter a.
 * @param[in] source_b      Source Gaussian parameter b.
 * @param[in] source_c      Source Gaussian parameter c.
 * @param[in] station_u     Station u-coordinates multiplied by the wavenumber.
 * @param[in] station_v     Station v-coordinates multiplied by the wavenumber.
 * @param[in] station_x     Station x-coordinates multiplied by the wavenumber.
 * @param[in] station_y     Station y-coordinates multiplied by the wavenumber.
 * @param[in] freq_hz       Frequency, in Hz.
 * @param[in] bandwidth_hz  Channel bandwidth, in Hz.
 * @param[in] time_int_sec  Time averaging interval, in seconds.
 * @param[in] gha0_rad      Greenwich Hour Angle of phase centre, in radians.
 * @param[in] dec0_rad      Declination of phase centre, in radians.
 * @param[in,out] vis       Modified output complex visibilities.
 */
__global__
void oskar_cudak_correlator_time_smearing_extended_d(const int num_sources,
        const int num_stations, const double4c* jones, const double* source_I,
        const double* source_Q, const double* source_U, const double* source_V,
        const double* source_l, const double* source_m, const double* source_n,
        const double* source_a, const double* source_b, const double* source_c,
        const double* station_u, const double* station_v,
        const double* station_x, const double* station_y, const double freq_hz,
        const double bandwidth_hz, const double time_int_sec,
        const double gha0_rad, const double dec0_rad, double4c* vis);

#endif /* OSKAR_CUDAK_CORRELATOR_TIME_SMEARING_EXTENDED_H_ */