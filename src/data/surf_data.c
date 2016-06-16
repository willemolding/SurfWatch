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
