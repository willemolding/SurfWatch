#pragma once

#define MAX_TIDE_EVENTS 20
#define MAX_NAME_LENGTH 48

#define TIDE_ERROR -100

enum {
    NAME,
    UNIT,
    N_EVENTS,
    TIMES,
    HEIGHTS,
    EVENTS,
    ERROR_MSG
  };


typedef union IntByteArray // this allows us to write as bytes (from app message) and read as an int array
{
	int values[MAX_TIDE_EVENTS];
	char bytes[MAX_TIDE_EVENTS*4];
} IntByteArray;


typedef struct{
	IntByteArray times;
	IntByteArray heights;
	char events[MAX_TIDE_EVENTS];
	char name[MAX_NAME_LENGTH];
	char unit[3];
	int n_events;
} TideData;

void store_tide_data(TideData *src);

bool load_tide_data(TideData *dest);

int find_min(int *array, int n_elements);

int find_max(int *array, int n_elements);

int get_tide_at_time(TideData *tide_data, time_t time);
