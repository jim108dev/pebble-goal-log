#pragma once

#include <pebble.h>
#include "../modules/types.h"
#include "../modules/util.h"

//Private
#define PADDING 0
#define HEAD_HEIGHT 20
#define DARK_BACKGROUND true

//Public
typedef void(InfoDoneCallback)(void *data);

typedef struct InfoConfig
{
    char head_left[MAX_TEXT_LEN];
    char head_right[MAX_TEXT_LEN];
    char main[80];
    InfoDoneCallback *action;
    void *extra;

} InfoConfig;

void info_window_init(InfoConfig *config);
