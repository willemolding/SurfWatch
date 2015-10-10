#include <pebble.h>
#include "tide_data.h"
#include "tick_path.h"
#include "surf_data.h"

#define SCREEN_WIDTH 180
#define SCREEN_HEIGHT 180
#define LEFT_MARGIN 30

// the text layers to display the info
static Window *window;

// layers for the load screen animation
Layer *wave_layer;


//////////////////////////////// From Chris
// Windows and Layers
static Layer *s_canvas_layer;
static Layer *s_hands_layer;


// Coordinate Paths
static GPath *s_large_ticks;
static GPath *s_small_ticks;
static GPath *s_hour_hand;
static GPath *s_minute_hand;
/*
static GPath *s_wind_ticks;
static GPath *s_swell_ticks;
*/

// Textlayers
static TextLayer *surf_label;
static TextLayer *star_label;
static TextLayer *wind_label;
static TextLayer *swell_label;
static TextLayer *wind_units_label;
static TextLayer *swell_units_label;

// Fonts
static GFont s_surf_font_24;
static GFont s_symbol_font_18;

static char wind_strength[] = "100";
static char swell_strength[] = "100";
static char star_string[2*MAX_SURF_RATING + 1] = "                    ";

////////////////////////////////////////////

// text layers to display the data
TextLayer *tide_event_text_layer;

TideData tide_data;
static SurfData surf_data;

int current_height;

// string buffers
static char height_text[10];
static char error_message[50];

float my_sqrt(const float num) {
  const uint MAX_STEPS = 40;
  const float MAX_ERROR = 0.001;
  
  float answer = num;
  float ans_sqr = answer * answer;
  uint step = 0;
  while((ans_sqr - num > MAX_ERROR) && (step++ < MAX_STEPS)) {
    answer = (answer + (num / answer)) / 2;
    ans_sqr = answer * answer;
  }
  return answer;
}

float getRadius(int a, int b, int theta) {
     double s = sin_lookup(DEG_TO_TRIGANGLE(theta))/TRIG_MAX_RATIO;
     double c = cos_lookup(DEG_TO_TRIGANGLE(theta))/TRIG_MAX_RATIO;
     return (a * b) / (my_sqrt((a*a)*(s*s)+(b*b)*(c*c)));
}

static void hand_update_radius(int theta, GRect bounds, int hand, GPathInfo *info){
  // 90, bounds, 1
  int b = bounds.size.w / 2;
  int a = bounds.size.h / 2;
  
  
  float value = (getRadius(a, b, TRIGANGLE_TO_DEG(theta)));
  int max = (int)value;
  
  if(hand == 2){
    max = max / 2;
  }
  
  max = (max * (-1)) - 8;
  int min = max + 5;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "When angle is %d and the hand is %d the length is %d", theta, hand, max);
  
  info->points[2].y = min;
  info->points[3].y = max;
  info->points[4].y = max;
  info->points[5].y = min;
  
 }


int has_data = 0;

static void update_display_data() {
    time_t t = time(NULL);

    current_height = get_tide_at_time(&tide_data, t);
    int d1 = abs(current_height/100);
    int d2 = abs(current_height) - d1*100;

    //make sure the sign is right even for d1=0
    if(current_height>=0)
      snprintf(height_text,10,"%d.%d%s",d1,d2, tide_data.unit);  
    else
      snprintf(height_text,10,"-%d.%d%s",d1,d2, tide_data.unit);  

    //update the star string
    for(uint16_t i = 0; i < MAX_SURF_RATING; i++){
        if(i < surf_data.surf_rating){
          star_string[2*i + 1] = 'b';
          star_string[2*i + 2] = ' ';
        }
        else{
          star_string[2*i + 1] = '\0';
          star_string[2*i + 2] = '\0';
        }
    }

    text_layer_set_text(star_label, star_string);
    text_layer_set_text(tide_event_text_layer, height_text);
    layer_mark_dirty(window_get_root_layer(window));
  
    text_layer_set_text(wind_units_label, surf_data.wind_units);
    text_layer_set_text(swell_units_label, surf_data.swell_units);
    
    snprintf(wind_strength, sizeof(wind_strength), "%d", surf_data.wind_strength);
    text_layer_set_text(wind_label, wind_strength);
  
    snprintf(swell_strength, sizeof(swell_strength), "%d", surf_data.swell_strength);
    text_layer_set_text(swell_label, swell_strength);
  
}


//ensure that pressing back during an error dialog quits the app
void error_back_click_handler(ClickRecognizerRef recognizer, void *context) {
  window_stack_pop_all(true);
}
void error_layer_config_provider(Window *window) {
  window_single_click_subscribe(BUTTON_ID_BACK, error_back_click_handler);
}


