/*
 * Copyright (c) 2011-2014, The University of Oxford
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

#ifndef OSKAR_EVALUATE_JONES_E_H_
#define OSKAR_EVALUATE_JONES_E_H_

/**
 * @file oskar_evaluate_jones_E.h
 */

#include <oskar_global.h>
#include <oskar_telescope.h>
#include <oskar_jones.h>
#include <oskar_sky.h>
#include <oskar_station_work.h>
#include <oskar_random_state.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Evaluates a set of E-Jones matrices for a number of stations
 * and source directions.
 *
 * @details
 * Note: Processing in this function is performed on the GPU and therefore
 * the memory passed to and from this function must be allocated on the
 * GPU.
 *
 * @param[out] E            Output set of Jones matrices.
 * @param[in]  num_sources  Number of sources to use from the sky model.
 * @param[in]  sky          Input sky model.
 * @param[in]  telescope    Input telescope model.
 * @param[in]  gast         The Greenwich Apparent Sidereal Time, in radians.
 * @param[in]  frequency_hz The observing frequency, in Hz.
 * @param[in]  work         Pointer to structure holding work arrays.
 * @param[in]  random_state Structure holding curand states.
 * @param[in,out] status    Status return code.
 */
OSKAR_EXPORT
void oskar_evaluate_jones_E(oskar_Jones* E, int num_sources, oskar_Sky* sky,
        const oskar_Telescope* telescope, double gast, double frequency_hz,
        oskar_StationWork* work, oskar_RandomState* random_state, int* status);

#ifdef __cplusplus
}
#endif

#endif /* OSKAR_EVALUATE_JONES_E_H_ */