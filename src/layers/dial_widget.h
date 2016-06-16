#pragma once

#include <pebble.h>

// A dial widget is a circular  dial that is well suited to displaying data that is 
// comprised of a magnitude and direction component e.g wind speed and direction. 

// When creating a dial widget you must pass a frame. The displayed circle behaves in the 
// same way as graphics_draw_arc.

typedef Layer DialWidgetLayer;

typedef struct Vector {
    uint16_t magnitude;
    uint16_t direction; //direction should be in degrees
} Vector;

typedef struct {
	char *units;
	Vector vec;
} DialWidgetData;

DialWidgetLayer* dial_widget_layer_create(const GRect frame);

void dial_widget_layer_destroy(DialWidgetLayer *dial_widget_layer);

static void dial_widget_layer_update(DialWidgetLayer *dial_widget_layer, GContext *ctx);

void dial_widget_set_unit(DialWidgetLayer *dial_widget_layer, char *units);

void dial_widget_set_vector(DialWidgetLayer *dial_widget_layer, Vector vec);