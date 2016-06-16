#pragma once

#include <pebble.h>

typedef struct Vector {
    uint16_t magnitude;
    uint16_t direction; //direction should be in degrees
} Vector;