#pragma once

#define MAX_SURF_RATING 10

 typedef struct SurfData {
 	uint16_t wind_strength;
    uint16_t wind_direction;
    char wind_units[4];
    uint16_t swell_strength;
    uint16_t swell_direction;
    char swell_units[4];
    uint16_t surf_rating;
    uint16_t min_surf_height;
    uint16_t max_surf_height;
 } SurfData;


 enum {
    WIND_STRENGTH = 7, //start the enum at 7 because the tide data is using the earlier numbers
    WIND_DIRECTION,
    WIND_UNITS,
    SWELL_STRENGTH,
    SWELL_DIRECTION,
    SWELL_UNITS,
    SURF_RATING,
    MIN_SURF_HEIGHT,
    MAX_SURF_HEIGHT
  };