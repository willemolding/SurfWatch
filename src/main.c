#include <pebble.h>
#include "tick_path.h"

#include "data/surf_data.h"
#include "windows/main_window.h"

static SurfData surf_data;

int current_height;

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
     float s = (float)sin_lookup(DEG_TO_TRIGANGLE(theta))/TRIG_MAX_RATIO;
     float c = (float)cos_lookup(DEG_TO_TRIGANGLE(theta))/TRIG_MAX_RATIO;
     return (a * b) / (my_sqrt((a*a)*(s*s)+(b*b)*(c*c)));
}

static void hand_update_radius(int theta, GRect bounds, int hand, GPathInfo *info){
  int b = bounds.size.w;
  int a = bounds.size.h;
  
  float value = (getRadius(a, b, theta)) / 2;
  
  int max = (int)value - 8;
  if(hand == 2){
    max = max / 2;
  }
  int min;
  if(max >= 0){
    max = (max * (-1));
    min = max + 5;
  } else {
    max = (max * (-1));
    min = max - 5;
  }
  
  info->points[2].y = min;
  info->points[3].y = max;
  info->points[4].y = max;
  info->points[5].y = min;
  
 }

static void update_display_data() {
    time_t t = time(NULL); 

    //update the star string
    for(uint16_t i = 0; i < MAX_SURF_RATING; i++){
        if(i < surf_data.surf_rating){
          if(i < surf_data.surf_rating - surf_data.wind_rating_penalty){
            star_string[2*i + 1] = 'w';
            star_string[2*i + 2] = ' ';
          }
          else{
            star_string[2*i + 1] = 'o';
            star_string[2*i + 2] = ' ';
          }

        }
        else{
          star_string[2*i + 1] = '\0';
          star_string[2*i + 2] = '\0';
        }
    }

    //update the height string
    snprintf(wave_height_string, sizeof(wave_height_string), "%d-%d ", 
      surf_data.min_surf_height, surf_data.max_surf_height);

    strcat(wave_height_string, surf_data.swell_units);

    text_layer_set_text(star_label, star_string);
    text_layer_set_text(tide_event_text_layer, height_text);
  
    text_layer_set_text(wind_units_label, surf_data.wind_units);
    text_layer_set_text(swell_units_label, surf_data.swell_units);
    
    snprintf(wind_strength, sizeof(wind_strength), "%d", surf_data.wind_strength);
    text_layer_set_text(wind_label, wind_strength);
  
    snprintf(swell_height, sizeof(swell_height), "%d", surf_data.swell_height);
    text_layer_set_text(swell_label, swell_height);
  
}


static void inbox_received_callback(DictionaryIterator *iter, void *context) {
  receive_surf_data(iter, context);
  update_display_data();
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}



static void destroy_layers(){

}


static void init(void) {

  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  if(!bluetooth_connection_service_peek()){
        APP_LOG(APP_LOG_LEVEL_DEBUG, "No Phone Connection!");
  }

  main_window_load();  

}

static void deinit(void) {
  //TODO: add cleanup code
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
