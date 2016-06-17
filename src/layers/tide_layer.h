#pragma once

#include <pebble.h>
#include "../data/surf_data.h"

// A tide layer is a way of visualising tide data. It displys a sine-wave type curve 
// indicating where the current time is in the tide cycle. It also displays the current height.

typedef Layer TideLayer;

typedef struct TideLayerData {
	TidePoint *points;
	uint16_t n_points;
	char *units;
} TideLayerData;


TideLayer* tide_layer_create(const GRect frame);

void tide_layer_destroy(TideLayer *tide_layer);

void tide_layer_set_points(TideLayer *tide_layer, TidePoint *points, uint16_t n_points);

void tide_layer_set_units(TideLayer *tide_layer, char *units);


