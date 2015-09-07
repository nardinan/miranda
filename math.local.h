/*
 * miranda
 * Copyright (C) 2014 Andrea Nardinocchi (andrea@nardinan.it)
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
#define d_math_pi 3.14159265358979323846
#define d_math_default_precision 0.001
#define d_math_frand (((double)(rand()%1000))/1000.0)
extern float f_math_mean(float *values, size_t elements);
extern float f_math_sqrt(float value, float precision);
extern float f_math_rms(float *values, size_t elements, float precision);
#endif
