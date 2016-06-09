#include "dial_widget.h"

DialWidgetLayer* dial_widget_layer_create(const GRect frame){
	DialWidgetLayer *dial_widget_layer = layer_create_with_data(frame, sizeof(DialWidgetData));
	layer_set_update_proc(dial_widget_layer,dial_widget_layer_update);
	layer_mark_dirty(dial_widget_layer);
	return dial_widget_layer;
}

void dial_widget_layer_destroy(DialWidgetLayer *dial_widget_layer){
  layer_destroy(dial_widget_layer);
}

static dial_widget_layer_update(DialWidgetLayer *dial_widget_layer, GContext *ctx){

  uint16_t direction = layer_get_data(dial_widget_layer)->direction;
  graphics_context_set_fill_color(ctx, GColorCobaltBlue);

  // draw the dial
  graphics_fill_radial(ctx, layer_get_frame(dial_widget_layer), 
  	GOvalScaleModeFillCircle, 5, 
  	DEG_TO_TRIGANGLE(direction - 15), 
  	DEG_TO_TRIGANGLE(direction + 15));

  // draw the center text
  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, 
    layer_get_data(dial_widget_layer)->center_text,
    font,
    layer_get_frame(dial_widget_layer),
    GTextOverflowModeFill,
    GTextAlignmentCenter,
    NULL);
}

void dial_widget_set_center_text(DialWidgetLayer * dial_widget_layer, char *text){
	layer_get_data(dial_widget_layer)->center_text = text;
}

void dial_widget_set_direction(DialWidgetLayer *dial_widget_layer, uint16_t direction){
	layer_get_data(dial_widget_layer)->direction = direction;
}