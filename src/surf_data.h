#pragma once

 typedef struct SurfData {
 	uint16_t wind_strength;
    uint16_t wind_direction;
    char wind_units[4];
    uint16_t swell_strength;
    uint16_t swell_direction;
    char swell_units[4];
    uint16_t surf_rating;
    uint16_t surf_height;
 } SurfData;