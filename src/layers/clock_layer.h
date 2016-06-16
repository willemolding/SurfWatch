#pragma once

#include <pebble.h>

typedef Layer ClockLayer;

ClockLayer* clock_layer_create(const GRect frame);

void clock_layer_destroy(ClockLayer *clock_layer);

static clock_layer_update(ClockLayer *clock_layer, GContext *ctx);