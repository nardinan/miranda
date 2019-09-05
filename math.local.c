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
#include "math.local.h"
float f_math_mean(float *values, size_t elements) {
  float result = 0.0f;
  int index;
  for (index = 0; index < elements; ++index)
    result += values[index];
  result /= (float)elements;
  return result;
}
float f_math_sqrt(float value, float precision) {
  float result = value, level = 1.0f;
  while ((result - level) > precision) {
    result = (result + level) / 2.0f;
    level = value / result;
  }
  return result;
}
float f_math_rms(float *values, size_t elements, float precision) {
  float mean = 0.0f, mean_square = 0.0f;
  int index;
  for (index = 0; index < elements; ++index) {
    mean += values[index];
    mean_square += (values[index] * values[index]);
  }
  mean /= (float)elements;
  mean_square /= (float)elements;
  return f_math_sqrt(fabs(mean_square - (mean * mean)), precision);
}