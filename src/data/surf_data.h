#pragma once

#define MAX_SURF_RATING 10
#define MAX_UNIT_LEN 4
#define MAX_NAME_LEN 100

 typedef struct SurfData {
    char name[MAX_NAME_LEN];

    char tide_units[MAX_UNIT_LEN];

    uint16_t tide_1_time;
    int16_t tide_1_height;
    uint8_t tide_1_state;

    uint16_t tide_2_time;
    int16_t tide_2_height;
    uint8_t tide_2_state;

    uint16_t solid_rating;
    uint16_t faded_rating;

    uint16_t swell_height;
    uint16_t swell_direction;
    char swell_units[MAX_UNIT_LEN];

 	uint16_t wind_strength;
    uint16_t wind_direction;
    char wind_units[MAX_UNIT_LEN];

    uint16_t min_surf_height;
    uint16_t max_surf_height;
 } SurfData;


enum {
    NAME, 

    TIDE_UNITS,
    TIDE_1_TIME,
    TIDE_1_HEIGHT,
    TIDE_1_STATE,
    TIDE_2_TIME,
    TIDE_2_HEIGHT,
    TIDE_2_STATE,

    SOLID_RATING,
    FADED_RATING,

    SWELL_DIRECTION,
    SWELL_HEIGHT,
    SWELL_UNITS,

    WIND_DIRECTION,
    WIND_STRENGTH,
    WIND_UNITS,

    MIN_SURF_HEIGHT,
    MAX_SURF_HEIGHT 
};