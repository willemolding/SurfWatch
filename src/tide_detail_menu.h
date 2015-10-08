#pragma once
#include <pebble.h>
#include "tide_data.h"

extern TideData tide_data;

Window *detail_window;
MenuLayer *menu_layer;

Window *setup_detail_window();

uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data);

uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data);

int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data);

void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data);

void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);

void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data);