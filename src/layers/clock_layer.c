#include "clock_layer.h"
#include "tick_path.h"

// Coordinate Paths
static GPath *s_large_ticks;
static GPath *s_small_ticks;
static GPath *s_hour_hand;
static GPath *s_minute_hand;

static float my_sqrt(const float num) {
  const uint MAX_STEPS = 40;
  const float MAX_ERROR = 0.001;
  
  float answer = num;
  float ans_sqr = answer * answer;
  uint step = 0;
  while((ans_sqr - num > MAX_ERROR) && (step++ < MAX_STEPS)) {
    answer = (answer + (num / answer)) / 2;
    ans_sqr = answer * answer;
  }
  return answer;
}

static float getRadius(int a, int b, int theta) {
     float s = (float)sin_lookup(DEG_TO_TRIGANGLE(theta))/TRIG_MAX_RATIO;
     float c = (float)cos_lookup(DEG_TO_TRIGANGLE(theta))/TRIG_MAX_RATIO;
     return (a * b) / (my_sqrt((a*a)*(s*s)+(b*b)*(c*c)));
}

static void hand_update_radius(int theta, GRect bounds, int hand, GPathInfo *info){
  int b = bounds.size.w;
  int a = bounds.size.h;
  
  float value = (getRadius(a, b, theta)) / 2;
  
  int max = (int)value - 8;
  if(hand == 2){
    max = max / 2;
  }
  int min;
  if(max >= 0){
    max = (max * (-1));
    min = max + 5;
  } else {
    max = (max * (-1));
    min = max - 5;
  }
  
  info->points[2].y = min;
  info->points[3].y = max;
  info->points[4].y = max;
  info->points[5].y = min;
  
 }


 static void clock_layer_update(ClockLayer *clock_layer, GContext *ctx){

  GRect bounds = layer_get_bounds(clock_layer);
  GPoint center = grect_center_point(&bounds);
  
  // Get the current time
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  const int hand_stroke_width = 2;
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, hand_stroke_width);

  int hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
  int minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;

  hand_update_radius(hour_angle, bounds, 2, &HOUR_HAND);
  hand_update_radius(minute_angle, bounds, 1, &MINUTE_HAND);
  
  // Draw hour hand
  gpath_rotate_to(s_hour_hand, hour_angle);
  gpath_draw_outline(ctx, s_hour_hand);
  
  // Draw minute hand
  gpath_rotate_to(s_minute_hand, minute_angle);
  gpath_draw_outline(ctx, s_minute_hand);
  
  const int hand_fill_width = 6;
  graphics_context_set_stroke_width(ctx, hand_fill_width);
  
  GPoint minute_hand_fill = gpoint_from_polar(GRect((bounds.size.w / 2) - 8, (bounds.size.h / 2) - 8, 17, 17), GOvalScaleModeFitCircle, TRIG_MAX_ANGLE * t->tm_min / 60);
  graphics_draw_line(ctx, center, minute_hand_fill);
  
  GPoint hour_hand_fill = gpoint_from_polar(GRect((bounds.size.w / 2) - 8, (bounds.size.h / 2) - 8, 17, 17), GOvalScaleModeFitCircle, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  graphics_draw_line(ctx, center, hour_hand_fill);

  // Draw large ticks
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
  
  graphics_fill_radial(ctx, GRect((bounds.size.w / 2) - 3, (bounds.size.h / 2) - 3, 5, 5), GOvalScaleModeFitCircle, 5, 0, TRIG_MAX_ANGLE);
  
}



ClockLayer* clock_layer_create(const GRect frame){
	ClockLayer *clock_layer = layer_create(frame);
	layer_set_update_proc(clock_layer,clock_layer_update);

  //Create coordinate paths
  s_large_ticks = gpath_create(&LARGE_TICKS);
  s_small_ticks = gpath_create(&SMALL_TICKS);

  s_hour_hand = gpath_create(&HOUR_HAND);
  s_minute_hand = gpath_create(&MINUTE_HAND);

  // Center the coordinate paths
  GPoint center = grect_center_point(&frame);
  gpath_move_to(s_large_ticks, center);
  gpath_move_to(s_small_ticks, center);
  gpath_move_to(s_hour_hand, center);
  gpath_move_to(s_minute_hand, center);
  
	return clock_layer;
}

void clock_layer_destroy(ClockLayer *clock_layer){
  layer_destroy(clock_layer);
}
