#pragma once

#include <pebble.h>
#include "tick_path.h"


typedef Layer ClockLayer;

ClockLayer* clock_layer_create(const GRect frame);

void clock_layer_destroy(ClockLayer *clock_layer);
