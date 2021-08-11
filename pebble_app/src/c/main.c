#include "main.h"

static LinkedRoot *s_menu_cache = NULL;

static void on_finish_record(uint8_t values[], void *data)
{
  MenuItem *menu_item = (MenuItem*) data;
  DEBUG("Finish %s", menu_item->label);

  Record record;
  pers_read_record(&record, menu_item->ref);

  record.date = time(NULL);
  record.done = true;

  uint8_t num =  linked_list_find(s_menu_cache, menu_item);
  linked_list_remove(s_menu_cache, num);

  for (int i = 0; i < record.max_inputs; i++)
  {
    record.values[i] = values[i];
  }
  pers_write_record(&record, menu_item->ref);
  dlog_log(record);
  uint8_t max_menu_items = linked_list_count(s_menu_cache);

  window_stack_pop_all(true);
  if (max_menu_items > 0)
    on_show_goals(NULL);
  else
    show_last_window();
}

static void cache_menu()
{
  uint8_t max_records = pers_read_max_records();
  DEBUG("MAX records = %d", max_records);

  if (s_menu_cache != NULL)
  {
    linked_list_foreach(s_menu_cache, ll_free_callback, NULL);
    linked_list_clear(s_menu_cache);
  }

  for (int i = 0; i < max_records; i++)
  {
    Record record;
    pers_read_record(&record, i);
    if (record.done)
      continue;

    MenuItem *menu_item = malloc(sizeof(MenuItem));
    menu_item->ref = i;
    small_textcpy(menu_item->label, record.label);
    linked_list_append(s_menu_cache, menu_item);
  }
}

static void on_show_goals(void *data)
{
  download_deinit();

  uint8_t max_menu_items = linked_list_count(s_menu_cache);

  MenuConfig *c = malloc(sizeof(MenuConfig));
  c->action = on_show_goal_detail;

  for (int i = 0; i < max_menu_items; i++)
  {
    MenuItem *item = linked_list_get(s_menu_cache, i);
    Record record;
    pers_read_record(&record, item->ref);
    DEBUG("Plug in %s", record.label);
    strcpy(c->labels[i], record.label);
  }
  c->max_items = max_menu_items;
  c->extra = NULL;
  c->selected = 0;

  DEBUG("Show menu");
  menu_window_init(c);
}

static void on_show_goal_detail(uint8_t select_num, void *data)
{
  uint8_t max_menu_items = linked_list_count(s_menu_cache);
  MenuItem *menu_item = linked_list_get(s_menu_cache, select_num);
  Record record;
  pers_read_record(&record, menu_item->ref);

  InputConfig *c = malloc(sizeof(InputConfig));
  c->action = on_finish_record;
  c->extra = menu_item;

  strcpy(c->head_left, record.id);
  sprint_progress(c->head_right, select_num + 1, max_menu_items);
  for (int i = 0; i < record.max_inputs; i++)
  {
    strcpy(c->labels[i], record.labels[i]);
    c->values[i] = record.values[i];
  }
  snprintf(c->main, MAX_TEXT_LEN, "%.14s:\n%.14s", record.label, record.goal);
  c->max_inputs = record.max_inputs;

  input_window_init(c);
}

static void show_first_window()
{
  InfoConfig *c = malloc(sizeof(InfoConfig));

  cache_menu();
  uint8_t max_menu_items = linked_list_count(s_menu_cache);

  if (max_menu_items > 0)
  {
    dlog_init();
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
  c->action = NULL;
  strcpy(c->main, "All goals done for today. Please run 'pebble_download.py'.");
  strcpy(c->head_left, "");
  strcpy(c->head_right, "");

  info_window_init(c);
}

static void download_success(Record *records, uint8_t max_records)
{
  DEBUG("Downloaded %d records", max_records);
  pers_sweep();
  
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
  s_menu_cache = linked_list_create_root();

  AppLaunchReason appLaunchReason = launch_reason();

  show_first_window();
  if (appLaunchReason == APP_LAUNCH_PHONE)
  {
    download_init(download_success, download_fail);
    // If app was launched by phone and close to last app is disabled, always exit to the watchface instead of to the menu
    exit_reason_set(APP_EXIT_ACTION_PERFORMED_SUCCESSFULLY);
    return;
  }
}

static void deinit()
{

  if (s_menu_cache != NULL)
  {
    linked_list_foreach(s_menu_cache, ll_free_callback, NULL);
    linked_list_clear(s_menu_cache);
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