static void push_error(char *error_message){

    Window *error_window = window_create();
    window_set_click_config_provider(error_window, (ClickConfigProvider) error_layer_config_provider);
    window_set_background_color(error_window, COLOR_FALLBACK(GColorOrange,GColorWhite));
    Layer *error_window_layer = window_get_root_layer(error_window);
    GRect bounds = layer_get_bounds(error_window_layer);

    TextLayer *error_text_layer = text_layer_create((GRect) { .origin = { LEFT_MARGIN, 50 }, .size = { bounds.size.w - LEFT_MARGIN, bounds.size.h } });
    text_layer_set_text_alignment(error_text_layer, GTextAlignmentCenter);
    text_layer_set_text(error_text_layer, error_message);
    text_layer_set_font(error_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
    text_layer_set_background_color(error_text_layer, GColorClear);
    layer_add_child(error_window_layer, text_layer_get_layer(error_text_layer));

    GBitmap *s_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_WARNING);
    GRect bitmap_bounds = gbitmap_get_bounds(s_icon_bitmap);
    BitmapLayer *s_icon_layer = bitmap_layer_create((GRect) { .origin = { bounds.origin.x, 20}, .size = { bounds.size.w , 30 } });
    bitmap_layer_set_bitmap(s_icon_layer, s_icon_bitmap);
    bitmap_layer_set_compositing_mode(s_icon_layer, GCompOpSet);
    layer_add_child(error_window_layer, bitmap_layer_get_layer(s_icon_layer));

    window_stack_push(error_window, true);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {

   // incoming message received
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Message was received");

  Tuple *tuple = dict_read_first(iterator);
  bool is_error = false;

  //read in the data from the message using the dictionary iterator
  while (tuple) 
  {
    switch (tuple->key) 
    {
      //tide message data
      case NAME:
        strcpy(tide_data.name,tuple->value->cstring);
        break;
      case UNIT:
        strcpy(tide_data.unit,tuple->value->cstring);
        break;
      case N_EVENTS:
        tide_data.n_events = tuple->value->int32;
        break;
      case TIMES:
        memcpy(tide_data.times.bytes, tuple->value->data, sizeof(IntByteArray));
        break;
      case HEIGHTS:
        memcpy(tide_data.heights.bytes, tuple->value->data, sizeof(IntByteArray));
        break;
      case EVENTS:
        memcpy(tide_data.events, tuple->value->data, MAX_TIDE_EVENTS);
        break;
      case ERROR_MSG:
        strcpy(error_message,tuple->value->cstring);
        is_error = true;
        break;

        //surf message data
      case WIND_STRENGTH:
        surf_data.wind_strength = tuple->value->uint16;
        break;
      case WIND_DIRECTION:
        surf_data.wind_direction = tuple->value->uint16;
        break;
      case WIND_UNITS:
        strcpy(surf_data.wind_units,tuple->value->cstring);
        break;
      case SWELL_STRENGTH:
        surf_data.swell_strength = tuple->value->uint16;
        break;
      case SWELL_DIRECTION:
        surf_data.swell_direction = tuple->value->uint16;
        break;
      case SWELL_UNITS:
        strcpy(surf_data.swell_units,tuple->value->cstring);
        break;
      case SURF_RATING:
        surf_data.surf_rating = tuple->value->uint16;
        break;
      case MIN_SURF_HEIGHT:
        surf_data.min_surf_height = tuple->value->uint16;
        break;
      case MAX_SURF_HEIGHT:
        surf_data.max_surf_height = tuple->value->uint16;
        break;

    }

    tuple = dict_read_next(iterator);
  }

  if(is_error == false) {

    has_data = 1;
    store_tide_data(&tide_data);
  }
  else { // push an error message window to the stack
      push_error(error_message);
  }

  update_display_data();

}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void wave_layer_update_callback(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  // for every pixel in the layer draw a vertical line at the wave height
  const int max_wave_height = 10;
  const int offset = 10;
  const float cycles = 1.5;


  // There might be a bug in this part of the code... Check it properly layer after getting some sleep.
  time_t t = time(NULL);
  int low_t = get_time_to_next_low(&tide_data, t);
  int T = get_tide_period(&tide_data, t);

  int pixel_period = bounds.size.w / cycles;
  int pixel_offset = pixel_period * (t - low_t) / (2*T); //the phase indicates how close it is to a low tide in pixels

  graphics_context_set_stroke_color(ctx,GColorPictonBlue);          
  graphics_context_set_stroke_width(ctx,1);
  for(int i = 0; i < bounds.size.w; i++){
    int height = cos_lookup((i + pixel_offset) * TRIG_MAX_ANGLE * cycles / bounds.size.w) * max_wave_height / TRIG_MAX_RATIO;
    graphics_draw_line(ctx,GPoint(i, bounds.size.h),
                           GPoint(i, bounds.size.h / 2 - height - offset));      
  } 

  //draw the tick marker line
  graphics_context_set_stroke_color(ctx,GColorBlack);  
  graphics_context_set_stroke_width(ctx,1);    
  graphics_draw_line(ctx,GPoint(bounds.size.w / 3, 4),
                         GPoint(bounds.size.w / 3, bounds.size.h));
  graphics_draw_circle(ctx,GPoint(bounds.size.w / 3, 2),2);

}

// Update the three clock hands
static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(window_get_root_layer (window));
  GPoint center = grect_center_point(&bounds);
  
  // Get the current time
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  // Draw Wind Hand
  int wind_direction = surf_data.wind_direction;
  GRect wind_hand = GRect(((bounds.size.w / 4) * 3) - 25, ((bounds.size.h / 2) - 12), 40, 40);
  graphics_context_set_fill_color(ctx, GColorCobaltBlue);
  graphics_fill_radial(ctx, wind_hand, GOvalScaleModeFillCircle, 5, DEG_TO_TRIGANGLE(wind_direction - 15), DEG_TO_TRIGANGLE(wind_direction + 15));
  
  // Draw Swell Hand
  int swell_direction = surf_data.swell_direction;
  GRect swell_hand = GRect((bounds.size.w / 4) - 15, ((bounds.size.h / 2) - 12), 40, 40);
  graphics_context_set_fill_color(ctx, GColorCobaltBlue);
  graphics_fill_radial(ctx, swell_hand, GOvalScaleModeFillCircle, 5, DEG_TO_TRIGANGLE(swell_direction - 15), DEG_TO_TRIGANGLE(swell_direction + 15));
  
  const int hand_stroke_width = 2;
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, hand_stroke_width);

  int hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
  int minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;

  hand_update_radius(hour_angle, bounds, 2, &HOUR_HAND);
  hand_update_radius(minute_angle, bounds, 1, &MINUTE_HAND);
  
  // Draw hour hand
  gpath_rotate_to(s_hour_hand, hour_angle);
  gpath_draw_outline(ctx, s_hour_hand);
  
  // Draw minute hand
  gpath_rotate_to(s_minute_hand, minute_angle);
  gpath_draw_outline(ctx, s_minute_hand);
  
  const int hand_fill_width = 6;
  graphics_context_set_stroke_width(ctx, hand_fill_width);
  
  GPoint minute_hand_fill = gpoint_from_polar(GRect((bounds.size.w / 2) - 8, (bounds.size.h / 2) - 8, 17, 17), GOvalScaleModeFitCircle, TRIG_MAX_ANGLE * t->tm_min / 60);
  graphics_draw_line(ctx, center, minute_hand_fill);
  
  GPoint hour_hand_fill = gpoint_from_polar(GRect((bounds.size.w / 2) - 8, (bounds.size.h / 2) - 8, 17, 17), GOvalScaleModeFitCircle, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  graphics_draw_line(ctx, center, hour_hand_fill);
  
}

