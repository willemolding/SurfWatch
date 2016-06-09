#include "dial_widget.h"

DialWidgetLayer* dial_widget_layer_create(const GRect frame){
	DialWidgetLayer *dial_widget_layer = layer_create_with_data(frame, sizeof(DialWidgetData));
	layer_set_update_proc(dial_widget_layer, dial_widget_layer_update);
	layer_mark_dirty(dial_widget_layer);
	return dial_widget_layer;
}

void dial_widget_layer_destroy(DialWidgetLayer *dial_widget_layer){
  layer_destroy(dial_widget_layer);
}

static void dial_widget_layer_update(DialWidgetLayer *dial_widget_layer, GContext *ctx){

  DialWidgetData *data = (DialWidgetData*)layer_get_data(dial_widget_layer);
  uint16_t direction = data->direction;
  graphics_context_set_fill_color(ctx, GColorCobaltBlue);


  // draw the dial outline
  graphics_draw_arc(ctx, layer_get_frame(dial_widget_layer), GOvalScaleModeFitCircle, 0, TRIG_MAX_ANGLE);

  // draw the arrow
  graphics_fill_radial(ctx, layer_get_frame(dial_widget_layer), 
  	GOvalScaleModeFitCircle, 5, 
  	DEG_TO_TRIGANGLE(direction - 15), 
  	DEG_TO_TRIGANGLE(direction + 15));

  // draw the center text
  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, 
    data->center_text,
    font,
    layer_get_frame(dial_widget_layer),
    GTextOverflowModeFill,
    GTextAlignmentCenter,
    NULL);
}

void dial_widget_set_center_text(DialWidgetLayer * dial_widget_layer, char *text){
  DialWidgetData *data = (DialWidgetData*)layer_get_data(dial_widget_layer);
	data->center_text = text;
}

void dial_widget_set_direction(DialWidgetLayer *dial_widget_layer, uint16_t direction){
  DialWidgetData *data = (DialWidgetData*)layer_get_data(dial_widget_layer);
	data->direction = direction;
}