/*
 * miranda
 * Copyright (C) 2014-2020 Andrea Nardinocchi (andrea@nardinan.it)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef miranda_local_math_h
#define miranda_local_math_h
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define d_math_two_pi 6.28318530718
#define d_math_pi 3.14159265359
#define d_math_half_pi 1.57079632679
#define d_math_default_precision 0.1
#define d_math_radians_conversion 0.0174533
#define d_math_degrees_conversion 57.29577951307855
#define d_math_frand (((double)(rand()%1000))/1000.0)
#define d_math_square(a) ((a)*(a))
#define d_math_min(a, b) (((a)<(b))?(a):(b))
#define d_math_max(a, b) (((a)<(b))?(b):(a))
#define d_math_double_different(a, b) (fabs(a-b)>0.000001)
extern float f_math_mean(float *values, size_t elements);
extern float f_math_sqrt(float value, float precision);
extern float f_math_rms(float *values, size_t elements, float precision);
#endif
