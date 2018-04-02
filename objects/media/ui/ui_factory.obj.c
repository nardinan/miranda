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
#include "ui_factory.obj.h"
struct s_ui_factory_attributes *p_ui_factory_alloc(struct s_object *self) {
  struct s_ui_factory_attributes *result = d_prepare(self, ui_factory);
  f_memory_new(self);   /* inherit */
  f_mutex_new(self);    /* inherit */
  return result;
}
void p_ui_factory_container_delete(struct s_uiable_container *container) {
  struct s_uiable_container *current_container;
  d_delete(container->uiable);
  while ((current_container = (struct s_uiable_container *)container->children.head)) {
    f_list_delete(&(container->children), (struct s_list_node *)current_container);
    p_ui_factory_container_delete(current_container);
    d_free(current_container);
  }
}
struct s_object *f_ui_factory_new(struct s_object *self, struct s_object *resources_png, struct s_object *resources_ttf, struct s_object *resources_json,
                                  struct s_object *environment, struct s_object *json_configuration, struct s_object *json_ui) {
  struct s_ui_factory_attributes *attributes = p_ui_factory_alloc(self);
  struct s_exception *exception;
  struct s_object *font;
  struct s_uiable_container *current_container;
  char *font_buffer, *string_supply;
  t_json_starting_point *starting_point;
  t_boolean boolean_supply;
  double font_size, font_outline, position_x, position_y;
  int index_font = 0, index_container = 0;
  attributes->resources_png = d_retain(resources_png);
  attributes->resources_ttf = d_retain(resources_ttf);
  attributes->resources_json = d_retain(resources_json);
  attributes->environment = d_retain(environment);
  attributes->json_configuration = d_retain(json_configuration);
  d_try
      {
        if ((attributes->font_system = f_fonts_new(d_new(fonts)))) {
          while (d_call(attributes->json_configuration, m_json_get_string, &font_buffer, "sds", "fonts", index_font, "font")) {
            font_size = d_ui_factory_default_font_size;
            font_outline = d_ui_factory_default_font_outline;
            d_call(attributes->json_configuration, m_json_get_double, &font_size, "sds", "fonts", index_font, "size");
            d_call(attributes->json_configuration, m_json_get_double, &font_outline, "sds", "fonts", index_font, "outline");
            if ((font = d_call(attributes->resources_ttf, m_resources_get_stream, font_buffer, e_resources_type_common))) {
              d_call(attributes->font_system, m_fonts_add_font, index_font, font, (int)font_size);
              if (font_outline > 0)
                d_call(attributes->font_system, m_fonts_set_outline, index_font, (int)font_outline);
            }
            ++index_font;
          }
          while (d_call(json_ui, m_json_get_string, &string_supply, "sds", "root", index_container, "label")) {
            position_x = 0;
            position_y = 0;
            if ((current_container = (struct s_uiable_container *)d_malloc(sizeof(struct s_uiable_container)))) {
              strncpy(current_container->label, string_supply, d_ui_factory_label_size);
              f_list_append(&(attributes->children), (struct s_list_node *)current_container, e_list_insert_head);
              if ((starting_point = (t_json_starting_point *)d_call(json_ui, m_json_get_relative, NULL, "sd", "root", index_container))) {
                d_call(json_ui, m_json_get_boolean_relative, starting_point, &boolean_supply, "s", "floatable");
                d_call(json_ui, m_json_get_double_relative, starting_point, &position_x, "s", "position_x");
                d_call(json_ui, m_json_get_double_relative, starting_point, &position_y, "s", "position_y");
                if ((current_container->uiable = d_call(self, m_ui_factory_new_container, boolean_supply))) {
                  d_call(current_container->uiable, m_drawable_set_position, position_x, position_y);
                  d_call(self, m_ui_factory_load_component, json_ui, current_container, starting_point);
                }
              }
            } else
              d_die(d_error_malloc);
            ++index_container;
          }
        } else
          d_die(d_error_malloc);
      }
    d_catch(exception)
      {
        d_exception_dump(stderr, exception);
        d_raise;
      }
  d_endtry;
  return self;
}
d_define_method(ui_factory, load_component)(struct s_object *self, struct s_object *json_ui, struct s_uiable_container *current_container,
                                            t_json_starting_point *starting_point) {
  d_using(ui_factory);
  struct _scoped_keyword_list {
    const char *kind;
    t_class_method call;
    t_boolean container;
  } scoped_keyword_list[] = {{"container", (t_class_method)&(p_ui_factory_new_container), d_true},
                             {"label",     (t_class_method)&(p_ui_factory_new_label),     d_false},
                             {"checkbox",  (t_class_method)&(p_ui_factory_new_checkbox),  d_false},
                             {"button",    (t_class_method)&(p_ui_factory_new_button),    d_false},
                             {"field",     (t_class_method)&(p_ui_factory_new_field),     d_false},
                             {"list",      (t_class_method)&(p_ui_factory_new_list),      d_false},
                             {"scroll",    (t_class_method)&(p_ui_factory_new_scroll),    d_false},
                             {NULL}};
  struct s_label_attributes *label_attributes;
  struct s_checkbox_attributes *checkbox_attributes;
  struct s_uiable_container *current_component;
  struct s_object *stream;
  t_json_starting_point *next_starting_point;
  t_boolean boolean_supply;
  char buffer[d_string_buffer_size], *string_supply = NULL;
  int index, index_component = 0;
  double font_id = d_ui_factory_default_font_id, font_style = d_ui_factory_default_font_style, position_x, position_y, width, height, alignment_x, alignment_y,
    background_R, background_G, background_B, background_A, angle, range_minimum, range_maximum, modifier;
  while ((d_call(json_ui, m_json_get_string_relative, starting_point, &string_supply, "sds", "components", index_component, "label"))) {
    position_x = 0.0;
    position_y = 0.0;
    background_R = 255.0;
    background_G = 255.0;
    background_B = 255.0;
    background_A = 0.0;
    if ((current_component = (struct s_uiable_container *)d_malloc(sizeof(struct s_uiable_container)))) {
      strncpy(current_component->label, string_supply, d_ui_factory_label_size);
      f_list_append(&(current_container->children), (struct s_list_node *)current_component, e_list_insert_head);
      if ((next_starting_point = (t_json_starting_point *)d_call(json_ui, m_json_get_relative, starting_point, "sd", "components", index_component))) {
        if ((d_call(json_ui, m_json_get_string_relative, next_starting_point, &string_supply, "s", "kind"))) {
          for (index = 0; scoped_keyword_list[index].kind; ++index)
            if (f_string_strcmp(string_supply, scoped_keyword_list[index].kind) == 0)
              break;
          if (scoped_keyword_list[index].kind) {
            if (scoped_keyword_list[index].container) {
              /* a container */
              d_call(json_ui, m_json_get_boolean_relative, next_starting_point, &boolean_supply, "s", "floatable");
              if ((current_component->uiable = scoped_keyword_list[index].call(self, boolean_supply)))
                d_call(self, m_ui_factory_load_component, json_ui, current_component, next_starting_point);
            } else {
              d_call(json_ui, m_json_get_double_relative, next_starting_point, &font_id, "s", "font_id");
              d_call(json_ui, m_json_get_double_relative, next_starting_point, &font_style, "s", "font_style");
              d_call(json_ui, m_json_get_string_relative, next_starting_point, &string_supply, "s", "content");
              if ((current_component->uiable = scoped_keyword_list[index].call(self, (unsigned int)font_id, (unsigned int)font_style, string_supply))) {
                /* if ineriths from the label object */
                if ((label_attributes = d_cast(current_component->uiable, label))) {
                  if ((d_call(json_ui, m_json_get_double_relative, next_starting_point, &alignment_x, "s", "alignment_x")))
                    label_attributes->alignment_x = (enum e_label_alignments)alignment_x;
                  if ((d_call(json_ui, m_json_get_double_relative, next_starting_point, &alignment_y, "s", "alignment_y")))
                    label_attributes->alignment_y = (enum e_label_alignments)alignment_y;
                }
                /* if ineriths from the checkbox object */
                if ((checkbox_attributes = d_cast(current_component->uiable, checkbox)))
                  if ((d_call(json_ui, m_json_get_boolean_relative, next_starting_point, &boolean_supply, "s", "checked")))
                    checkbox_attributes->is_checked = boolean_supply;
                if (d_cast(current_component->uiable, scroll)) {
                  range_minimum = 0.0;
                  range_maximum = 100.0;
                  modifier = 1.0;
                  d_call(json_ui, m_json_get_double_relative, next_starting_point, &range_minimum, "s", "minimum");
                  d_call(json_ui, m_json_get_double_relative, next_starting_point, &range_maximum, "s", "maximum");
                  d_call(json_ui, m_json_get_double_relative, next_starting_point, &modifier, "s", "scroll_speed");
                  d_call(current_component->uiable, m_scroll_set_modifier, (int)modifier);
                  d_call(current_component->uiable, m_scroll_set_range, (int)range_minimum, (int)range_maximum);
                }
              }
            }
          }
        } else if (d_call(json_ui, m_json_get_string_relative, next_starting_point, &string_supply, "s", "source"))
          if ((stream = d_call(ui_factory_attributes->resources_png, m_resources_get_stream, string_supply, e_resources_type_common)))
            current_component->uiable = f_bitmap_new(d_new(bitmap), stream, ui_factory_attributes->environment);
        if (current_component->uiable) {
          d_call(json_ui, m_json_get_double_relative, next_starting_point, &position_x, "s", "position_x");
          d_call(json_ui, m_json_get_double_relative, next_starting_point, &position_y, "s", "position_y");
          if ((d_call(json_ui, m_json_get_double_relative, next_starting_point, &width, "s", "width")))
            d_call(current_component->uiable, m_drawable_set_dimension_w, width);
          if ((d_call(json_ui, m_json_get_double_relative, next_starting_point, &height, "s", "height")))
            d_call(current_component->uiable, m_drawable_set_dimension_h, height);
          if ((d_call(json_ui, m_json_get_double_relative, next_starting_point, &angle, "s", "angle"))) {
            d_call(current_component->uiable, m_drawable_get_dimension, &width, &height);
            d_call(current_component->uiable, m_drawable_set_center, (double)(width / 2.0), (double)(height / 2.0));
            d_call(current_component->uiable, m_drawable_set_angle, angle);
          }
          current_component->position_x = position_x;
          current_component->position_y = position_y;
          if (d_cast(current_component->uiable, uiable)) {
            d_call(json_ui, m_json_get_double_relative, next_starting_point, &background_R, "s", "background_R");
            d_call(json_ui, m_json_get_double_relative, next_starting_point, &background_G, "s", "background_G");
            d_call(json_ui, m_json_get_double_relative, next_starting_point, &background_B, "s", "background_B");
            d_call(json_ui, m_json_get_double_relative, next_starting_point, &background_A, "s", "background_A");
            d_call(current_component->uiable, m_uiable_set_background, (unsigned int)background_R, (unsigned int)background_G, (unsigned int)background_B,
                   (unsigned int)background_A);
          }
          d_call(current_container->uiable, m_container_add_drawable, current_component->uiable, position_x, position_y);
        } else {
          snprintf(buffer, d_string_buffer_size, "kind '%s' has not been recognized", string_supply);
          d_throw(v_exception_wrong_type, buffer);
        }
      }
    } else
      d_die(d_error_malloc);
    ++index_component;
  }
  return self;
}
d_define_method(ui_factory, load_uiable)(struct s_object *self, struct s_object *uiable, const char *component) {
  d_using(ui_factory);
  struct s_drawable_attributes *drawable_attributes = d_cast(uiable, drawable);
  struct s_uiable_attributes *uiable_attributes = d_cast(uiable, uiable);
  struct s_exception *exception;
  struct s_object *stream = NULL;
  struct s_object *bitmap = NULL;
  int index_component, index_mode;
  double default_mask_R = drawable_attributes->last_mask_R, default_mask_G = drawable_attributes->last_mask_G,
    default_mask_B = drawable_attributes->last_mask_B, default_mask_A = drawable_attributes->last_mask_A, default_border_w = d_uiable_default_border,
    default_border_h = d_uiable_default_border;
  char buffer_path[PATH_MAX];
  d_try
      {
        for (index_component = 0; index_component != e_uiable_component_NULL; ++index_component) {
          for (index_mode = 0; index_mode != e_uiable_mode_NULL; ++index_mode) {
            snprintf(buffer_path, PATH_MAX, "%s_%s_%s", component, v_uiable_modes[index_mode], v_uiable_components[index_component]);
            if (!(stream = d_call(ui_factory_attributes->resources_png, m_resources_get_stream_strict, buffer_path, e_resources_type_common))) {
              snprintf(buffer_path, PATH_MAX, "%s_%s_%s", component, v_uiable_modes[d_ui_factory_default_mode], v_uiable_components[index_component]);
              stream = d_call(ui_factory_attributes->resources_png, m_resources_get_stream_strict, buffer_path, e_resources_type_common);
            }
            if (stream)
              if ((bitmap = f_bitmap_new(d_new(bitmap), stream, ui_factory_attributes->environment))) {
                d_call(uiable, m_uiable_set, bitmap, index_mode, index_component);
                d_delete(bitmap);
              }
          }
        }
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &default_mask_R, "sss", "ui", "default", "mask_R");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &default_mask_G, "sss", "ui", "default", "mask_G");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &default_mask_B, "sss", "ui", "default", "mask_B");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &default_mask_A, "sss", "ui", "default", "mask_A");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &default_border_w, "sss", "ui", "default", "border_w");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &default_border_h, "sss", "ui", "default", "border_h");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &default_mask_R, "sss", "ui", component, "mask_R");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &default_mask_G, "sss", "ui", component, "mask_G");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &default_mask_B, "sss", "ui", component, "mask_B");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &default_mask_A, "sss", "ui", component, "mask_A");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &default_border_w, "sss", "ui", component, "border_w");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &default_border_h, "sss", "ui", component, "border_h");
        d_call(uiable, m_drawable_set_maskRGB, (unsigned int)default_mask_R, (unsigned int)default_mask_G, (unsigned int)default_mask_B);
        d_call(uiable, m_drawable_set_maskA, (unsigned int)default_mask_A);
        uiable_attributes->border_w = default_border_w;
        uiable_attributes->border_h = default_border_h;
      }
    d_catch(exception)
      {
        d_exception_dump(stderr, exception);
        d_raise;
      }
  d_endtry;
  return self;
}
d_define_method(ui_factory, new_container)(struct s_object *self, t_boolean floatable) {
  d_using(ui_factory);
  struct s_exception *exception;
  struct s_object *result = NULL;
  double border_top = d_uiable_default_border, border_bottom = d_uiable_default_border, border_left = d_uiable_default_border,
    border_right = d_uiable_default_border;
  d_try
      {
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &border_top, "sss", "ui", "container", "border_top");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &border_bottom, "sss", "ui", "container", "border_bottom");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &border_left, "sss", "ui", "container", "border_left");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &border_right, "sss", "ui", "container", "border_right");
        if ((result = f_container_new(d_new(container), border_top, border_bottom, border_left, border_right, floatable)))
          d_call(self, m_ui_factory_load_uiable, result, "container");
      }
    d_catch(exception)
      {
        d_exception_dump(stderr, exception);
        d_raise;
      }
  d_endtry;
  return result;
}
d_define_method(ui_factory, new_list)(struct s_object *self) {
  d_using(ui_factory);
  struct s_exception *exception;
  struct s_object *scroll, *result = NULL;
  struct s_object *stream;
  char *string_supply = NULL;
  double selected_R = 0.0, selected_G = 0.0, selected_B = 0.0, selected_A = 0.0, over_R = 0.0, over_G = 0.0, over_B = 0.0, over_A = 0.0, width_scroll = 0.0;
  d_try
      {
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &selected_R, "sss", "ui", "list", "selected_R");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &selected_G, "sss", "ui", "list", "selected_G");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &selected_B, "sss", "ui", "list", "selected_B");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &selected_A, "sss", "ui", "list", "selected_A");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &over_R, "sss", "ui", "list", "over_R");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &over_G, "sss", "ui", "list", "over_G");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &over_B, "sss", "ui", "list", "over_B");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &over_A, "sss", "ui", "list", "over_A");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &width_scroll, "sss", "ui", "scroll", "width");
        if (!ui_factory_attributes->scroll_handler)
          if ((d_call(ui_factory_attributes->json_configuration, m_json_get_string, &string_supply, "sss", "ui", "scroll", "scroll")))
            if ((stream = d_call(ui_factory_attributes->resources_png, m_resources_get_stream, string_supply, e_resources_type_common)))
              ui_factory_attributes->scroll_handler = f_bitmap_new(d_new(bitmap), stream, ui_factory_attributes->environment);
        if ((scroll = f_scroll_new(d_new(scroll), ui_factory_attributes->scroll_handler))) {
          d_call(scroll, m_drawable_set_dimension_w, width_scroll);
          d_call(self, m_ui_factory_load_uiable, scroll, "scroll");
          if ((result = f_list_new(d_new(list), scroll))) {
            d_call(result, m_list_set_selected, (unsigned int)selected_R, (unsigned int)selected_G, (unsigned int)selected_B, (unsigned int)selected_A);
            d_call(result, m_list_set_over, (unsigned int)over_R, (unsigned int)over_G, (unsigned int)over_B, (unsigned int)over_A);
            d_call(self, m_ui_factory_load_uiable, result, "list");
          }
          d_delete(scroll);
        }
      }
    d_catch(exception)
      {
        d_exception_dump(stderr, exception);
        d_raise;
      }
  d_endtry;
  return result;
}
d_define_method(ui_factory, new_scroll)(struct s_object *self) {
  d_using(ui_factory);
  struct s_exception *exception;
  struct s_object *result = NULL;
  struct s_object *stream;
  char *string_supply = NULL;
  double width_scroll = 0.0;
  d_try
      {
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &width_scroll, "sss", "ui", "scroll", "width");
        if (!ui_factory_attributes->pointer_handler)
          if ((d_call(ui_factory_attributes->json_configuration, m_json_get_string, &string_supply, "sss", "ui", "scroll", "pointer")))
            if ((stream = d_call(ui_factory_attributes->resources_png, m_resources_get_stream, string_supply, e_resources_type_common)))
              ui_factory_attributes->pointer_handler = f_bitmap_new(d_new(bitmap), stream, ui_factory_attributes->environment);
        if ((result = f_scroll_new(d_new(scroll), ui_factory_attributes->pointer_handler))) {
          d_call(result, m_drawable_set_dimension_w, width_scroll);
          d_call(result, m_drawable_add_flags, e_drawable_kind_ui_no_attribute_angle);
          d_call(self, m_ui_factory_load_uiable, result, "scroll");
        }
      }
    d_catch(exception)
      {
        d_exception_dump(stderr, exception);
        d_raise;
      }
  d_endtry;
  return result;
}
d_define_method(ui_factory, new_label)(struct s_object *self, unsigned int font_id, unsigned int font_style, char *string_content) {
  d_using(ui_factory);
  struct s_exception *exception;
  struct s_object *result = NULL;
  TTF_Font *current_font;
  double format = (double)e_label_background_format_adaptable, alignment_x = (double)e_label_alignment_left, alignment_y = (double)e_label_alignment_center,
    font_height;
  d_try
      {
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &format, "sss", "ui", "label", "format");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &alignment_x, "sss", "ui", "label", "alignment_x");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &alignment_y, "sss", "ui", "label", "alignment_y");
        if ((current_font = d_call(self, m_ui_factory_get_font, font_id, font_style, &font_height)))
          if ((result = f_label_new_alignment(d_new(label), string_content, current_font, (enum e_label_background_formats)format,
                                              (enum e_label_alignments)alignment_x, (enum e_label_alignments)alignment_y, ui_factory_attributes->environment)))
            d_call(self, m_ui_factory_load_uiable, result, "label");
      }
    d_catch(exception)
      {
        d_exception_dump(stderr, exception);
        d_raise;
      }
  d_endtry;
  return result;
}
d_define_method(ui_factory, new_checkbox)(struct s_object *self, unsigned int font_id, unsigned int font_style, char *string_content) {
  d_using(ui_factory);
  struct s_label_attributes *label_attributes;
  struct s_exception *exception;
  struct s_object *result = NULL;
  TTF_Font *current_font;
  char *string_supply_checked, *string_supply_unchecked;
  struct s_object *stream_checked, *stream_unchecked;
  double format = (double)e_label_background_format_fixed, alignment_x = (double)e_label_alignment_left, alignment_y = (double)e_label_alignment_center,
    width = 0.0, font_height = 0.0;
  d_try
      {
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &format, "sss", "ui", "checkbox", "format");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &alignment_x, "sss", "ui", "checkbox", "alignment_x");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &alignment_y, "sss", "ui", "checkbox", "alignment_y");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &width, "sss", "ui", "checkbox", "width");
        if (!ui_factory_attributes->checkbox_bitmap_checked)
          if ((d_call(ui_factory_attributes->json_configuration, m_json_get_string, &string_supply_checked, "sss", "ui", "checkbox", "checked")))
            if ((stream_checked = d_call(ui_factory_attributes->resources_png, m_resources_get_stream, string_supply_checked, e_resources_type_common)))
              ui_factory_attributes->checkbox_bitmap_checked = f_bitmap_new(d_new(bitmap), stream_checked, ui_factory_attributes->environment);
        if (!ui_factory_attributes->checkbox_bitmap_unchecked)
          if ((d_call(ui_factory_attributes->json_configuration, m_json_get_string, &string_supply_unchecked, "sss", "ui", "checkbox", "unchecked")))
            if ((stream_unchecked = d_call(ui_factory_attributes->resources_png, m_resources_get_stream, string_supply_unchecked, e_resources_type_common)))
              ui_factory_attributes->checkbox_bitmap_unchecked = f_bitmap_new(d_new(bitmap), stream_unchecked, ui_factory_attributes->environment);
        if ((current_font = d_call(self, m_ui_factory_get_font, font_id, font_style, &font_height)))
          if ((result = f_checkbox_new(d_new(field), string_content, current_font, ui_factory_attributes->environment))) {
            label_attributes = d_cast(result, label);
            label_attributes->format = (enum e_label_background_formats)format;
            label_attributes->alignment_x = (enum e_label_alignments)alignment_x;
            label_attributes->alignment_y = (enum e_label_alignments)alignment_y;
            d_call(result, m_drawable_set_dimension, width, font_height);
            d_call(result, m_checkbox_set_drawable, ui_factory_attributes->checkbox_bitmap_checked, ui_factory_attributes->checkbox_bitmap_unchecked);
            d_call(self, m_ui_factory_load_uiable, result, "label");
          }
      }
    d_catch(exception)
      {
        d_exception_dump(stderr, exception);
        d_raise;
      }
  d_endtry;
  return result;
}
d_define_method(ui_factory, new_button)(struct s_object *self, unsigned int font_id, unsigned int font_style, char *string_content) {
  d_using(ui_factory);
  struct s_exception *exception;
  struct s_label_attributes *label_attributes;
  struct s_object *result = NULL;
  TTF_Font *current_font;
  double format = (double)e_label_background_format_adaptable, alignment_x = (double)e_label_alignment_center, alignment_y = (double)e_label_alignment_center,
    font_height;
  d_try
      {
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &format, "sss", "ui", "button", "format");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &alignment_x, "sss", "ui", "button", "alignment_x");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &alignment_y, "sss", "ui", "button", "alignment_y");
        if ((current_font = d_call(self, m_ui_factory_get_font, font_id, font_style, &font_height)))
          if ((result = f_button_new(d_new(button), string_content, current_font, ui_factory_attributes->environment))) {
            label_attributes = d_cast(result, label);
            label_attributes->format = (enum e_label_background_formats)format;
            label_attributes->alignment_x = (enum e_label_alignments)alignment_x;
            label_attributes->alignment_y = (enum e_label_alignments)alignment_y;
            d_call(self, m_ui_factory_load_uiable, result, "button");
          }
      }
    d_catch(exception)
      {
        d_exception_dump(stderr, exception);
        d_raise;
      }
  d_endtry;
  return result;
}
d_define_method(ui_factory, new_field)(struct s_object *self, unsigned int font_id, unsigned int font_style, char *string_content) {
  d_using(ui_factory);
  struct s_exception *exception;
  struct s_object *result = NULL;
  TTF_Font *current_font;
  double format = (double)e_label_background_format_fixed, alignment_x = (double)e_label_alignment_left, alignment_y = (double)e_label_alignment_center,
    cursor_R = (double)d_field_default_R, cursor_G = (double)d_field_default_G, cursor_B = (double)d_field_default_B, cursor_A = (double)d_field_default_A,
    size = 0.0, width = 0.0, font_height;
  d_try
      {
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &format, "sss", "ui", "field", "format");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &alignment_x, "sss", "ui", "field", "alignment_x");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &alignment_y, "sss", "ui", "field", "alignment_y");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &cursor_R, "sss", "ui", "field", "cursor_R");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &cursor_G, "sss", "ui", "field", "cursor_G");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &cursor_B, "sss", "ui", "field", "cursor_B");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &cursor_A, "sss", "ui", "field", "cursor_A");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &size, "sss", "ui", "field", "size");
        d_call(ui_factory_attributes->json_configuration, m_json_get_double, &width, "sss", "ui", "field", "width");
        if ((current_font = d_call(self, m_ui_factory_get_font, font_id, font_style, &font_height)))
          if ((result = f_field_new_alignment(d_new(field), string_content, current_font, (enum e_label_background_formats)format,
                                              (enum e_label_alignments)alignment_x, (enum e_label_alignments)alignment_y,
                                              ui_factory_attributes->environment))) {
            d_call(result, m_drawable_set_dimension, width, font_height);
            d_call(result, m_field_set_cursor, (unsigned int)cursor_R, (unsigned int)cursor_G, (unsigned int)cursor_B, (unsigned int)cursor_A);
            d_call(result, m_field_set_size, (unsigned int)size);
            d_call(self, m_ui_factory_load_uiable, result, "field");
          }
      }
    d_catch(exception)
      {
        d_exception_dump(stderr, exception);
        d_raise;
      }
  d_endtry;
  return result;
}
d_define_method(ui_factory, show_container)(struct s_object *self, struct s_object *environment, struct s_object *container) {
  return self;
}
d_define_method(ui_factory, hide_container)(struct s_object *self, struct s_object *environment, struct s_object *container) {
  return self;
}
d_define_method(ui_factory, get_component)(struct s_object *self, struct s_uiable_container *current_container, const char *label) {
  d_using(ui_factory);
  struct s_list *current_list = &(ui_factory_attributes->children);
  struct s_uiable_container *result;
  if (current_container)
    current_list = &(current_container->children);
  d_foreach(current_list, result, struct s_uiable_container)
    if (f_string_strcmp(label, result->label) == 0)
      break;
  d_cast_return(result);
}
d_define_method(ui_factory, get_font)(struct s_object *self, int ID, int style, int *height) {
  d_using(ui_factory);
  if (height)
    *height = (intptr_t)d_call(ui_factory_attributes->font_system, m_fonts_get_height, ID);
  return d_call(ui_factory_attributes->font_system, m_fonts_get_font, ID, style);
}
d_define_method(ui_factory, delete)(struct s_object *self, struct s_ui_factory_attributes *attributes) {
  struct s_uiable_container *current_container;
  d_delete(attributes->resources_png);
  d_delete(attributes->resources_ttf);
  d_delete(attributes->resources_json);
  d_delete(attributes->environment);
  d_delete(attributes->json_configuration);
  d_delete(attributes->font_system);
  if (attributes->checkbox_bitmap_checked)
    d_delete(attributes->checkbox_bitmap_checked);
  if (attributes->checkbox_bitmap_unchecked)
    d_delete(attributes->checkbox_bitmap_unchecked);
  if (attributes->scroll_handler)
    d_delete(attributes->scroll_handler);
  if (attributes->pointer_handler)
    d_delete(attributes->pointer_handler);
  while ((current_container = (struct s_uiable_container *)attributes->children.head)) {
    f_list_delete(&(attributes->children), (struct s_list_node *)current_container);
    p_ui_factory_container_delete(current_container);
    d_free(current_container);
  }
  return NULL;
}
d_define_class(ui_factory) {d_hook_method(ui_factory, e_flag_private, load_component),
                            d_hook_method(ui_factory, e_flag_public, load_uiable),
                            d_hook_method(ui_factory, e_flag_public, new_container),
                            d_hook_method(ui_factory, e_flag_public, new_scroll),
                            d_hook_method(ui_factory, e_flag_public, new_list),
                            d_hook_method(ui_factory, e_flag_public, new_label),
                            d_hook_method(ui_factory, e_flag_public, new_checkbox),
                            d_hook_method(ui_factory, e_flag_public, new_button),
                            d_hook_method(ui_factory, e_flag_public, new_field),
                            d_hook_method(ui_factory, e_flag_public, get_component),
                            d_hook_method(ui_factory, e_flag_public, get_font),
                            d_hook_delete(ui_factory),
                            d_hook_method_tail};
void f_ui_factory_callback_visibility(struct s_object *self, void **parameters, size_t entries) {
  struct s_object *uiable = (struct s_object *)parameters[0];
  struct s_object *environment = (struct s_object *)parameters[1];
  if ((uiable->head.next) || (uiable->head.back))
    d_call(environment, m_environment_del_drawable, uiable, d_ui_factory_default_level, e_environment_surface_ui);
  else
    d_call(environment, m_environment_add_drawable, uiable, d_ui_factory_default_level, e_environment_surface_ui);
}
