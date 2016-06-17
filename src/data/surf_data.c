#include "surf_data.h"

void receive_surf_data(DictionaryIterator *iterator, void *context) {

  SurfData *surf_data = (SurfData*)context;

  // incoming message received
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Message was received");

  Tuple *tuple = dict_read_first(iterator);

  //read in the data from the message using the dictionary iterator
  while (tuple) 
  {
    switch (tuple->key) 
    {
      //tide message data
      case NAME:
        strncpy(surf_data->name,tuple->value->cstring, MAX_NAME_LEN);
        break;
      case TIDE_UNITS:
        strncpy(surf_data->tide_units,tuple->value->cstring, MAX_UNIT_LEN);
        break;
      case TIDE_1_TIME:
        surf_data->tide_points[0].time = tuple->value->uint16;
        break;
      case TIDE_1_HEIGHT:
        surf_data->tide_points[0].height = tuple->value->int16;
        break;
      case TIDE_1_STATE:
        surf_data->tide_points[0].state = tuple->value->uint8;
        break;
      case TIDE_2_TIME:
        surf_data->tide_points[1].time = tuple->value->uint16;
        break;
      case TIDE_2_HEIGHT:
        surf_data->tide_points[1].height = tuple->value->int16;
        break;
      case TIDE_2_STATE:
        surf_data->tide_points[1].state = tuple->value->uint8;
        break;

        //surf message data
      case WIND_STRENGTH:
        surf_data->wind.magnitude = tuple->value->uint16;
        break;
      case WIND_DIRECTION:
        surf_data->wind.direction = tuple->value->uint16;
        break;
      case WIND_UNITS:
        strncpy(surf_data->wind_units,tuple->value->cstring, MAX_UNIT_LEN);
        break;

      case SWELL_HEIGHT:
        surf_data->swell.magnitude = tuple->value->uint16;
        break;
      case SWELL_DIRECTION:
        surf_data->swell.direction = tuple->value->uint16;
        break;
      case SWELL_UNITS:
        strncpy(surf_data->swell_units,tuple->value->cstring, MAX_UNIT_LEN);
        break;

      case SOLID_RATING:
        surf_data->solid_rating = tuple->value->uint16;
        break;
      case FADED_RATING:
        surf_data->faded_rating = tuple->value->uint16;
        break;

      case MIN_SURF_HEIGHT:
        surf_data->min_surf_height = tuple->value->uint16;
        break;
      case MAX_SURF_HEIGHT:
        surf_data->max_surf_height = tuple->value->uint16;
        break;

    }

    tuple = dict_read_next(iterator);
  }

}


int16_t get_tide_at_time(time_t t, TidePoint *tide_points, int n_points){
  // first locate the tide events occuring before and after the time
  int t1=-1,t2=-1,h1=0,h2=0;

  for(int i=1; i < n_points; i++){

    if(tide_points[i-1].time < t && tide_points[i].time > t){
      t1 = tide_points[i-1].time;
      t2 = tide_points[i].time;
      h1 = tide_points[i-1].height;
      h2 = tide_points[i].height;
    }
  }

  //if it couldn't be found then the data is not current
  if(t1 < 0){
    return -9999;
  }

  // Using these calculate the current height using a fitted cosing approximation

  int A = TRIG_MAX_ANGLE * ((float)(t - t1) / (t2 - t1) + 1) / 2;

  int16_t h = h1 + (h2 - h1) * ((float)cos_lookup(A) / TRIG_MAX_RATIO + 1.0) / 2;

  return h;

}
