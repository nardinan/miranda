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
#include "particle.obj.h"
struct s_particle_attributes *p_particle_alloc(struct s_object *self) {
	struct s_particle_attributes *result = d_prepare(self, particle);
	f_mutex_new(self);									/* inherit */
	f_drawable_new(self, (e_drawable_kind_multiple|e_drawable_kind_force_visibility));	/* inherit */
	return result;
}

struct s_object *f_particle_new(struct s_object *self, struct s_object *drawable_particle, struct s_object *environment,
		struct s_particle_configuration *configuration) {
	struct s_particle_attributes *attributes = p_particle_alloc(self);
	attributes->drawable_core = d_retain(drawable_particle);
	memcpy(&(attributes->configuration), configuration, sizeof(struct s_particle_configuration));
	memset(&(attributes->particles), 0, (sizeof(struct s_particle_information) * d_particle_cores));
	d_call(attributes->drawable_core, m_drawable_set_blend, attributes->configuration.blend);
	attributes->initialized = d_false;
	attributes->mask_R = 255.0;
	attributes->mask_G = 255.0;
	attributes->mask_B = 255.0;
	attributes->mask_A = 255.0;
	return self;
}

d_define_method(particle, update)(struct s_object *self, unsigned int max_particles) {
	d_using(particle);
	unsigned int index;
	struct timeval current, elapsed_begin, elapsed_update;
	struct s_drawable_attributes *drawable_attributes_self = d_cast(self, drawable);
	double local_position_x, local_position_y, real_elapsed_begin, real_elapsed_update, radians, speed_x, speed_y;
	unsigned int generated = 0;
	gettimeofday(&current, NULL);
	d_call(&(drawable_attributes_self->point_destination), m_point_get, (double *)&local_position_x, (double *)&local_position_y);
	for (index = 0; index < particle_attributes->configuration.particles; ++index) {
		if (particle_attributes->particles[index].alive) {
			timersub(&current, &(particle_attributes->particles[index].born), &elapsed_begin);
			real_elapsed_begin = elapsed_begin.tv_sec + ((double)(elapsed_begin.tv_usec)/1000000.0);
			if (particle_attributes->particles[index].core.lifetime > real_elapsed_begin) {
				timersub(&current, &(particle_attributes->particles[index].update), &elapsed_update);
				real_elapsed_update = elapsed_update.tv_sec + ((double)(elapsed_update.tv_usec)/1000000.0);
				particle_attributes->particles[index].core.mask_R +=
					(particle_attributes->particles[index].core.speed_R * real_elapsed_update);
				particle_attributes->particles[index].core.mask_G +=
					(particle_attributes->particles[index].core.speed_G * real_elapsed_update);
				particle_attributes->particles[index].core.mask_B +=
					(particle_attributes->particles[index].core.speed_B * real_elapsed_update);
				particle_attributes->particles[index].core.mask_A +=
					(particle_attributes->particles[index].core.speed_A * real_elapsed_update);
				d_particle_kill(particle_attributes->particles[index].core.mask_R, 0, 255);
				d_particle_kill(particle_attributes->particles[index].core.mask_G, 0, 255);
				d_particle_kill(particle_attributes->particles[index].core.mask_B, 0, 255);
				d_particle_kill(particle_attributes->particles[index].core.mask_A, 0, 255);
				particle_attributes->particles[index].core.zoom +=
				       	(particle_attributes->particles[index].core.speed_zoom * real_elapsed_update);
				particle_attributes->particles[index].core.angle +=
					(particle_attributes->particles[index].core.speed_angle * real_elapsed_update);
				particle_attributes->particles[index].core.direction_angle +=
					(particle_attributes->particles[index].core.speed_direction_angle * real_elapsed_update);
				radians = (particle_attributes->particles[index].core.direction_angle * d_math_pi)/180.0;
				speed_x = particle_attributes->particles[index].core.speed_linear * cos(radians);
				speed_y = particle_attributes->particles[index].core.speed_linear * sin(radians);
				speed_x += (particle_attributes->particles[index].core.gravity_x * real_elapsed_begin);
				speed_y += (particle_attributes->particles[index].core.gravity_y * real_elapsed_begin);
				particle_attributes->particles[index].core.position_x += (speed_x * real_elapsed_update);
				particle_attributes->particles[index].core.position_y += (speed_y * real_elapsed_update);
				memcpy(&(particle_attributes->particles[index].update), &current, sizeof(struct timeval));
			} else
				particle_attributes->particles[index].alive = d_false;
		} else if ((generated < max_particles) && ((particle_attributes->particles[index].was_alive == d_false) ||
					(particle_attributes->configuration.single_shoot == d_false))) {
			particle_attributes->particles[index].was_alive = d_true;
			memcpy(&(particle_attributes->particles[index].born), &current, sizeof(struct timeval));
			memcpy(&(particle_attributes->particles[index].update), &current, sizeof(struct timeval));
			particle_attributes->particles[index].core.position_x = d_particle_randomizeF(particle_attributes, position_x) + local_position_x;
			particle_attributes->particles[index].core.position_y = d_particle_randomizeF(particle_attributes, position_y) + local_position_y;
			particle_attributes->particles[index].core.zoom = d_particle_randomizeF(particle_attributes, zoom);
			particle_attributes->particles[index].core.angle = d_particle_randomizeF(particle_attributes, angle);
			particle_attributes->particles[index].core.gravity_x = d_particle_randomizeF(particle_attributes, gravity_x);
			particle_attributes->particles[index].core.gravity_y = d_particle_randomizeF(particle_attributes, gravity_y);
			particle_attributes->particles[index].core.direction_angle = d_particle_randomizeF(particle_attributes, direction_angle);
			particle_attributes->particles[index].core.speed_linear = d_particle_randomizeF(particle_attributes, speed_linear);
			particle_attributes->particles[index].core.speed_direction_angle = d_particle_randomizeF(particle_attributes, speed_direction_angle);
			particle_attributes->particles[index].core.speed_zoom = d_particle_randomizeF(particle_attributes, speed_zoom);
			particle_attributes->particles[index].core.speed_angle = d_particle_randomizeF(particle_attributes, speed_angle);
			particle_attributes->particles[index].core.mask_R = d_particle_randomizeF(particle_attributes, mask_R);
			particle_attributes->particles[index].core.mask_G = d_particle_randomizeF(particle_attributes, mask_G);
			particle_attributes->particles[index].core.mask_B = d_particle_randomizeF(particle_attributes, mask_B);
			particle_attributes->particles[index].core.mask_A = d_particle_randomizeF(particle_attributes, mask_A);
			particle_attributes->particles[index].core.speed_R = d_particle_randomizeF(particle_attributes, speed_R);
			particle_attributes->particles[index].core.speed_G = d_particle_randomizeF(particle_attributes, speed_G);
			particle_attributes->particles[index].core.speed_B = d_particle_randomizeF(particle_attributes, speed_B);
			particle_attributes->particles[index].core.speed_A = d_particle_randomizeF(particle_attributes, speed_A);
			particle_attributes->particles[index].core.lifetime = d_particle_randomizeF(particle_attributes, lifetime);
			if (particle_attributes->configuration.initializer)
				particle_attributes->configuration.initializer(&(particle_attributes->particles[index].core));
			particle_attributes->particles[index].alive = d_true;
			++generated;
		}
	}
	return self;
}

