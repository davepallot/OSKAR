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

#include <cuda_runtime_api.h>
#include "math/oskar_jones_join.h"
#include "math/oskar_cuda_jones_mul_c2.h"
#include "math/oskar_cuda_jones_mul_mat1_c1.h"
#include "math/oskar_cuda_jones_mul_mat2.h"
#include "math/oskar_cuda_jones_mul_scalar_c2.h"
#include "utility/oskar_mem_element_size.h"

extern "C"
int oskar_jones_join(oskar_Jones* j3, oskar_Jones* j1, const oskar_Jones* j2)
{
    // Check to see if the output structure J3 exists: if not, set it to J1.
    if (j3 == NULL) j3 = j1;

    // Check that all pointers are not NULL.
    if (j1 == NULL) return OSKAR_ERR_INVALID_ARGUMENT;
    if (j2 == NULL) return OSKAR_ERR_INVALID_ARGUMENT;
    if (j1->ptr.data == NULL) return OSKAR_ERR_MEMORY_NOT_ALLOCATED;
    if (j2->ptr.data == NULL) return OSKAR_ERR_MEMORY_NOT_ALLOCATED;
    if (j3->ptr.data == NULL) return OSKAR_ERR_MEMORY_NOT_ALLOCATED;

    // Get the dimensions of the input data.
    int n_sources1 = j1->num_sources();
    int n_sources2 = j2->num_sources();
    int n_sources3 = j3->num_sources();
    int n_stations1 = j1->num_stations();
    int n_stations2 = j2->num_stations();
    int n_stations3 = j3->num_stations();

    // Check the data dimensions.
    if (n_sources1 != n_sources2 || n_sources1 != n_sources3)
        return OSKAR_ERR_DIMENSION_MISMATCH;
    if (n_stations1 != n_stations2 || n_stations1 != n_stations3)
        return OSKAR_ERR_DIMENSION_MISMATCH;

    // Figure out what we've been given.
    int type1 = j1->type();
    int type2 = j2->type();
    int type3 = j3->type();
    int location1 = j1->location();
    int location2 = j2->location();
    int location3 = j3->location();

    // Check that there is enough memory to hold the result.
    size_t size1 = oskar_mem_element_size(type1);
    size_t size2 = oskar_mem_element_size(type2);
    size_t size3 = oskar_mem_element_size(type3);
    if (size3 < size2 || size3 < size1)
        return OSKAR_ERR_TYPE_MISMATCH;

    // Copy data to GPU if required.
    int n_elements = n_sources1 * n_stations1;
    const oskar_Jones* hd1 = (location1 == OSKAR_LOCATION_CPU) ?
    		new oskar_Jones(j1, OSKAR_LOCATION_GPU) : j1;
    const oskar_Jones* hd2 = (location2 == OSKAR_LOCATION_CPU) ?
    		new oskar_Jones(j2, OSKAR_LOCATION_GPU) : j2;
    oskar_Jones* hd3 = (location3 == OSKAR_LOCATION_CPU) ?
    		new oskar_Jones(j3, OSKAR_LOCATION_GPU) : j3;
    const void* d1 = hd1->ptr.data;
    const void* d2 = hd2->ptr.data;
    void* d3 = hd3->ptr.data;

    // Check for errors.
    int err = cudaPeekAtLastError();
    if (err) goto stop;

    // Set error code to type mismatch by default.
    err = OSKAR_ERR_TYPE_MISMATCH;

    // Multiply the matrices.
    if (type1 == OSKAR_SINGLE_COMPLEX)
    {
        if (type2 == OSKAR_SINGLE_COMPLEX)
        {
            if (type3 == OSKAR_SINGLE_COMPLEX)
            {
                // Scalar-scalar to scalar, float: OK.
                err = oskar_cuda_jones_mul_scalar_c2_f(n_elements,
                        (const float2*)d1, (const float2*)d2, (float2*)d3);
            }
            else if (type3 == OSKAR_SINGLE_COMPLEX_MATRIX)
            {
                // Scalar-scalar to matrix, float: OK.
                err = oskar_cuda_jones_mul_c2_f(n_elements,
                        (const float2*)d1, (const float2*)d2, (float4c*)d3);
            }
        }
        else if (type2 == OSKAR_SINGLE_COMPLEX_MATRIX)
        {
            // Scalar-matrix, float: OK.
            err = oskar_cuda_jones_mul_mat1_c1_f(n_elements,
                    (const float4c*)d2, (const float2*)d1, (float4c*)d3);
        }
    }
    else if (type1 == OSKAR_SINGLE_COMPLEX_MATRIX)
    {
        if (type2 == OSKAR_SINGLE_COMPLEX)
        {
            // Matrix-scalar, float: OK.
            err = oskar_cuda_jones_mul_mat1_c1_f(n_elements,
                    (const float4c*)d1, (const float2*)d2, (float4c*)d3);
        }
        else if (type2 == OSKAR_SINGLE_COMPLEX_MATRIX)
        {
            // Matrix-matrix, float: OK.
            err = oskar_cuda_jones_mul_mat2_f(n_elements,
                    (const float4c*)d1, (const float4c*)d2, (float4c*)d3);
        }
    }
    else if (type1 == OSKAR_DOUBLE_COMPLEX)
    {
        if (type2 == OSKAR_DOUBLE_COMPLEX)
        {
            if (type3 == OSKAR_DOUBLE_COMPLEX)
            {
                // Scalar-scalar to scalar, double: OK.
                err = oskar_cuda_jones_mul_scalar_c2_d(n_elements,
                        (const double2*)d1, (const double2*)d2, (double2*)d3);
            }
            else if (type3 == OSKAR_DOUBLE_COMPLEX_MATRIX)
            {
                // Scalar-scalar to matrix, double: OK.
                err = oskar_cuda_jones_mul_c2_d(n_elements,
                        (const double2*)d1, (const double2*)d2, (double4c*)d3);
            }
        }
        else if (type2 == OSKAR_DOUBLE_COMPLEX_MATRIX)
        {
            // Scalar-matrix, double: OK.
            err = oskar_cuda_jones_mul_mat1_c1_d(n_elements,
                    (const double4c*)d2, (const double2*)d1, (double4c*)d3);
        }
    }
    else if (type1 == OSKAR_DOUBLE_COMPLEX_MATRIX)
    {
        if (type2 == OSKAR_DOUBLE_COMPLEX)
        {
            // Matrix-scalar, double: OK.
            err = oskar_cuda_jones_mul_mat1_c1_d(n_elements,
                    (const double4c*)d1, (const double2*)d2, (double4c*)d3);
        }
        else if (type2 == OSKAR_DOUBLE_COMPLEX_MATRIX)
        {
            // Matrix-matrix, double: OK.
            err = oskar_cuda_jones_mul_mat2_d (n_elements,
                    (const double4c*)d1, (const double4c*)d2, (double4c*)d3);
        }
    }

stop:
    // Free GPU memory if input data was on the host.
    if (location1 == OSKAR_LOCATION_CPU) delete hd1;
    if (location2 == OSKAR_LOCATION_CPU) delete hd2;

    // Copy result back to host memory if required.
    if (location3 == OSKAR_LOCATION_CPU)
    {
        if (err == 0)
            err = hd3->copy_to(j3);
        delete hd3;
    }
    return err;
}
