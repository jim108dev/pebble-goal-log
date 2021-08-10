#pragma once // Prevent errors from being included multiple times

#include <pebble.h>
#include "../modules/util.h"

#define DARK_BACKGROUND true
#define MAX_MENU_ITEMS 20
typedef void(ActionCallback)(uint8_t feedback, void *data);

typedef struct MenuConfig
{
    char labels[MAX_MENU_ITEMS][MAX_SMALL_TEXT_LEN];
    uint8_t max_items;
    ActionCallback *action;
    uint8_t selected;
    void *extra;
} MenuConfig;

#define ITEM_HEIGHT 30

void menu_window_init(MenuConfig *config);
void menu_window_deinit();