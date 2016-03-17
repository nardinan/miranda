/*
 * miranda
 * Copyright (C) 2016 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_media_primitives_h
#define miranda_media_primitives_h
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#define d_primitives_minimum_polygon_points 2
extern void p_primitive_fill_triangle_bottom_flat(SDL_Renderer *renderer, int x_A, int y_A, int x_B, int y_B, int x_C, int y_C,
		int red, int green, int blue, int alpha);
extern void p_primitive_fill_triangle_top_flat(SDL_Renderer *renderer, int x_A, int y_A, int x_B, int y_B, int x_C, int y_C,
		int red, int green, int blue, int alpha);
extern void f_primitive_fill_triangle(SDL_Renderer *renderer, int x_A, int y_A, int x_B, int y_B, int x_C, int y_C, int red, int green, int blue, int alpha);
extern void f_primitive_fill_polygon(SDL_Renderer *renderer, int *x, int *y, size_t entries, int red, int green, int blue, int alpha);
#endif