d_define_method_override(particle, draw)(struct s_object *self, struct s_object *environment) {
	d_using(particle);
	unsigned int index, new_particles;
	struct timeval current, elapsed_update;
	double real_elapsed_update, local_position_x, local_position_y, position_x, position_y, normalized_R, normalized_G, normalized_B, normalized_A;
	struct s_drawable_attributes *drawable_attributes_core = d_cast(particle_attributes->drawable_core, drawable),
				     *drawable_attributes_self = d_cast(self, drawable);
	struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
	gettimeofday(&current, NULL);
	if (!particle_attributes->initialized) {
		memcpy(&(particle_attributes->last_generation), &current, sizeof(struct timeval));
		particle_attributes->initialized = d_true;
	}
	timersub(&current, &(particle_attributes->last_generation), &elapsed_update);
	real_elapsed_update = elapsed_update.tv_sec + ((double)(elapsed_update.tv_usec)/1000000.0);
	if ((new_particles = (unsigned int)(particle_attributes->configuration.emission_rate * real_elapsed_update)) > 0)
		memcpy(&(particle_attributes->last_generation), &current, sizeof(struct timeval));
	d_call(self, m_particle_update, new_particles);
	d_call(&(drawable_attributes_self->point_destination), m_point_get, (double *)&local_position_x, (double *)&local_position_y);
	for (index = 0; index < particle_attributes->configuration.particles; ++index)
		if (particle_attributes->particles[index].alive) {
			normalized_R = ((particle_attributes->particles[index].core.mask_R/255.0)*(particle_attributes->mask_R/255.0))*255.0;
			normalized_G = ((particle_attributes->particles[index].core.mask_G/255.0)*(particle_attributes->mask_G/255.0))*255.0;
			normalized_B = ((particle_attributes->particles[index].core.mask_B/255.0)*(particle_attributes->mask_B/255.0))*255.0;
			normalized_A = ((particle_attributes->particles[index].core.mask_A/255.0)*(particle_attributes->mask_A/255.0))*255.0;
			d_call(particle_attributes->drawable_core, m_drawable_set_maskRGB,
					(unsigned int)normalized_R,
					(unsigned int)normalized_G,
					(unsigned int)normalized_B);
			d_call(particle_attributes->drawable_core, m_drawable_set_maskA, (unsigned int)normalized_A);
			position_x = particle_attributes->particles[index].core.position_x;
			position_y = particle_attributes->particles[index].core.position_y;
			d_call(&(drawable_attributes_core->point_destination), m_point_set_x, (double)position_x);
			d_call(&(drawable_attributes_core->point_destination), m_point_set_y, (double)position_y);
			drawable_attributes_core->zoom = (particle_attributes->particles[index].core.zoom * drawable_attributes_self->zoom);
			drawable_attributes_core->angle = (particle_attributes->particles[index].core.angle + drawable_attributes_self->angle);
			drawable_attributes_core->flip = drawable_attributes_self->flip;
			if ((d_call(particle_attributes->drawable_core, m_drawable_normalize_scale, environment_attributes->reference_w,
							environment_attributes->reference_h,
							environment_attributes->camera_origin_x,
							environment_attributes->camera_origin_y,
							environment_attributes->camera_focus_x,
							environment_attributes->camera_focus_y,
							environment_attributes->current_w,
							environment_attributes->current_h,
							environment_attributes->zoom)))
				while (((int)d_call(particle_attributes->drawable_core, m_drawable_draw, environment)) == d_drawable_return_continue);
		}
	d_cast_return(d_drawable_return_last);
}

