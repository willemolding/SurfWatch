#include "main_window.h"
#include "../data/surf_data.h"
#include "../layers/dial_widget.h"
#include "../layers/clock_layer.h"

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

static char star_string[2*MAX_SURF_RATING + 1] = "                    ";
static char wave_height_string[20];

static void update_display_data() {
    // //update the star string
    // for(uint16_t i = 0; i < MAX_SURF_RATING; i++){
    //     if(i < surf_data->surf_rating){
    //       if(i < surf_data->surf_rating + surf_data->wind_rating_penalty){
    //         star_string[2*i + 1] = 'w';
    //         star_string[2*i + 2] = ' ';
    //       }
    //       else{
    //         star_string[2*i + 1] = 'o';
    //         star_string[2*i + 2] = ' ';
    //       }

    //     }
    //     else{
    //       star_string[2*i + 1] = '\0';
    //       star_string[2*i + 2] = '\0';
    //     }
    // }

    //update the height string
    // snprintf(wave_height_string, sizeof(wave_height_string), "%d-%d ", 
    //   surf_data->min_surf_height, surf_data->max_surf_height);

    // strcat(wave_height_string, surf_data->swell_units);

    // text_layer_set_text(star_label, star_string);
    // text_layer_set_text(tide_event_text_layer, height_text);
}


static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);

  GRect bounds = layer_get_bounds(window_layer);


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


}

static void window_unload(Window *window) {
  window_destroy(window);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(clock_layer);
}

void main_window_push(SurfData *surf_data) {
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