#pragma once // Prevent errors from being included multiple times

#include <pebble.h> // Pebble SDK symbols

#define MAX_SMALL_TEXT_LEN    20
#define MAX_TEXT_LEN          30
#define MAX_INPUTS             4

//id;text;count_names;count_values;goal;left
typedef struct Record
{
    char id[MAX_SMALL_TEXT_LEN];                          //  20 bytes
    char label[MAX_TEXT_LEN];                             //  30 bytes
    uint8_t max_inputs;
    char labels[MAX_INPUTS][MAX_TEXT_LEN];                //  90 bytes
    uint8_t values[MAX_INPUTS];                           //   3 bytes
    char goal[MAX_TEXT_LEN];                              //  30 bytes
    uint8_t left;                                         //   1 byte
    time_t stop;                                          //   4 bytes
} Record;                                                 // 174 bytes

typedef void (*DownloadSuccessCallback)(Record records[], uint8_t max_records);
typedef void (*DownloadFailCallback)(char message[MAX_TEXT_LEN]);

typedef struct CurrentRecord
{
    uint8_t num;
    uint8_t max;
    Record *record;

} CurrentRecord;