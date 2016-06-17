#pragma once

#include <pebble.h>
#include "vector.h"

#define MAX_SURF_RATING 10
#define MAX_UNIT_LEN 4
#define MAX_NAME_LEN 100
#define N_TIDE_POINTS 2

typedef struct TidePoint{
    time_t time; //time is stored as a utc timestamp
    int16_t height; //height is stored as the decimal height multiplied by 100 e.g. 1.13 is stored as 113
    uint8_t state;
} TidePoint;


 typedef struct SurfData {
    char name[MAX_NAME_LEN];

    char tide_units[MAX_UNIT_LEN];

    TidePoint tide_points[N_TIDE_POINTS];

    uint16_t solid_rating;
    uint16_t faded_rating;

    Vector swell;
    char swell_units[MAX_UNIT_LEN];

 	Vector wind;
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

//Accepts an app message containing a JSON defined surf_data struct
//stores it in the context which must be a SurfData*
void receive_surf_data(DictionaryIterator *iterator, void *context);
