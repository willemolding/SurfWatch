#include <pebble.h>
#include "tide_data.h"

#define SCREEN_WIDTH 180
#define SCREEN_HEIGHT 180
#define LEFT_MARGIN 30

// the text layers to display the info
static Window *window;

// layers for the load screen animation
Layer *wave_layer;

// text layers to display the data
TextLayer *tide_event_text_layer;

TideData tide_data;
int current_height;

// string buffers
static char height_text[10];
static char error_message[50];

// other random global vars
int level_height = SCREEN_HEIGHT / 2; // how many pixels above the bottom to draw the blue layer
int min_height = 10000;
int max_height = 0;
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

  if(has_data == 1){ //don't bother if there is already valid data cached on the watch
    return;
  }

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
  }
  else { // push an error message window to the stack
      push_error(error_message);
  }
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


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  //window_set_background_color(window, COLOR_FALLBACK(GColorPictonBlue, GColorWhite));
  GRect bounds = layer_get_bounds(window_layer);

  //create the event text layer
  GRect tide_event_text_layer_bounds = GRect(SCREEN_WIDTH/3 - 40, SCREEN_HEIGHT * 3 / 4 - 20,
                                             SCREEN_WIDTH/3 + 20, SCREEN_HEIGHT * 3 / 4 + 20);
  tide_event_text_layer = text_layer_create(tide_event_text_layer_bounds);
  text_layer_set_text(tide_event_text_layer, "Loading");
  text_layer_set_font(tide_event_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_background_color(tide_event_text_layer, GColorClear);
  text_layer_set_text_alignment(tide_event_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(tide_event_text_layer));


  //add the wave layer at the base
  GRect wave_layer_bounds = grect_inset(bounds, GEdgeInsets(SCREEN_HEIGHT * 3 / 4, 0, 0));
  wave_layer = layer_create(wave_layer_bounds);
  layer_set_update_proc(wave_layer, wave_layer_update_callback);
  layer_add_child(window_layer, wave_layer);


  //if there is already data cached and it is valid then load it
  if(load_tide_data(&tide_data)) {
    time_t t = time(NULL);
    current_height = get_tide_at_time(&tide_data, t);
    if(current_height != -1){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Valid cached data found.");

      min_height = find_min(tide_data.heights.values, tide_data.n_events);
      max_height = find_max(tide_data.heights.values, tide_data.n_events);    

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

  window_stack_push(window, animated);

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
