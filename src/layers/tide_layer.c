#include "tide_layer.h"

// text layers to display the data
TextLayer *tide_event_text_layer;

static void tide_layer_update(TideLayer *tide_layer, GContext *ctx){

  GRect bounds = layer_get_bounds(tide_layer);
  int w = bounds.size.w;
  int h = bounds.size.h;

  int cycles = 1;
  int tick_marker_pos = w / 3;

  //phase offset in trig_max_angle radians.
  //This is what varies as the time to the next tide event changes.
  // 
  int phase_offset = ; 

  //draw a line for every pixel width
  graphics_context_set_stroke_color(ctx,GColorPictonBlue);    

  for(int i = 0; i < w; i++){
    int32_t c = (i - tick_marker_pos) * cycles * TRIG_MAX_ANGLE / w;
    int32_t a = h * (TRIG_MAX_RATIO - cos_lookup(c)) / (2 * TRIG_MAX_RATIO);
    graphics_draw_line(ctx, GPoint(i, h),
                            GPoint(i, a));
  }

  //draw the tick marker line
  graphics_context_set_stroke_color(ctx,GColorBlack);  
  graphics_context_set_stroke_width(ctx,1);    
  graphics_draw_line(ctx,GPoint(tick_marker_pos, 4),
                         GPoint(tick_marker_pos, h));
  graphics_draw_circle(ctx,GPoint(tick_marker_pos, 2),2);

}

TideLayer* tide_layer_create(const GRect frame){
	TideLayer *tide_layer = layer_create(frame);
	layer_set_update_proc(tide_layer,tide_layer_update);
	layer_mark_dirty(tide_layer);
	return tide_layer;
}

void tide_layer_destroy(TideLayer *tide_layer){
  layer_destroy(tide_layer);
}

