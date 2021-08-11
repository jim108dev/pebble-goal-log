#pragma once // Prevent errors from being included multiple times

#include <pebble.h>
#include "time.h"
#include <@smallstoneapps/linked-list/linked-list.h>
#include "modules/types.h"
#include "modules/util.h"
#include "modules/download.h"
#include "modules/dlog.h"
#include "modules/pers.h"
#include "windows/info_window.h"
#include "windows/input_window.h"
#include "windows/menu_window.h"

typedef struct MenuItem
{
    uint8_t ref;
    char label[MAX_SMALL_TEXT_LEN];

} MenuItem;   

static void on_finish_record(uint8_t values[], void *data);
static void show_first_window();
static void show_last_window();
static void cache_menu();
static void on_show_goals(void *data);
static void on_show_goal_detail(uint8_t select_num, void *data);
static void download_success(Record *records, uint8_t max_records);
static void download_fail(char *msg);
static void init();
static void deinit();