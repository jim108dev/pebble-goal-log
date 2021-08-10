#pragma once

#include <pebble.h>
#include "../modules/types.h"
#include "../modules/util.h"

//Private
#define MAX_INPUTS 4
#define PADDING 0
#define ROW_HEIGHT 20
#define IS_DARK true

static void set_color_theme(TextLayer *layer, bool isInverted);

//Public
typedef void(InputDoneCallback)(uint8_t values[], void *data);

typedef struct InputConfig
{
    uint8_t max_inputs;
    char head_left[MAX_TEXT_LEN];
    char head_right[MAX_TEXT_LEN];
    char labels[MAX_INPUTS][MAX_TEXT_LEN];
    uint8_t values[MAX_INPUTS];
    char main[MAX_TEXT_LEN];
    InputDoneCallback *action;
    void *extra;

} InputConfig;

void input_window_init(InputConfig *config);
void input_window_deinit();
static void update_value_layer(uint8_t i);
