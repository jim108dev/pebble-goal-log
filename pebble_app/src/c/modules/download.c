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
static void parse_into_max_inputs(Record *record, char *data)
{
  ProcessingState *state = data_processor_create(data, '|');
  uint8_t hmax_inputs = data_processor_count(state);
  record->max_inputs = hmax_inputs < MAX_INPUTS ? hmax_inputs : MAX_INPUTS;
  data_processor_destroy(state);
}

static void parse_into_labels(Record *record, char *data)
{
  DEBUG("Parse %s", data);
  ProcessingState *state = data_processor_create(data, '|');

  for (int i = 0; i < record->max_inputs; i++)
  {
    strcpy(record->labels[i], data_processor_get_string(state));
  }
  data_processor_destroy(state);
}

static void parse_into_values(Record *record, char *data)
{
  DEBUG("Parse %s", data);
  ProcessingState *state = data_processor_create(data, '|');
  for (int i = 0; i < record->max_inputs; i++)
  {
    record->values[i] = data_processor_get_int(state);
  }
  data_processor_destroy(state);
}

static void parse_into_record(Record *record, char *data)
{
  DEBUG("Parse %s", data);
  ProcessingState *state = data_processor_create(data, ';');
  small_textcpy(record->id, data_processor_get_string(state));
  textcpy(record->label, data_processor_get_string(state));

  char labels_data[MAX_TEXT_LEN];
  textcpy(labels_data, data_processor_get_string(state));
  parse_into_max_inputs(record, labels_data);
  parse_into_labels(record, labels_data);

  char values_data[MAX_TEXT_LEN];
  textcpy(values_data, data_processor_get_string(state));
  parse_into_values(record, values_data);

  textcpy(record->goal, data_processor_get_string(state));
  record->left = data_processor_get_int(state);

  record->date = 0;

  data_processor_destroy(state);
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

    parse_into_record(&s_records[s_num_received], data);

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

static void timeout_timer_handler(void *context)
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

  s_timeout_timer = app_timer_register(TIMEOUT, timeout_timer_handler, NULL);

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
