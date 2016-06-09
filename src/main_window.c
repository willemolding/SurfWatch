#include "main_window.h"

static Window *window;

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  GRect bounds = layer_get_bounds(window_layer);


  //add the wave layer at the base
  GRect wave_layer_bounds = grect_inset(bounds, GEdgeInsets(SCREEN_HEIGHT * 3 / 4, 0, 0));
  wave_layer = layer_create(wave_layer_bounds);
  layer_set_update_proc(wave_layer, wave_layer_update_callback);
  layer_add_child(window_layer, wave_layer);

  ////////////////////////////////////// From Chris
  // Load Font
  s_surf_font_24 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GOTHAM_LIGHT_24));
  s_symbol_font_18 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SYMBOL_18));
  
  // Create the surf text layer
  surf_label = text_layer_create(GRect((bounds.size.w / 4), (bounds.size.h / 4) - 15, (bounds.size.w / 2), 30));
  text_layer_set_text(surf_label, wave_height_string);
  text_layer_set_text_color(surf_label, GColorDarkCandyAppleRed);
  text_layer_set_font(surf_label, s_surf_font_24);
  text_layer_set_text_alignment(surf_label, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(surf_label));
  
  // Create the star text layer
  star_label = text_layer_create(GRect(0, (bounds.size.h / 4) + 10, bounds.size.w, 50));
  text_layer_set_text(star_label, star_string);
  text_layer_set_font(star_label, s_symbol_font_18);
  text_layer_set_text_alignment(star_label, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(star_label));
  
  // Create the wind text layer
  wind_label = text_layer_create(GRect(((bounds.size.w / 4) * 3) - 19, ((bounds.size.h / 2) - 8), 30, 15));
  snprintf(wind_strength, sizeof(wind_strength), "%d", surf_data.wind_strength);
  text_layer_set_text(wind_label, wind_strength);
  text_layer_set_background_color(wind_label, GColorClear);
  text_layer_set_font(wind_label, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(wind_label, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(wind_label));
  
  // Create the wind units text layer
  wind_units_label = text_layer_create(GRect(((bounds.size.w / 4) * 3) - 19, ((bounds.size.h / 2) + 5), 30, 15));
  text_layer_set_text(wind_units_label, surf_data.wind_units);
  text_layer_set_background_color(wind_units_label, GColorClear);
  text_layer_set_font(wind_units_label, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(wind_units_label, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(wind_units_label));
  
  // Create the swell text layer
  swell_label = text_layer_create(GRect((bounds.size.w / 4) - 10, ((bounds.size.h / 2) - 7), 30, 15));
  snprintf(swell_height, sizeof(swell_height), "%d", surf_data.swell_height);
  text_layer_set_text(swell_label, swell_height);
  text_layer_set_background_color(swell_label, GColorClear);
  text_layer_set_font(swell_label, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(swell_label, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(swell_label));
  
  // Create the swell units text layer
  swell_units_label = text_layer_create(GRect((bounds.size.w / 4) - 10, ((bounds.size.h / 2) + 5), 30, 15));
  text_layer_set_text(swell_units_label, surf_data.swell_units);
  text_layer_set_background_color(swell_units_label, GColorClear);
  text_layer_set_font(swell_units_label, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(swell_units_label, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(swell_units_label));
  
  // Create background layer
  s_canvas_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  layer_add_child(window_layer, s_canvas_layer);
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  
  // Create hands layer
  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);

  //////////////////////////////////////////////////

  //create the event text layer
  GRect tide_event_text_layer_bounds = GRect(SCREEN_WIDTH/3 - 40, SCREEN_HEIGHT * 3 / 4 - 20,
                                             SCREEN_WIDTH/3 + 20, SCREEN_HEIGHT * 3 / 4 + 20);
  tide_event_text_layer = text_layer_create(tide_event_text_layer_bounds);
  text_layer_set_text(tide_event_text_layer, "Loading");
  text_layer_set_font(tide_event_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_background_color(tide_event_text_layer, GColorClear);
  text_layer_set_text_alignment(tide_event_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(tide_event_text_layer));

}

static void window_unload(Window *window) {
  window_destroy(s_window);
}

void main_window_push() {
  if(!window) {
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
    });
  }
  window_stack_push(window, true);
}