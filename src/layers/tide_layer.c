#include "tide_layer.h"

// text layers to display the data
TextLayer *tide_event_text_layer;

static void tide_layer_update(TideLayer *tide_layer, GContext *ctx){

  GRect bounds = layer_get_bounds(tide_layer);
  TideLayerData* data = (TideLayerData*)layer_get_data(tide_layer);

  int w = bounds.size.w;
  int h = bounds.size.h;

  int display_cycles = 1;
  int tick_marker_pos = w / 3;

  time_t now = time(NULL); //current time in UTC epoch

  // check that one or both of the tide events are still to come. If not draw nothing
    // assume events are sorted and find the next
    time_t next_tide_event_time;
    for(int i=0; i < data->n_points; i++){
      if(data->points[i].time > now){
        next_tide_event_time = data->points[i].time;

        // Calculate the time to next tide event as a phase offset 
        int period = data->points[1].time - data->points[0].time;
        int phase_offset = TRIG_MAX_ANGLE * (next_tide_event_time - now) / period; 

        //draw a line for every pixel width
        graphics_context_set_stroke_color(ctx,GColorPictonBlue);    

        for(int i = 0; i < w; i++){
          int32_t c = (i - tick_marker_pos) * display_cycles * TRIG_MAX_ANGLE / w;
          int32_t a = h * (TRIG_MAX_RATIO - cos_lookup(c + phase_offset)) / (2 * TRIG_MAX_RATIO);
          graphics_draw_line(ctx, GPoint(i, h),
                                  GPoint(i, a));
        }

        //draw the tick marker line
        graphics_context_set_stroke_color(ctx,GColorBlack);  
        graphics_context_set_stroke_width(ctx,1);    
        graphics_draw_line(ctx,GPoint(tick_marker_pos, 4),
                               GPoint(tick_marker_pos, h));
        graphics_draw_circle(ctx,GPoint(tick_marker_pos, 2),2);

        break;
      }
    }

   


}

TideLayer* tide_layer_create(const GRect frame){
	TideLayer *tide_layer = layer_create_with_data(frame, sizeof(TideLayerData));
	layer_set_update_proc(tide_layer,tide_layer_update);
	layer_mark_dirty(tide_layer);
	return tide_layer;
}

void tide_layer_destroy(TideLayer *tide_layer){
  layer_destroy(tide_layer);
}

void tide_layer_set_data(TideLayer *tide_layer, TidePoint *points, uint16_t n_points){
  TideLayerData* data = (TideLayerData*)layer_get_data(tide_layer);
  data->points = points;
  data->n_points = n_points;
}


