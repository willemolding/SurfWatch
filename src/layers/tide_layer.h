#pragma once

#include <pebble.h>

// A tide layer is a way of visualising tide data. It displys a sine-wave type curve 
// indicating where the current time is in the tide cycle. It also displays the current height.

typedef Layer TideLayer;

TideLayer* tide_layer_create(const GRect frame);

void tide_layer_destroy(TideLayer *tide_layer);

static tide_layer_update(TideLayer *tide_layer, GContext *ctx);
