
#include "app_animations.h"
#include <pebble.h>


Animation *create_anim_load() {
  //the loading animation for the blue_layer
  GRect from_frame = layer_get_frame((Layer*) blue_layer);
  GRect to_frame = GRect(from_frame.origin.x, from_frame.origin.y + 20, from_frame.size.w, from_frame.size.h);

  PropertyAnimation *tide_rise = property_animation_create_layer_frame((Layer*) blue_layer, &from_frame, &to_frame);
  animation_set_duration((Animation*) tide_rise, 2000);

  PropertyAnimation *tide_fall = property_animation_create_layer_frame((Layer*) blue_layer, &to_frame, &from_frame);
  animation_set_duration((Animation*) tide_fall, 2000);

  Animation *load_sequence = animation_sequence_create((Animation*) tide_rise, (Animation*) tide_fall, NULL);
  animation_set_play_count(load_sequence, 20);
  return load_sequence;

}


Animation *create_anim_water_level(){
  //scrolls the water level to the current height after the load screen
  GRect from_frame = layer_get_frame((Layer*) blue_layer);

  level_height = ((current_height - min_height)*(MAX_LEVEL - MIN_LEVEL))/(max_height-min_height) + MIN_LEVEL;

  GRect from_frame_blue = layer_get_frame((Layer*) blue_layer);
  GRect to_frame_blue = GRect(from_frame_blue.origin.x, SCREEN_HEIGHT - level_height, from_frame_blue.size.w, from_frame_blue.size.h);
  PropertyAnimation *shift_blue_animation = property_animation_create_layer_frame((Layer*) blue_layer, &from_frame_blue, &to_frame_blue);
  animation_set_delay((Animation*) shift_blue_animation, 150);
  animation_set_duration((Animation*) shift_blue_animation, 1000);

  return (Animation*)shift_blue_animation;

}