static void canvas_update_proc(Layer *this_layer, GContext *ctx) {
  
  GRect bounds = layer_get_bounds(window_get_root_layer (window));
  
  // Draw wind circle
  graphics_context_set_stroke_color(ctx, GColorCobaltBlue);
  graphics_draw_circle(ctx, GPoint(((bounds.size.w / 4) * 3) - 5, ((bounds.size.h / 2) + 8)), 20);
  graphics_draw_circle(ctx, GPoint(((bounds.size.w / 4) * 3) - 5, ((bounds.size.h / 2) + 8)), 15);
  
  // Draw swell circle
  graphics_context_set_stroke_color(ctx, GColorCobaltBlue);
  graphics_draw_circle(ctx, GPoint((bounds.size.w / 4) + 5, ((bounds.size.h / 2) + 8)), 20);
  graphics_draw_circle(ctx, GPoint((bounds.size.w / 4) + 5, ((bounds.size.h / 2) + 8)), 15);
  
  // Draw large ticks
  for (int c = 0; c < 4; c++){
    #if defined(PBL_BW)
      graphics_context_set_stroke_color(ctx, GColorBlack);
    #elif defined(PBL_COLOR)
      graphics_context_set_stroke_color(ctx, GColorBlack);
    #endif
    gpath_rotate_to(s_large_ticks, ((TRIG_MAX_ANGLE/4) * c ) + (TRIG_MAX_ANGLE/2));
    gpath_draw_outline(ctx, s_large_ticks);
  }
  
  /*
  // Draw wind ticks
  for (int c = 0; c < 4; c++){
    #if defined(PBL_BW)
      graphics_context_set_stroke_color(ctx, GColorCobaltBlue);
    #elif defined(PBL_COLOR)
      graphics_context_set_stroke_color(ctx, GColorCobaltBlue);
    #endif
    gpath_rotate_to(s_wind_ticks, ((TRIG_MAX_ANGLE/4) * c ) + (TRIG_MAX_ANGLE/2));
    gpath_draw_outline(ctx, s_wind_ticks);
  }
  
  // Draw swell ticks
  for (int c = 0; c < 4; c++){
    #if defined(PBL_BW)
      graphics_context_set_stroke_color(ctx, GColorCobaltBlue);
    #elif defined(PBL_COLOR)
      graphics_context_set_stroke_color(ctx, GColorCobaltBlue);
    #endif
    gpath_rotate_to(s_swell_ticks, ((TRIG_MAX_ANGLE/4) * c ) + (TRIG_MAX_ANGLE/2));
    gpath_draw_outline(ctx, s_swell_ticks);
  }
  */
  
  // Draw small ticks
  for (int c = 1; c < 12; c++){
    #if defined(PBL_BW)
      graphics_context_set_fill_color(ctx, GColorBlack);
      graphics_context_set_stroke_color(ctx, GColorBlack);
    #elif defined(PBL_COLOR)
      graphics_context_set_fill_color(ctx, GColorBlack);
      graphics_context_set_stroke_color(ctx, GColorBlack);
    #endif
    if((c != 3) || (c != 6) || (c != 6)){
      gpath_rotate_to(s_small_ticks, ((TRIG_MAX_ANGLE/12) * c ) + (TRIG_MAX_ANGLE/2));
      gpath_draw_outline(ctx, s_small_ticks);
    }
  }
  
  graphics_fill_radial(ctx, GRect((bounds.size.w / 2) - 3, (bounds.size.h / 2) - 3, 5, 5), GOvalScaleModeFitCircle, 5, 0, TRIG_MAX_ANGLE);
  
}


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
  text_layer_set_text(surf_label, "5-8 ft");
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
  snprintf(swell_strength, sizeof(swell_strength), "%d", surf_data.swell_strength);
  text_layer_set_text(swell_label, swell_strength);
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


  //if there is already data cached and it is valid then load it
  if(load_tide_data(&tide_data)) {
    time_t t = time(NULL);
    current_height = get_tide_at_time(&tide_data, t);
    if(current_height != -1){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Valid cached data found.");

      has_data = 1;

      update_display_data();
    }
    else{
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Cached data was found but it is out of date.");
    }
  }
  
  if(has_data != 1){
    //load animation
  }

}


