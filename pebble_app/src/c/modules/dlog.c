#include "dlog.h"
#include "types.h"
#include "util.h"

static DataLoggingSessionRef s_session_ref = NULL;

void dlog_init()
{
  if (s_session_ref == NULL)
  {
    s_session_ref = data_logging_create(LOG_TAG, DATA_LOGGING_BYTE_ARRAY, sizeof(DLogRecord), true);
  }
}

void dlog_log(Record record)
{
  DLogRecord o;
  strcpy(o.id, record.id);
  for (int i = 0; i < MAX_INPUTS; i++)
  {
    o.values[i] = i < record.max_inputs ? record.values[i] : 0;
  }
  o.date = record.date;

  DataLoggingResult result = data_logging_log(s_session_ref, &o, 1);
  DEBUG("Send: %d bytes", sizeof(DLogRecord));

  // Was the value successfully stored? If it failed, print the reason
  if (result != DATA_LOGGING_SUCCESS)
  {
    DEBUG("Error logging data: %d", (int)result);
  }
}

void dlog_deinit()
{
  if (s_session_ref != NULL)
  {
    data_logging_finish(s_session_ref);
    s_session_ref = NULL;
    DEBUG("Data logging completed");
  }
}
