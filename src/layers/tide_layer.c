#include "tide_layer.h"

TideLayer* tide_layer_create(const GRect frame){
	TideLayer *tide_layer = layer_create(frame);
	layer_set_update_proc(tide_layer,dial_widget_layer_update);
	layer_mark_dirty(tide_layer);
	return tide_layer;
}

void tide_layer_destroy(TideLayer *tide_layer){
  layer_destroy(tide_layer);
}

static tide_layer_update(TideLayer *tide_layer, GContext *ctx){

  GRect bounds = layer_get_bounds(tide_layer);

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
