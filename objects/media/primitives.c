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
#include "primitives.h"
void f_primitive_fill_triangle(SDL_Renderer *renderer, int x_A, int y_A, int x_B, int y_B, int x_C, int y_C, int red, int green, int blue, int alpha) {
  filledTrigonRGBA(renderer, x_A, y_A, x_B, y_B, x_C, y_C, red, green, blue, alpha);
}
void f_primitive_fill_polygon(SDL_Renderer *renderer, short int *x, short int *y, size_t entries, int red, int green, int blue, int alpha) {
  filledPolygonRGBA(renderer, x, y, entries, red, green, blue, alpha);
}
void f_primitive_fill_circle(SDL_Renderer *renderer, int x, int y, int radius, int red, int green, int blue, int alpha) {
  filledCircleRGBA(renderer, x, y, radius, red, green, blue, alpha);
}

