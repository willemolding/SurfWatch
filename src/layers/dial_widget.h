#pragma once

#include <pebble.h>
#include "../data/vector.h"

#define MAX_CENTER_TEXT 10

// A dial widget is a circular  dial that is well suited to displaying data that is 
// comprised of a magnitude and direction component e.g wind speed and direction. 

// When creating a dial widget you must pass a frame. The displayed circle behaves in the 
// same way as graphics_draw_arc.

typedef Layer DialWidgetLayer;
typedef Layer DialWidgetMiniLayer;

typedef struct {
	char *units;
	Vector *vec;
} DialWidgetData;

typedef struct {
	uint16_t units;
} DialWidgetMiniData;

DialWidgetLayer* dial_widget_layer_create(GRect frame);
DialWidgetMiniLayer* dial_widget_mini_layer_create(GRect frame);

void dial_widget_layer_destroy(DialWidgetLayer *dial_widget_layer);
void dial_widget_mini_layer_destroy(DialWidgetLayer *dial_widget_layer);

void dial_widget_layer_set_unit(DialWidgetLayer *dial_widget_layer, char *units);
void dial_widget_mini_layer_set_unit(DialWidgetMiniLayer *dial_widget_mini_layer, uint16_t units);

void dial_widget_layer_set_vector(DialWidgetLayer *dial_widget_layer, Vector *vec);