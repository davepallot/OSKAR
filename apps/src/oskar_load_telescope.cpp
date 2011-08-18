/*
 * Copyright (c) 2011, The University of Oxford
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


#include "apps/oskar_load_telescope.h"
#include "utility/oskar_load_csv_coordinates_2d.h"
#include "interferometry/oskar_horizon_plane_to_itrs.h"
#include <stdlib.h>

void oskar_load_telescope(const char* file_path, oskar_TelescopeModel* telescope)
{
    double* x_temp;
    double* y_temp;
    unsigned n;
    oskar_load_csv_coordinates_2d(file_path, &n, &x_temp, &y_temp);

    telescope->num_antennas = n;
    size_t mem_size = n * sizeof(double);
    telescope->antenna_x = (double*)malloc(mem_size);
    telescope->antenna_y = (double*)malloc(mem_size);
    telescope->antenna_z = (double*)malloc(mem_size);

    // Convert horizon x, y coordinates to ITRS (local equatorial system)
    oskar_horizon_plane_to_itrs(n, x_temp, y_temp, telescope->latitude,
            telescope->antenna_x, telescope->antenna_y, telescope->antenna_z);

    free(x_temp);
    free(y_temp);
}
