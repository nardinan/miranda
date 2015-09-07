/*
 * miranda
 * Copyright (C) 2015 Andrea Nardinocchi (andrea@nardinan.it)
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
#ifndef miranda_graphic_particle_h
#define miranda_graphic_particle_h
#include <sys/time.h>
#include <math.h>
#include "environment.obj.h"
#define d_particle_cores 2048
#define d_particle_randomizeF(obj,key) ((obj)->configuration.minimum.key+(d_math_frand*((obj)->configuration.maximum.key-(obj)->configuration.minimum.key)))
#define d_particle_randomizeI(obj,key) ((obj)->configuration.minimum.key+(rand()%((obj)->configuration.maximum.key-(obj)->configuration.minimum.key)))
#define d_particle_kill(v,min,max) do{\
	if((v)>(max))\
		(v)=(max);\
	if((v)<(min))\
		(v)=(min);\
}while(0)
typedef struct s_particle_configuration_core {
	double position_x, position_y, zoom, angle, gravity_x, gravity_y, direction_angle, speed_linear, speed_radial, speed_tangential, speed_zoom,
	       speed_angle, mask_R, mask_G, mask_B, mask_A, speed_R, speed_G, speed_B, speed_A, lifetime;
} s_particle_configuration_core;
typedef void (*t_particle_initializer)(struct s_particle_configuration_core *particle);
typedef struct s_particle_configuration {
	unsigned int particles, emission_rate;
	struct s_particle_configuration_core minimum, maximum;
	t_particle_initializer initializer; /* isn't mandatory */
	t_boolean expired;
	enum e_drawable_blends blend;
} s_particle_configuration;
typedef struct s_particle_information {
	struct timeval born, update;
	struct s_particle_configuration_core core;
	t_boolean alive;
} s_particle_information;
d_declare_class(particle) {
	struct s_attributes head;
	struct s_object *drawable_core;
	struct timeval last_generation;
	struct s_particle_configuration configuration;
	struct s_particle_information particles[d_particle_cores];
	t_boolean initialized;
} d_declare_class_tail(particle);
struct s_particle_attributes *p_particle_alloc(struct s_object *self);
extern struct s_object *f_particle_new(struct s_object *self, struct s_object *drawable_particle, struct s_object *environment,
		struct s_particle_configuration *configuration);
d_declare_method(particle, update)(struct s_object *self, unsigned int max_particles);
d_declare_method(particle, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(particle, set_mask)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha);
d_declare_method(particle, set_blend)(struct s_object *self, enum e_drawable_blends blend);
d_declare_method(particle, delete)(struct s_object *self, struct s_particle_attributes *attributes);
#endif
