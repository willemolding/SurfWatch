#pragma once
#include <pebble.h>
#include "tide_data.h"

#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168

#define MIN_LEVEL 30
#define MAX_LEVEL 130

#define LEFT_MARGIN 5

#define tide_event_text_layer_bounds (GRect) { .origin = { LEFT_MARGIN, 45 }, .size = { SCREEN_WIDTH - LEFT_MARGIN, 55 } }
#define at_text_layer_bounds (GRect) { .origin = { LEFT_MARGIN, 80 }, .size = { SCREEN_WIDTH - LEFT_MARGIN, 53 } }

extern Layer *blue_layer;
extern Layer *line_layer;

extern TextLayer *name_text_layer;
extern TextLayer *tide_event_text_layer;
extern TextLayer *at_text_layer;

extern int current_height;

extern int data_index;
extern int has_data;
extern int level_height; // how many pixels above the bottom to draw the blue layer
extern int min_height;
extern int max_height;

Animation *create_anim_load();

Animation *create_anim_water_level();