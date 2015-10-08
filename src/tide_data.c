#include <pebble.h>
#include "tide_data.h"

void store_tide_data(TideData *src) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Storing tide data.");
  persist_write_string(NAME, src->name);
  persist_write_string(UNIT, src->unit);
  persist_write_int(N_EVENTS, src->n_events);
  persist_write_data(TIMES, src->times.bytes, sizeof(IntByteArray));
  persist_write_data(HEIGHTS, src->heights.bytes, sizeof(IntByteArray));
  persist_write_data(EVENTS, src->events, MAX_TIDE_EVENTS);
}

bool load_tide_data(TideData *dst) {
	if(persist_exists(NAME)){ //assume that if the name exists then all data is there.
	    APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading data from storage");
	    persist_read_string(NAME, dst->name, MAX_NAME_LENGTH);
	    persist_read_string(UNIT, dst->unit, 3);
	    dst->n_events = persist_read_int(N_EVENTS);
	    persist_read_data(TIMES, dst->times.bytes, sizeof(IntByteArray));
	    persist_read_data(HEIGHTS, dst->heights.bytes, sizeof(IntByteArray));
	    persist_read_data(EVENTS, dst->events, MAX_TIDE_EVENTS);
	}

	return persist_exists(NAME);
}

//it is less efficient to find the min/max in two passes but it makes the code more readable
int find_min(int *array, int n_elements){
	int min = array[0];
	for(int i=1; i < n_elements; i++){
	        if(array[i] < min) {
	          min = array[i];
	        }
	    }
	return min;
}

int find_max(int *array, int n_elements){
	int max = array[0];
	for(int i=1; i < n_elements; i++){
	        if(array[i] > max) {
	          max = array[i];
	        }
	    }
	return max;
}


int get_tide_at_time(TideData *tide_data, time_t t){
	// first locate the tide events occuring before and after the time
	int t1=-1,t2=-1,h1=0,h2=0;

	for(int i=1; i < tide_data->n_events; i++){
		if(tide_data->times.values[i-1] < t && tide_data->times.values[i] > t){
			t1 = tide_data->times.values[i-1];
			t2 = tide_data->times.values[i];
			h1 = tide_data->heights.values[i-1];
			h2 = tide_data->heights.values[i];
		}
	}

	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", (int)t);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", t1);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", t2);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", h1);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", h2);


	//if it couldn't be found then the data is not current
	if(t1 < 0){
		return TIDE_ERROR;
	}

	// Using these calculate the current height using a fitted cosing approximation

	int A = TRIG_MAX_ANGLE * ((float)(t - t1) / (t2 - t1) + 1) / 2;

	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", A);


	int h = h1 + (h2 - h1) * ((float)cos_lookup(A) / TRIG_MAX_RATIO + 1.0) / 2;

	return h;
}


