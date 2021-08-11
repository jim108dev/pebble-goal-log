#pragma once // Prevent errors from being included multiple times

#include <pebble.h>
#include "types.h"
#include "util.h"

//private
#define LOG_TAG 1
typedef struct DLogRecord
{
    char id[MAX_SMALL_TEXT_LEN];      //  20 bytes
    uint8_t values[MAX_INPUTS];       //   4 byte
    time_t date;                      //   4 bytes
} DLogRecord;                         //  28 bytes


//public
void dlog_init();

void dlog_log(Record record);

void dlog_deinit();


