#pragma once

#include <pebble.h>

// A dial widget is a circular  dial that is well suited to displaying data that is 
// comprised of a magnitude and direction component e.g wind speed and direction. 

// When creating a dial widget you must pass a frame. The displayed circle behaves in the 
// same way as graphics_draw_arc.

typedef Layer DialWidgetLayer;

typedef struct {
	char *center_text;
	uint16_t direction;
} DialWidgetData;

DialWidgetLayer* dial_widget_layer_create(const GRect frame);

void dial_widget_layer_destroy(DialWidgetLayer *dial_widget_layer);

static dial_widget_layer_update(DialWidgetLayer *dial_widget_layer, GContext *ctx);

void dial_widget_set_center_text(DialWidgetLayer * dial_widget_layer, char *text);

void dial_widget_set_direction(DialWidgetLayer *dial_widget_layer, uint16_t direction);