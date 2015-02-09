/*
 * Copyright (c) 2015, The University of Oxford
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

#include "apps/lib/oskar_set_up_vis.h"
#include <oskar_cmath.h>

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void oskar_set_up_vis(const oskar_Settings* settings,
        const oskar_Telescope* tel, oskar_VisHeader** hdr,
        oskar_VisBlock** blk, int* status)
{
    int i, max_times_per_block, num_stations, num_channels;
    int precision, vis_type;
    const double rad2deg = 180.0/M_PI;

    /* Check all inputs. */
    if (!settings || !tel || !hdr || !blk || !status)
    {
        oskar_set_invalid_argument(status);
        return;
    }

    /* Check if safe to proceed. */
    if (*status) return;

    /* Create visibility header and data block. */
    max_times_per_block = settings->interferometer.max_time_samples_per_block;
    num_channels = settings->obs.num_channels;
    num_stations = oskar_telescope_num_stations(tel);
    precision = settings->sim.double_precision ? OSKAR_DOUBLE : OSKAR_SINGLE;
    vis_type = precision | OSKAR_COMPLEX;
    if (oskar_telescope_pol_mode(tel) == OSKAR_POL_MODE_FULL)
        vis_type |= OSKAR_MATRIX;
    *hdr = oskar_vis_header_create(vis_type, max_times_per_block,
            settings->obs.num_time_steps, num_channels, num_stations, status);
    *blk = oskar_vis_block_create(vis_type, OSKAR_CPU, max_times_per_block,
            num_channels, num_stations, status);

    /* Add metadata from settings. */
    oskar_vis_header_set_freq_start_hz(*hdr,
            settings->obs.start_frequency_hz);
    oskar_vis_header_set_freq_inc_hz(*hdr,
            settings->obs.frequency_inc_hz);
    oskar_vis_header_set_time_start_mjd_utc(*hdr,
            settings->obs.start_mjd_utc);
    oskar_vis_header_set_time_inc_sec(*hdr,
            settings->obs.dt_dump_days * 86400.0);

    /* Add telescope model path. */
    oskar_mem_append_raw(oskar_vis_header_telescope_path(*hdr),
            settings->telescope.input_directory, OSKAR_CHAR, OSKAR_CPU,
            1 + strlen(settings->telescope.input_directory), status);

    /* Add settings file contents. */
    {
        oskar_Mem* temp;
        temp = oskar_mem_read_binary_raw(settings->settings_path,
                OSKAR_CHAR, OSKAR_CPU, status);
        oskar_mem_copy(oskar_vis_header_settings(*hdr), temp, status);
        oskar_mem_free(temp, status);
    }

    /* Copy other metadata from telescope model. */
    oskar_vis_header_set_time_average_sec(*hdr,
            oskar_telescope_time_average_sec(tel));
    oskar_vis_header_set_channel_bandwidth_hz(*hdr,
            oskar_telescope_channel_bandwidth_hz(tel));
    oskar_vis_header_set_phase_centre(*hdr,
            oskar_telescope_phase_centre_ra_rad(tel) * rad2deg,
            oskar_telescope_phase_centre_dec_rad(tel) * rad2deg);
    oskar_vis_header_set_telescope_centre(*hdr,
            oskar_telescope_lon_rad(tel) * rad2deg,
            oskar_telescope_lat_rad(tel) * rad2deg,
            oskar_telescope_alt_metres(tel));
    oskar_mem_copy(oskar_vis_header_station_x_offset_ecef_metres(*hdr),
            oskar_telescope_station_true_x_offset_ecef_metres_const(tel),
            status);
    oskar_mem_copy(oskar_vis_header_station_y_offset_ecef_metres(*hdr),
            oskar_telescope_station_true_y_offset_ecef_metres_const(tel),
            status);
    oskar_mem_copy(oskar_vis_header_station_z_offset_ecef_metres(*hdr),
            oskar_telescope_station_true_z_offset_ecef_metres_const(tel),
            status);
}

#ifdef __cplusplus
}
#endif