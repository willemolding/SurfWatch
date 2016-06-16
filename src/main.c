#include <pebble.h>
#include "tick_path.h"

#include "data/surf_data.h"
#include "windows/main_window.h"

static SurfData surf_data;

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
  receive_surf_data(iter, context);
  // update_display_data();
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}


static void init(void) {

  // app_message_register_inbox_received(inbox_received_callback);
  // app_message_register_inbox_dropped(inbox_dropped_callback);
  // app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  if(!bluetooth_connection_service_peek()){
        APP_LOG(APP_LOG_LEVEL_DEBUG, "No Phone Connection!");
  }

  // testing data
  surf_data.swell.direction = 90;
  surf_data.swell.magnitude = 3;
  strcpy(surf_data.swell_units, "ft");

  surf_data.wind.direction = 280;
  surf_data.wind.magnitude = 11;
  strcpy(surf_data.wind_units, "kts");

  surf_data.solid_rating = 1;
  surf_data.faded_rating = 2;
  surf_data.min_surf_height = 1;
  surf_data.max_surf_height = 2;

  main_window_push(&surf_data);  

}

static void deinit(void) {
  //TODO: add cleanup code
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
