#include "tide_detail_menu.h"

Window *setup_detail_window(){

 detail_window = window_create();

 Layer *layer = window_get_root_layer(detail_window);
 GRect bounds = layer_get_frame(layer);

  // Create the menu layer
  menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

   menu_layer_set_click_config_onto_window(menu_layer, detail_window);

  layer_add_child(layer, menu_layer_get_layer(menu_layer));

  return detail_window;

}

uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data){
	return 1;
}

uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data){
	return tide_data.n_events;
}

int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data){
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data){
    menu_cell_basic_header_draw(ctx, cell_layer, "Highs and Lows");
}

void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data){
	int data_index = cell_index->row;

	char height_string[5];
	char time_string[20];

	time_t t = tide_data.times.values[data_index];

    if(clock_is_24h_style()) {
      strftime(time_string, 20, "%H:%M %b %d", localtime(&t));
    }
    else {
      strftime(time_string, 20, "%I:%M %p %b %d", localtime(&t));
    }

    int x = tide_data.heights.values[data_index];
    int d1 = abs(x/100);
    int d2 = abs(x) - d1*100;

    //make sure the sign is right even for d1=0
    if(x>=0)
      snprintf(height_string,10,"%d.%d%s",d1,d2, tide_data.unit);  
    else
      snprintf(height_string,10,"-%d.%d%s",d1,d2, tide_data.unit);  


    menu_cell_basic_draw(ctx, cell_layer, height_string, time_string, NULL);
}

void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data){

}