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

  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  if(!bluetooth_connection_service_peek()){
        APP_LOG(APP_LOG_LEVEL_DEBUG, "No Phone Connection!");
  }

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
