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
#include "primitives.h"
void p_primitive_fill_triangle_bottom_flat(SDL_Renderer *renderer, int x_A, int y_A, int x_B, int y_B, int x_C, int y_C,
		int red, int green, int blue, int alpha) {
	double inverse_slope[2] = {
		((double)(x_B - x_A)/(double)(y_B - y_A)),
		((double)(x_C - x_A)/(double)(y_C - y_A))
	}, current_x[2] = {
		x_A,
		x_A
	};
	int scanline_y;
	SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
	for (scanline_y = y_A; scanline_y <= y_B; ++scanline_y) {
		SDL_RenderDrawLine(renderer, current_x[0], scanline_y, current_x[1], scanline_y);
		current_x[0] += inverse_slope[0];
		current_x[1] += inverse_slope[1];
	}
}

void p_primitive_fill_triangle_top_flat(SDL_Renderer *renderer, int x_A, int y_A, int x_B, int y_B, int x_C, int y_C, int red, int green, int blue, int alpha) {
	double inverse_slope[2] = {
		((double)(x_C - x_A)/(double)(y_C - y_A)),
		((double)(x_C - x_B)/(double)(y_C - y_B))
	}, current_x[2] = {
		x_C,
		x_C
	};
	int scanline_y;
	SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
	for (scanline_y = y_C; scanline_y > y_A; --scanline_y) {
		current_x[0] -= inverse_slope[0];
		current_x[1] -= inverse_slope[1];
		SDL_RenderDrawLine(renderer, current_x[0], scanline_y, current_x[1], scanline_y);
	}
}

#define d_swap(idA,idB) {\
	int b_x, b_y;\
	b_x=sorted_x[idA];\
	b_y=sorted_y[idA];\
	sorted_x[idA]=sorted_x[idB];\
	sorted_y[idA]=sorted_y[idB];\
	sorted_x[idB]=b_x;\
	sorted_y[idB]=b_y;\
}

void f_primitive_fill_triangle(SDL_Renderer *renderer, int x_A, int y_A, int x_B, int y_B, int x_C, int y_C, int red, int green, int blue, int alpha) {
	int sorted_x[3] = {
		x_A,
		x_B,
		x_C
	}, sorted_y[3] = {
		y_A,
		y_B,
		y_C
	}, vertex_x, vertex_y;
	if (sorted_y[0] > sorted_y[1])
		d_swap(0, 1);
	if (sorted_y[1] > sorted_y[2])
		d_swap(1, 2);
	if (sorted_y[0] > sorted_y[1])
		d_swap(0, 1);
	if (sorted_y[1] == sorted_y[2])
		p_primitive_fill_triangle_bottom_flat(renderer, sorted_x[0], sorted_y[0], sorted_x[1], sorted_y[1], sorted_x[2], sorted_y[2],
				red, green, blue, alpha);
	else if (sorted_y[0] == sorted_y[1])
		p_primitive_fill_triangle_top_flat(renderer, sorted_x[0], sorted_y[0], sorted_x[1], sorted_y[1], sorted_x[2], sorted_y[2],
				red, green, blue, alpha);
	else {
		vertex_x = sorted_x[0] + ((double)(sorted_y[1] - sorted_y[0])/(double)(sorted_y[2] - sorted_y[0])) * (sorted_x[2] - sorted_x[0]);
		vertex_y = sorted_y[1];
		p_primitive_fill_triangle_bottom_flat(renderer, sorted_x[0], sorted_y[0], sorted_x[1], sorted_y[1], vertex_x, vertex_y,
				red, green, blue, alpha);
		p_primitive_fill_triangle_top_flat(renderer, sorted_x[1], sorted_y[1], vertex_x, vertex_y, sorted_x[2], sorted_y[2], red, green, blue, alpha);
	}
}

void f_primitive_fill_polygon(SDL_Renderer *renderer, int *x, int *y, size_t entries, int red, int green, int blue, int alpha) {
	int index;
	if (entries > d_primitives_minimum_polygon_points) {
		for (index = 0; index < (entries - d_primitives_minimum_polygon_points); ++index)
			f_primitive_fill_triangle(renderer, x[index], y[index], x[index+1], y[index+1], x[index+2], y[index+2], red, green, blue, alpha);
		f_primitive_fill_triangle(renderer, x[index], y[index], x[index+1], y[index+1], x[0], y[0], red, green, blue, alpha);
	}
}
