#pragma once

#include <pebble.h>
#include "../modules/types.h"
#include "../modules/util.h"

//Private
#define MAX_INPUTS 4
#define PADDING 0
#define HEAD_HEIGHT 20
#define ROW_HEIGHT 24
#define IS_DARK true
#define COUNT_TIMEOUT 100
#define INPUT_WINDOW_MAX_TEXT_LEN 80

static void set_color_theme(TextLayer *layer, bool isInverted);

//Public
typedef void(InputDoneCallback)(uint8_t values[], void *data);

typedef struct InputConfig
{
    uint8_t max_inputs;
    char head_left[MAX_SMALL_TEXT_LEN];
    char head_right[MAX_SMALL_TEXT_LEN];
    char labels[MAX_INPUTS][MAX_SMALL_TEXT_LEN];
    uint8_t values[MAX_INPUTS];
    char main[INPUT_WINDOW_MAX_TEXT_LEN];
    InputDoneCallback *action;
    void *extra;

} InputConfig;

void input_window_init(InputConfig *config);
void input_window_deinit();
static void update_value_layer(uint8_t i);
static void timer_handler(void *context);
static void on_up_start(ClickRecognizerRef recognizer, void *context);
static void on_up_stop(ClickRecognizerRef recognizer, void *context);

