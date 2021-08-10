#include "input_window.h"

static Window *s_window = NULL;
static TextLayer *s_main_layer;
static TextLayer *s_head_left_layer;
static TextLayer *s_head_right_layer;
static TextLayer *s_label_layers[MAX_INPUTS];
static TextLayer *s_value_layers[MAX_INPUTS];
static InputConfig *s_config;
static char s_value_text[MAX_INPUTS][MAX_SMALL_TEXT_LEN];
static uint8_t s_current_num;

static void on_up(ClickRecognizerRef recognizer, void *context)
{
  s_config->values[s_current_num]++;
}
static void on_select(ClickRecognizerRef recognizer, void *context)
{
  if (s_current_num == s_config->max_inputs)
  {
    s_config->action(s_config->values, s_config->extra);
    return;
  }
  set_color_theme(s_value_layers[s_current_num], false);
  s_current_num++;
  set_color_theme(s_value_layers[s_current_num], true);
}
static void on_down(ClickRecognizerRef recognizer, void *context)
{
  s_config->values[s_current_num]--;
}

static void click_provider(void *context)
{
  window_single_click_subscribe(BUTTON_ID_UP, on_up);
  window_single_click_subscribe(BUTTON_ID_SELECT, on_select);
  window_single_click_subscribe(BUTTON_ID_DOWN, on_down);
}

static void set_color_theme(TextLayer *layer, bool isInverted)
{
  if (isInverted)
  {
    text_layer_set_background_color(layer, GColorBlack);
    text_layer_set_text_color(layer, GColorWhite);
  }
  else
  {
    text_layer_set_background_color(layer, GColorWhite);
    text_layer_set_text_color(layer, GColorBlack);
  }
}

static void window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_head_left_layer = text_layer_create(GRect(PADDING, 0, bounds.size.w / 2, ROW_HEIGHT));
  text_layer_set_text(s_head_left_layer, s_config->head_left);
  text_layer_set_text_alignment(s_head_left_layer, GTextAlignmentLeft);
  set_color_theme(s_head_left_layer, false);
  layer_add_child(window_layer, text_layer_get_layer(s_head_left_layer));

  s_head_right_layer = text_layer_create(GRect(bounds.size.w / 2, 0, bounds.size.w / 2, ROW_HEIGHT));
  text_layer_set_text(s_head_right_layer, s_config->head_right);
  text_layer_set_text_alignment(s_head_right_layer, GTextAlignmentRight);
  set_color_theme(s_head_right_layer, false);
  layer_add_child(window_layer, text_layer_get_layer(s_head_right_layer));

  for (int i = 0; i < s_config->max_inputs; i++)
  {
    s_label_layers[i] = text_layer_create(GRect(PADDING, ROW_HEIGHT * (i + 1), bounds.size.w / 2, ROW_HEIGHT));
    text_layer_set_text(s_label_layers[i], s_config->labels[i]);
    text_layer_set_text_alignment(s_label_layers[i], GTextAlignmentLeft);
    set_color_theme(s_label_layers[i], false);
    layer_add_child(window_layer, text_layer_get_layer(s_label_layers[i]));

    s_value_layers[i] = text_layer_create(GRect(bounds.size.w / 2, ROW_HEIGHT * (i + 1), bounds.size.w / 2, ROW_HEIGHT));
    update_value_layer(i);
    text_layer_set_text_alignment(s_value_layers[i], GTextAlignmentRight);
    set_color_theme(s_value_layers[i], i==s_current_num);
    layer_add_child(window_layer, text_layer_get_layer(s_value_layers[i]));
  }

  GRect main_bounds = GRect(PADDING, ROW_HEIGHT, bounds.size.w - PADDING, bounds.size.h);
  s_main_layer = text_layer_create(main_bounds);
  text_layer_set_font(s_main_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text(s_main_layer, s_config->main);
  set_color_theme(s_main_layer, false);
  text_layer_set_text_alignment(s_main_layer, GTextAlignmentLeft);

  GSize max_size = GSize(main_bounds.size.w - main_bounds.origin.x, main_bounds.size.h - main_bounds.origin.y);
  text_layer_set_size(s_main_layer, max_size);
  layer_add_child(window_layer, text_layer_get_layer(s_main_layer));
}

static void update_value_layer(uint8_t i)
{
  snprintf(s_value_text[i], MAX_SMALL_TEXT_LEN, "%d", s_config->values[i]);
  text_layer_set_text(s_value_layers[i], s_value_text[i]);
}

static void window_unload(Window *window)
{
  text_layer_destroy(s_head_left_layer);
  text_layer_destroy(s_head_right_layer);
  for (int i = 0; i < s_config->max_inputs; i++)
  {
    text_layer_destroy(s_label_layers[i]);
    text_layer_destroy(s_value_layers[i]);
  }
  text_layer_destroy(s_main_layer);
}

static void info_window_deinit()
{
  window_destroy(s_window);
  s_window = NULL;
  FREE_SAFE(s_config);
}

void input_window_init(InputConfig *config)
{
  if (s_window != NULL)
  {
    window_stack_pop_all(true);
    info_window_deinit();
  }

  s_window = window_create();
  s_config = config;
  s_current_num = 0;

  window_set_click_config_provider(s_window, click_provider);
  window_set_window_handlers(s_window, (WindowHandlers){
                                           .load = window_load,
                                           .unload = window_unload,
                                       });
  window_stack_push(s_window, true);
}