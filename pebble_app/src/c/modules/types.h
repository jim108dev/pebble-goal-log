#pragma once // Prevent errors from being included multiple times

#include <pebble.h> // Pebble SDK symbols

#define MAX_SMALL_TEXT_LEN    20
#define MAX_TEXT_LEN          30
#define MAX_INPUTS             4

typedef struct Record
{
    char id[MAX_SMALL_TEXT_LEN];                          //  20 bytes
    char label[MAX_TEXT_LEN];                             //  30 bytes
    uint8_t max_inputs;                                   //   1 byte
    char labels[MAX_INPUTS][MAX_SMALL_TEXT_LEN];          //  80 bytes
    uint8_t values[MAX_INPUTS];                           //   3 bytes
    char goal[MAX_TEXT_LEN];                              //  30 bytes
    uint8_t left;                                         //   1 byte
    time_t date;                                          //   4 bytes
    bool done;
} Record;                                                 // 164 bytes

typedef void (*DownloadSuccessCallback)(Record records[], uint8_t max_records);
typedef void (*DownloadFailCallback)(char message[MAX_TEXT_LEN]);
