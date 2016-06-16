#include "dial_widget.h"


static void dial_widget_layer_update(DialWidgetLayer *dial_widget_layer, GContext *ctx) {

  GRect bounds = layer_get_bounds(dial_widget_layer);
  DialWidgetData *data = (DialWidgetData*)layer_get_data(dial_widget_layer);
  uint16_t direction = data->vec->direction;

  
  // string formatting
  char center_text[MAX_CENTER_TEXT];
  snprintf(center_text, MAX_CENTER_TEXT*sizeof(char), "%u\n%s", 
      data->vec->magnitude, data->units);

  graphics_context_set_fill_color(ctx, GColorCobaltBlue);
  graphics_context_set_stroke_color(ctx, GColorBlack);

  // draw the dial outline
  graphics_draw_arc(ctx, grect_inset(bounds, GEdgeInsets(1)), 
    GOvalScaleModeFitCircle, 0, DEG_TO_TRIGANGLE(360));

  graphics_draw_arc(ctx, grect_inset(bounds, GEdgeInsets(8)), 
    GOvalScaleModeFitCircle, 0, DEG_TO_TRIGANGLE(360));


  // draw the arrow
  graphics_fill_radial(ctx, bounds, 
  	GOvalScaleModeFitCircle, 8, 
  	DEG_TO_TRIGANGLE(direction - 15), 
  	DEG_TO_TRIGANGLE(direction + 15));

  // draw the center text
  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, 
    center_text,
    font,
    grect_inset(bounds, GEdgeInsets(8)),
    GTextOverflowModeWordWrap,
    GTextAlignmentCenter,
    NULL);
}

DialWidgetLayer* dial_widget_layer_create(GRect frame){
  DialWidgetLayer *dial_widget_layer = layer_create_with_data(frame, sizeof(DialWidgetData));
  layer_set_update_proc(dial_widget_layer, dial_widget_layer_update);
  layer_mark_dirty(dial_widget_layer);
  return dial_widget_layer;
}

void dial_widget_layer_destroy(DialWidgetLayer *dial_widget_layer){
  layer_destroy(dial_widget_layer);
}

void dial_widget_layer_set_unit(DialWidgetLayer *dial_widget_layer, char *units){
  DialWidgetData *data = (DialWidgetData*)layer_get_data(dial_widget_layer);
  data->units = units;

}

void dial_widget_layer_set_vector(DialWidgetLayer *dial_widget_layer, Vector *vec){
    DialWidgetData *data = (DialWidgetData*)layer_get_data(dial_widget_layer);
    data->vec = vec;
}
