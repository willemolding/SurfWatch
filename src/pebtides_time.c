#include <pebble.h>
#include "tide_data.h"
#include "app_animations.h"
#include "gpath_builder.h"

#define MAX_POINTS 512
#define N_WAVE_POINTS 10

// the text layers to display the info
static Window *window;

// layers for the load screen animation
Layer *blue_layer;
Layer *tick_layer;
Layer *hands_layer;

// text layers to display the data
TextLayer *tide_event_text_layer;

TideData tide_data;
int current_height;

// string buffers
static char timestring[20];
static char height_text[10];
static char error_message[50];

// other random global vars
int level_height = SCREEN_HEIGHT / 2; // how many pixels above the bottom to draw the blue layer
int min_height = 10000;
int max_height = 0;
int has_data = 0;
static GPath *s_my_path_ptr = NULL;

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

    text_layer_set_text(tide_event_text_layer, height_text);
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
    }

    tuple = dict_read_next(iterator);
  }

  if(is_error == false) {

  	min_height = find_min(tide_data.heights.values, tide_data.n_events);
  	max_height = find_max(tide_data.heights.values, tide_data.n_events);    

    has_data = 1;
    store_tide_data(&tide_data);

    update_display_data();
    animation_unschedule_all();
    animation_schedule(create_anim_water_level());

  }
  else { // push an error message window to the stack
      push_error(error_message);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void create_wave_path(){
  // Create GPathBuilder object
  GPathBuilder *builder = gpath_builder_create(MAX_POINTS);

  //start at point 0
  gpath_builder_move_to_point(builder, GPoint(0, 0));


  const int d = SCREEN_WIDTH/N_WAVE_POINTS;
  const int control_offset = 10;
  const int wave_height = 5; 

  for(int i = 1; i <= N_WAVE_POINTS; i++){
      gpath_builder_curve_to_point(builder, GPoint(d*i, wave_height*(i % 2)), 
                                            GPoint(d*i - d/2, wave_height*(i % 2) + control_offset), 
                                            GPoint(d*i - d/2, wave_height*(i % 2) + control_offset));
  }

  //complete square
  gpath_builder_line_to_point(builder, GPoint(SCREEN_WIDTH, SCREEN_HEIGHT));
  gpath_builder_line_to_point(builder, GPoint(0, SCREEN_HEIGHT));
  gpath_builder_line_to_point(builder, GPoint(0, SCREEN_HEIGHT));
  gpath_builder_line_to_point(builder, GPoint(0, 0));

  // Create GPath object out of our GPathBuilder object
  s_my_path_ptr = gpath_builder_create_path(builder);
  // Destroy GPathBuilder object
  gpath_builder_destroy(builder);
}

static void blue_layer_update_callback(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, COLOR_FALLBACK(GColorPictonBlue,GColorClear));
  //graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  gpath_draw_filled(ctx, s_my_path_ptr);
  // Stroke the path:
  graphics_context_set_stroke_color(ctx, GColorBlack);
  gpath_draw_outline(ctx, s_my_path_ptr);
}


static void tick_layer_update_callback(Layer *layer, GContext *ctx){
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, GColorBlack);
  for(int i = 0; i < 12; i++){
      graphics_fill_radial(ctx, grect_inset(bounds, GEdgeInsets(-3)), GOvalScaleModeFitCircle, 10,
        DEG_TO_TRIGANGLE(i*(360/12) - 1), DEG_TO_TRIGANGLE(i*(360/12) + 1));
  }
}

static void hands_layer_update_callback(Layer *layer, GContext *ctx){
  GRect bounds = layer_get_bounds(layer);
  GPoint center = GPoint(180/2,180/2);

  // graphics_fill_radial(ctx, grect_inset(bounds, GEdgeInsets(80)), GOvalScaleModeFitCircle, 20,
  //       DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));

  time_t timestamp = time(NULL);
  struct tm *t = localtime(&timestamp);

  int32_t hour_angle = TRIG_MAX_ANGLE * (t->tm_hour % 12) / 12;
  int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;

  GPoint hour_point = gpoint_from_polar(grect_inset(bounds, GEdgeInsets(30)), 
    GOvalScaleModeFitCircle,
    DEG_TO_TRIGANGLE(hour_angle));

  GPoint minute_point = gpoint_from_polar(grect_inset(bounds, GEdgeInsets(20)), 
    GOvalScaleModeFitCircle,
    DEG_TO_TRIGANGLE(minute_angle));

  graphics_context_set_stroke_width(ctx,5);
  graphics_draw_line(ctx,center, hour_point);
  graphics_context_set_stroke_width(ctx,3);
  graphics_draw_line(ctx,center, minute_point);


}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  create_wave_path();

  //window_set_background_color(window, COLOR_FALLBACK(GColorPictonBlue, GColorWhite));
  GRect bounds = layer_get_bounds(window_layer);

  //add the blue layer at the base
  blue_layer = layer_create(GRect(bounds.origin.x, SCREEN_HEIGHT - level_height, bounds.size.w, bounds.size.h));
  layer_set_update_proc(blue_layer, blue_layer_update_callback);
  layer_add_child(window_layer, blue_layer);

  //tick_layer
  tick_layer = layer_create(bounds);
  layer_set_update_proc(tick_layer, tick_layer_update_callback);
  layer_add_child(window_layer, tick_layer);

  //hands_layer
  hands_layer = layer_create(bounds);
  layer_set_update_proc(hands_layer, hands_layer_update_callback);
  layer_add_child(window_layer, hands_layer);


  //create the event text layer
  GRect tide_event_text_layer_bounds = grect_inset(bounds, GEdgeInsets(50, 0));
  tide_event_text_layer = text_layer_create(tide_event_text_layer_bounds);
  text_layer_set_text(tide_event_text_layer, "Loading");
  text_layer_set_font(tide_event_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_background_color(tide_event_text_layer, GColorClear);
  text_layer_set_text_alignment(tide_event_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(tide_event_text_layer));


  //start the loading animation
  animation_schedule(create_anim_load());
}


static void destroy_layers(){
  layer_destroy(blue_layer);
  layer_destroy(tick_layer);
  text_layer_destroy(tide_event_text_layer);
}

static void init(void) {

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

  // displays cached data before waiting for more. This makes data still available without phone connection.
  if(load_tide_data(&tide_data)) {
    has_data = 1;
  }

  window_stack_push(window, animated);

}

static void deinit(void) {
  window_destroy(window);
  destroy_layers();
}

int main(void) {
  init();
  timestring[0] = 'A';
  timestring[1] = 'T';
  timestring[2] = ' ';

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
