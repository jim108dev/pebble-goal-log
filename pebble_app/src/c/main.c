#include "main.h"

static Record *s_records = NULL;
static uint8_t s_max_records = 0;

static void on_finish_record(uint8_t values[], void *data)
{
  uint8_t current_num = *(uint8_t *)data;

  Record *record = &s_records[current_num];
  DEBUG_RECORD(*record);

  record->stop = time(NULL);

  for (int i = 0; i < record->max_inputs; i++)
  {
    record->values[i] = values[i];
  }
  pers_write_record(record, current_num);
  //dlog_log(*record);

  on_show_goals(NULL);
}

static void load_records()
{
  uint8_t s_max_records = pers_read_max_records();

  FREE_SAFE(s_records);

  s_records = malloc(sizeof(Record) * s_max_records);
  for (int i = 0; i < s_max_records; i++)
  {
    pers_read_record(&s_records[i], i);
  }
}

static void on_show_goals(void *data)
{
  MenuConfig *c = malloc(sizeof(MenuConfig));
  c->action = on_show_goal_detail;

  for (int i = 0; i < s_max_records; i++)
  {
    strcpy(c->labels[i], s_records[i].label);
  }

  c->extra = NULL;
  c->selected = 0;

  menu_window_init(c);
}

static void on_show_goal_detail(uint8_t select_num, void *data)
{
  Record *record = &s_records[select_num];

  InputConfig *c = malloc(sizeof(InputConfig));
  c->action = on_finish_record;
  strcpy(c->head_left, record->id);
  sprint_progress(c->head_right, select_num, s_max_records);
  for (int i = 0; i < record->max_inputs; i++)
  {
    strcpy(c->labels[i], record->labels[i]);
    c->values[i] = record->values[i];
  }
  strcpy(c->main, record->goal);

  input_window_init(c);
}

static void show_first_window()
{
  InfoConfig *c = malloc(sizeof(InfoConfig));

  uint8_t max_records = 0; //pers_read_max_records();

  if (max_records > 0)
  {
    load_records();
    strcpy(c->main, "Questions are ready. Press 'select' to start.");
    c->action = on_show_goals;
  }
  else
  {
    strcpy(c->main, "No questions found. Please run 'pebble_upload.py'.");
    c->action = NULL;
  }

  strcpy(c->head_left, "");
  strcpy(c->head_right, "");

  info_window_init(c);
}

static void download_success(Record *records, uint8_t max_records)
{
  for (int i = 0; i < max_records; i++)
  {
    int status_code = pers_write_record(&records[i], i);
    if (status_code <= 0)
    {
      DEBUG("Persistend write error. Could only store %d items", i);
      max_records = i;
      break;
    }
  }
  download_deinit();

  pers_write_max_records(max_records);

  show_first_window();
}

static void download_fail(char msg[MAX_TEXT_LEN])
{
  DEBUG("Download failed. Please try 'pebble_upload.py' again");

  download_deinit();
}

static void init()
{
  AppLaunchReason appLaunchReason = launch_reason();
  pers_sweep();
  
  if (appLaunchReason == APP_LAUNCH_PHONE)
  {
    pers_sweep();
    show_first_window();
    download_init(download_success, download_fail);
    // If app was launched by phone and close to last app is disabled, always exit to the watchface instead of to the menu
    exit_reason_set(APP_EXIT_ACTION_PERFORMED_SUCCESSFULLY);
    return;
  }
  show_first_window();
}

static void deinit()
{
  FREE_SAFE(s_records);
  //dlog_deinit();
}

int main()
{
  init();
  app_event_loop();
  deinit();
}