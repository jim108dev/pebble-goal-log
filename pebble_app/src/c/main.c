#include "main.h"

static LinkedRoot *s_record_list = NULL;
static uint8_t s_current_num = 0;

static void on_finish_record(uint8_t values[], void *data)
{
  DEBUG("Current num = %d", s_current_num);

  Record *record = linked_list_get(s_record_list, s_current_num);

  record->date = time(NULL);
  record->done = true;

  for (int i = 0; i < record->max_inputs; i++)
  {
    record->values[i] = values[i];
  }
  pers_write_record(record, s_current_num);
  dlog_log(*record);
  FREE_SAFE(record);
  linked_list_remove(s_record_list, s_current_num);
  uint8_t max_records = linked_list_count(s_record_list);

  window_stack_pop_all(true);
  if (max_records > 0)
    on_show_goals(NULL);
  else
    show_last_window();
}

static void load_records()
{
  uint8_t max_records = pers_read_max_records();
  if (s_record_list != NULL)
  {
    linked_list_foreach(s_record_list, ll_free_callback, NULL);
    linked_list_clear(s_record_list);
  }
  s_record_list = linked_list_create_root();

  for (int i = 0; i < max_records; i++)
  {
    Record *record = malloc(sizeof(Record));
    pers_read_record(record, i);
    if (!record->done)
      linked_list_append(s_record_list, record);
  }
}

static void on_show_goals(void *data)
{
  download_deinit();

  uint8_t max_records = linked_list_count(s_record_list);

  MenuConfig *c = malloc(sizeof(MenuConfig));
  c->action = on_show_goal_detail;

  for (int i = 0; i < max_records; i++)
  {
    Record *record = (Record *)linked_list_get(s_record_list, i);
    DEBUG("Plug in %s", record->label);
    strcpy(c->labels[i], record->label);
  }
  c->max_items = max_records;
  c->extra = NULL;
  c->selected = 0;

  DEBUG("Show menu");
  menu_window_init(c);
}

static void on_show_goal_detail(uint8_t select_num, void *data)
{
  uint8_t max_records = linked_list_count(s_record_list);
  Record *record = (Record *)linked_list_get(s_record_list, select_num);

  InputConfig *c = malloc(sizeof(InputConfig));
  c->action = on_finish_record;
  s_current_num = select_num;
  strcpy(c->head_left, record->id);
  sprint_progress(c->head_right, select_num + 1, max_records);
  for (int i = 0; i < record->max_inputs; i++)
  {
    strcpy(c->labels[i], record->labels[i]);
    c->values[i] = record->values[i];
  }
  snprintf(c->main, MAX_TEXT_LEN, "%.14s: %.14s", record->label, record->goal);
  c->max_inputs = record->max_inputs;

  input_window_init(c);
}

static void show_first_window()
{
  InfoConfig *c = malloc(sizeof(InfoConfig));

  uint8_t max_records = linked_list_count(s_record_list);

  if (max_records > 0)
  {
    dlog_init();
    load_records();
    strcpy(c->main, "Goals ready. Press 'select' to start.");
    c->action = on_show_goals;
  }
  else
  {
    strcpy(c->main, "All done for today. Please upload new goals with 'pebble_upload.py'.");
    c->action = NULL;
  }

  strcpy(c->head_left, "");
  strcpy(c->head_right, "");

  info_window_init(c);
}

static void show_last_window()
{
  dlog_deinit();

  uint8_t max_records = pers_read_max_records();
  DEBUG("Read max_records %d", max_records);

  InfoConfig *c = malloc(sizeof(InfoConfig));
  c->action=NULL;
  strcpy(c->main, "All goals done for today. Please run 'pebble_download.py'.");
  strcpy(c->head_left, "");
  strcpy(c->head_right, "");

  info_window_init(c);
}

static void download_success(Record *records, uint8_t max_records)
{
  DEBUG("Downloaded %d records", max_records);
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

  load_records();

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

  load_records();
  show_first_window();
  if (appLaunchReason == APP_LAUNCH_PHONE)
  {
    pers_sweep();
    download_init(download_success, download_fail);
    // If app was launched by phone and close to last app is disabled, always exit to the watchface instead of to the menu
    exit_reason_set(APP_EXIT_ACTION_PERFORMED_SUCCESSFULLY);
    return;
  }
}

static void deinit()
{

  if (s_record_list != NULL)
  {
    linked_list_foreach(s_record_list, ll_free_callback, NULL);
    linked_list_clear(s_record_list);
  }
  menu_window_deinit();
  info_window_deinit();
  input_window_deinit();
  dlog_deinit();
  download_deinit();
}

int main()
{
  init();
  app_event_loop();
  deinit();
}