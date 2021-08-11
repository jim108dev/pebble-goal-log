#include "download.h"

static DownloadSuccessCallback s_success;
static DownloadFailCallback s_fail;
static uint8_t s_num_received = 0;
static Record *s_records = NULL;
static EventHandle s_app_message_handle = NULL;
static AppTimer *s_timeout_timer = NULL;

static uint8_t s_max_records = 0;

void close_connection()
{
  if (s_timeout_timer != NULL)
  {
    app_timer_cancel(s_timeout_timer);
    s_timeout_timer = NULL;
  }
  if (s_app_message_handle != NULL)
  {
    events_app_message_unsubscribe(s_app_message_handle);
    s_app_message_handle = NULL;
  }
}

static uint8_t get_max_inputs(Record record)
{
  uint8_t result = MAX_INPUTS;
  for (int i = 0; i < MAX_INPUTS; i++)
  {
    if (strlen(record.labels[i]) == 0){
      result = i;
      break;
    }
  }
  return result;
}

static void parse_data(Record *record, char *data)
{
  DEBUG("Parse %s", data);
  ProcessingState *state = data_processor_create(data, ';');
  dp_fill_small_text(record->id, state);
  dp_fill_small_text(record->label, state);

  for (int i = 0; i < MAX_INPUTS; i++)
  {
    dp_fill_small_text(record->labels[i], state);
    record->values[i] = data_processor_get_int(state);
  }
  dp_fill_small_text(record->goal, state);
  record->left = data_processor_get_int(state);

  data_processor_destroy(state);

  record->date = 0;

  record->max_inputs = get_max_inputs(*record);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context)
{
  DEBUG("Inbox received");

  if (packet_contains_key(iter, DOWNLOAD_KEY_MAX))
  {
    s_max_records = packet_get_uint8(iter, DOWNLOAD_KEY_MAX);
    s_records = malloc(sizeof(Record) * s_max_records);

    DEBUG("Max records (%d) received.", s_max_records);
    return;
  }

  if (packet_contains_key(iter, DOWNLOAD_KEY_DATA))
  {
    char *data = packet_get_string(iter, DOWNLOAD_KEY_DATA);

    if (s_max_records == 0)
    {
      s_fail("Max records is 0");
      return;
    }

    parse_data(&s_records[s_num_received], data);

    DEBUG_RECORD(s_records[s_num_received]);

    s_num_received++;

    if (s_num_received == s_max_records)
    {
      close_connection();
      s_success(s_records, s_num_received);
    }

    return;
  }

  DEBUG("Unknown message received");
}

static void timer_handler(void *context)
{
  s_timeout_timer = NULL;
  close_connection();
  DEBUG("Connection timeout (inbox size = %ld)", INBOX_SIZE);
  s_fail("Connection timeout");
}

void download_init(DownloadSuccessCallback success, DownloadFailCallback fail)
{
  s_success = success;
  s_fail = fail;
  s_app_message_handle = events_app_message_subscribe_handlers((EventAppMessageHandlers){
                                                                   .received = inbox_received_handler},
                                                               NULL);
  events_app_message_request_inbox_size(INBOX_SIZE);
  events_app_message_open();

  s_timeout_timer = app_timer_register(TIMEOUT, timer_handler, NULL);

  DEBUG("Waiting for connection (inbox size = %ld)", INBOX_SIZE);
}

void download_deinit()
{
  close_connection();
  if (s_records != NULL)
  {
    FREE_SAFE(s_records);
  }
}