static void destroy_layers(){
  layer_destroy(wave_layer);
  text_layer_destroy(tide_event_text_layer);

  // Destroy Layers
  layer_destroy(s_canvas_layer);
  layer_destroy(s_hands_layer);
  text_layer_destroy(surf_label);
  text_layer_destroy(star_label);
  
  // Destroy fonts
  fonts_unload_custom_font(s_surf_font_24);
  fonts_unload_custom_font(s_symbol_font_18);
}

// Function to update the time
static void update_time() {
  // Get the current time
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  // Update the hands layer
  layer_mark_dirty(s_hands_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}


static void init(void) {

  // Create coordinate paths
  s_large_ticks = gpath_create(&LARGE_TICKS);
  s_small_ticks = gpath_create(&SMALL_TICKS);

  s_hour_hand = gpath_create(&HOUR_HAND);
  s_minute_hand = gpath_create(&MINUTE_HAND);
  /*
  s_wind_ticks = gpath_create(&WIND_TICKS);
  s_swell_ticks = gpath_create(&WIND_TICKS);
  */

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
  });

  const bool animated = true;

  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  if(!bluetooth_connection_service_peek()){
    push_error("No phone connection");
  }

  window_stack_push(window, animated);

  // Center the coordinate paths
  GRect bounds = layer_get_bounds(window_get_root_layer (window));
  GPoint center = grect_center_point(&bounds);
  gpath_move_to(s_large_ticks, center);
  gpath_move_to(s_small_ticks, center);
  gpath_move_to(s_hour_hand, center);
  gpath_move_to(s_minute_hand, center);
  /*
  gpath_move_to(s_wind_ticks, GPoint(((bounds.size.w / 4) * 3) - 5, ((bounds.size.h / 2) + 8)));
  gpath_move_to(s_swell_ticks, GPoint((bounds.size.w / 4) + 5, ((bounds.size.h / 2) + 8)));
  */
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
  // Update the initial time
  update_time();

}

static void deinit(void) {
  window_destroy(window);
  destroy_layers();
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
