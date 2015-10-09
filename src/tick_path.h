#pragma once

#include <pebble.h>

static const GPathInfo LARGE_TICKS = {
  2,
  (GPoint []) {
    { 0, 86 },
    { 0, 75 }
  }
};

static const GPathInfo SMALL_TICKS = {
  2,
  (GPoint []) {
    { 0, 86 },
    { 0, 78 }
  }
};

static const GPathInfo HOUR_HAND = {
 9,
 (GPoint []) {
   { 1, 7 },
   { 4, 4 },
   { 4, -40 },
   { 1, -45 },
   { -1, -45 },
   { -4, -40 },
   { -4, 4 },
   { -1, 7 },
   { 1, 7 }
 }
};

// Minute hand outline coordinate path
static const GPathInfo MINUTE_HAND = {
 9,
 (GPoint []) {
   { 1, 7 },
   { 4, 4 },
   { 4, -75 },
   { 1, -80 },
   { -1, -80 },
   { -4, -75 },
   { -4, 4 },
   { -1, 7 },
   { 1, 7 }
 }
};

static const GPathInfo WIND_TICKS = {
  2,
  (GPoint []) {
    { 0, 20 },
    { 0, 17 }
  }
};