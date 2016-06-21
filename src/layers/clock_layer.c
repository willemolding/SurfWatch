#include "clock_layer.h"

// Coordinate Paths
static GPath *s_large_ticks;
static GPath *s_small_ticks;

static void clock_layer_update(ClockLayer *clock_layer, GContext *ctx){

  GRect bounds = layer_get_bounds(clock_layer);
  GPoint center = grect_center_point(&bounds);
  
  // Get the current time
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  const int hand_stroke_width = 2;
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, hand_stroke_width);
  
  // Draw the minute hand
  const int hand_fill_width = 3;
  graphics_context_set_stroke_width(ctx, hand_fill_width);
  GPoint minute_hand_fill = gpoint_from_polar(GRect(20, 20, bounds.size.w - 40, bounds.size.w - 40), GOvalScaleModeFitCircle, TRIG_MAX_ANGLE * t->tm_min / 60);
  graphics_draw_line(ctx, center, minute_hand_fill);
  
  // Draw the hour hand
  graphics_context_set_stroke_width(ctx, hand_fill_width);
  GPoint second_hand_fill = gpoint_from_polar(GRect(40, 40, bounds.size.w - 80, bounds.size.w - 80), GOvalScaleModeFitCircle, TRIG_MAX_ANGLE * t->tm_hour / 60);
  graphics_draw_line(ctx, center, second_hand_fill);
  
  // Draw the second hand
  graphics_context_set_stroke_width(ctx, hand_fill_width);
  #if defined(PBL_COLOR)
    graphics_context_set_stroke_color(ctx, GColorDarkCandyAppleRed);
  #endif
  GPoint hour_hand_fill = gpoint_from_polar(GRect(20, 20, bounds.size.w - 40, bounds.size.w - 40), GOvalScaleModeFitCircle, TRIG_MAX_ANGLE * t->tm_sec / 60);
  graphics_draw_line(ctx, center, hour_hand_fill);
  
  // Draw inner circle
  uint16_t outer_clock_radius = 4;
  uint16_t inner_clock_radius = outer_clock_radius - 2;
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, center, outer_clock_radius);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, inner_clock_radius);
  
  // Draw large ticks
  const int large_ticks_stroke_width = 4;
  graphics_context_set_stroke_width(ctx, large_ticks_stroke_width);
  for (int c = 0; c < 4; c++){
    #if defined(PBL_BW)
      graphics_context_set_stroke_color(ctx, GColorBlack);
    #elif defined(PBL_COLOR)
      graphics_context_set_stroke_color(ctx, GColorBlack);
    #endif
    gpath_rotate_to(s_large_ticks, ((TRIG_MAX_ANGLE/4) * c ) + (TRIG_MAX_ANGLE/2));
    gpath_draw_outline(ctx, s_large_ticks);
  }
  
  // Draw small ticks
  const int small_ticks_stroke_width = 3;
  graphics_context_set_stroke_width(ctx, small_ticks_stroke_width);
  for (int c = 1; c < 12; c++){
    #if defined(PBL_BW)
      graphics_context_set_fill_color(ctx, GColorBlack);
      graphics_context_set_stroke_color(ctx, GColorBlack);
    #elif defined(PBL_COLOR)
      graphics_context_set_fill_color(ctx, GColorBlack);
      graphics_context_set_stroke_color(ctx, GColorBlack);
    #endif
    if((c != 3) || (c != 6) || (c != 6)){
      gpath_rotate_to(s_small_ticks, ((TRIG_MAX_ANGLE/12) * c ) + (TRIG_MAX_ANGLE/2));
      gpath_draw_outline(ctx, s_small_ticks);
    }
  }

}



ClockLayer* clock_layer_create(const GRect frame){
	ClockLayer *clock_layer = layer_create(frame);
	layer_set_update_proc(clock_layer,clock_layer_update);

  //Create coordinate paths
  s_large_ticks = gpath_create(&LARGE_TICKS);
  s_small_ticks = gpath_create(&SMALL_TICKS);

  // Center the coordinate paths
  GPoint center = grect_center_point(&frame);
  gpath_move_to(s_large_ticks, center);
  gpath_move_to(s_small_ticks, center);
  
	return clock_layer;
}

void clock_layer_destroy(ClockLayer *clock_layer){
  layer_destroy(clock_layer);
}
