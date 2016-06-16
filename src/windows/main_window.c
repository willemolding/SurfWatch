#include "main_window.h"
#include "../data/surf_data.h"
#include "../layers/dial_widget.h"
#include "../layers/clock_layer.h"

#define MAX_STAR_STRING 20
#define MAX_WAVE_STRING 20


static Window *window;

static SurfData *surf_data;

//speical layers
static ClockLayer *clock_layer;
static DialWidgetLayer *wind_dial, *swell_dial;
// static TideLayer *tide_layer;

// Textlayers
static TextLayer *surf_label;
static TextLayer *star_label; 

// Fonts
static GFont s_surf_font_24;
static GFont s_symbol_font_18;

static char star_string[MAX_STAR_STRING];
static char wave_height_string[MAX_WAVE_STRING];

static void update_display_data() {

    //Do the star string
    int i = 0;
    for(; i < 2*surf_data->solid_rating; i+=2){
      star_string[i] = 'w';
      star_string[i + 1] = ' ';
    }
    for(; i < 2*(surf_data->solid_rating + surf_data->faded_rating); i+=2){
      star_string[i] = 'o';
      star_string[i + 1] = ' ';
    }
    for(; i < MAX_STAR_STRING; i++){
      star_string[i] = '\0';
    }

    // Do the surf height string
    snprintf(wave_height_string, MAX_WAVE_STRING*sizeof(char), "%u-%u %s", 
      surf_data->min_surf_height, surf_data->max_surf_height, surf_data->swell_units);

    
}


static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

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

  clock_layer = clock_layer_create(bounds);
  layer_add_child(window_layer, clock_layer);

  update_display_data();
}

static void window_unload(Window *window) {
  window_destroy(window);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(clock_layer);
}

void main_window_push(SurfData *data) {
  surf_data = data;

  if(!window) {
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload
    });
  }

  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  window_stack_push(window, true);

}