d_define_method_override(particle, set_maskRGB)(struct s_object *self, unsigned int red, unsigned int green, unsigned int blue) {
	d_using(particle);
	particle_attributes->mask_R = red;
	particle_attributes->mask_G = green,
	particle_attributes->mask_B = blue;
	return self;
}

d_define_method_override(particle, set_maskA)(struct s_object *self, unsigned int alpha) {
	d_using(particle);
	particle_attributes->mask_A = alpha;
	return self;
}

d_define_method_override(particle, set_blend)(struct s_object *self, enum e_drawable_blends blend) {
	d_using(particle);
	return d_call(particle_attributes->drawable_core, m_drawable_set_blend, blend);
}

d_define_method(particle, delete)(struct s_object *self, struct s_particle_attributes *attributes) {
	if (attributes->drawable_core)
		d_delete(attributes->drawable_core);
	return NULL;
}

d_define_class(particle) {
	d_hook_method(particle, e_flag_private, update),
	d_hook_method_override(particle, e_flag_public, drawable, draw),
	d_hook_method_override(particle, e_flag_public, drawable, set_maskRGB),
	d_hook_method_override(particle, e_flag_public, drawable, set_maskA),
	d_hook_method_override(particle, e_flag_public, drawable, set_blend),
	d_hook_delete(particle),
	d_hook_method_tail
};
