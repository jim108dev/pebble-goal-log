#pragma once

#include <pebble.h>
#include "../modules/types.h"
#include "../modules/util.h"

//Private
#define PADDING 0
#define HEAD_HEIGHT 20
#define DARK_BACKGROUND true
#define INFO_WINDOW_MAX_TEXT_LEN 80

//Public

typedef void(InfoDoneCallback)(void *data);

typedef struct InfoConfig
{
    char head_left[MAX_SMALL_TEXT_LEN];
    char head_right[MAX_SMALL_TEXT_LEN];
    char main[INFO_WINDOW_MAX_TEXT_LEN];
    InfoDoneCallback *action;
    void *extra;

} InfoConfig;

void info_window_init(InfoConfig *config);
void info_window_deinit();
