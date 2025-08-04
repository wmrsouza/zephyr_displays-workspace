#include "misc/lv_area.h"
#include "widgets/dropdown/lv_dropdown.h"
#include "widgets/spinbox/lv_spinbox.h"
#include <app_version.h>

/* #include <inttypes.h> */
#include <zephyr/kernel.h>
#include <zephyr/random/random.h>
#include <zephyr/display/cfb.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#include <lvgl.h>

lv_obj_t *screen = NULL;
lv_obj_t *button = NULL;
lv_obj_t *label = NULL;
lv_obj_t *dropdown = NULL;
lv_obj_t *hor_box = NULL;
lv_obj_t *spinbox = NULL;
lv_obj_t *button_inc = NULL;
lv_obj_t *button_dec = NULL;

#define NORMAL_DELTA_TIME  500
#define EXTRA_DELTA_TIME  2000

uint32_t click_counter = 0;
bool add_extra_time = false;

static void button_place_randon(lv_obj_t *button)
{
		uint32_t x, y, max_x, max_y;

		max_x =lv_obj_get_width(screen) - lv_obj_get_width(button);
		max_y =lv_obj_get_height(screen) - lv_obj_get_height(button);

		x = sys_rand8_get();
		y = sys_rand8_get();

		if (x > max_x) {
			x = max_x;
		}

		if (y > max_y) {
			y = max_y;
		}

		lv_obj_set_pos(button, x, y);

		/* LOG_DBG("(%"PRIi32", %"PRIi32") -> (%"PRIu32", %"PRIu32")", lv_obj_get_x(button), lv_obj_get_y(button), x, y); */
}

#ifdef CONFIG_LV_Z_POINTER_INPUT

static void screen_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		LOG_DBG("Screen clicked");

		add_extra_time = true;

		/* LOG_DBG("(%"PRIi32", %"PRIi32") -> (%"PRIu32", %"PRIu32")", lv_obj_get_x(button), lv_obj_get_y(button), 0, 0); */
	}
}

static void button_event_handler(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		LOG_DBG("Button clicked");

		lv_obj_set_pos(button, 0, 0);
		add_extra_time = true;
	}
}

static void dropdown_event_handler(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target_obj(e);

	if(code == LV_EVENT_VALUE_CHANGED) {
		char buf[32];
		uint32_t count = lv_dropdown_get_option_count(dropdown);
		uint32_t selected = lv_dropdown_get_selected(dropdown);

		lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
		LOG_DBG("[%"PRIu32"/%"PRIu32"] Opcao: %s", selected, count, buf);
	}
}

static void button_inc_event_handler(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		lv_spinbox_increment(spinbox);

		LOG_DBG("Button inc clicked");
	}
}

static void button_dec_event_handler(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		lv_spinbox_decrement(spinbox);

		LOG_DBG("Button dec clicked");
	}
}

#endif /* CONFIG_LV_Z_POINTER_INPUT */

int main(void) {
	LOG_INF("Display sample apllication");

	const struct device *display_dev;

	/* Init display */
	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));;
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return 0;
	}

	screen = lv_scr_act();
#ifdef CONFIG_LV_Z_POINTER_INPUT
	lv_obj_add_event_cb(screen, screen_event_handler, LV_EVENT_ALL, NULL);
#endif

	button = lv_btn_create(screen);
	lv_obj_set_size(button, 150, 50);
	lv_obj_set_pos(button, 0, 0);
#ifdef CONFIG_LV_Z_POINTER_INPUT
	lv_obj_add_event_cb(button, button_event_handler, LV_EVENT_ALL, NULL);
#endif

	dropdown = lv_dropdown_create(screen);
	/* lv_obj_set_pos(dropdown, 100, 0); */
	/* lv_obj_set_size(dropdown, 150, 50); */
	lv_obj_align(dropdown, LV_ALIGN_TOP_LEFT, 0, 0);
	lv_dropdown_set_text(dropdown, "Opcoes");
	lv_dropdown_set_options(dropdown,
												  "1a opcao\n"
												  "2a opcao\n"
												  "3a opcao\n"
												  "4a opcao\n"
												  "5a opcao");
	lv_dropdown_set_selected_highlight(dropdown, true);
  lv_dropdown_set_dir(dropdown, LV_DIR_BOTTOM);
  lv_dropdown_set_symbol(dropdown, LV_SYMBOL_HOME);
  /* lv_dropdown_open(dropdown); */
#ifdef CONFIG_LV_Z_POINTER_INPUT
	lv_obj_add_event_cb(dropdown, dropdown_event_handler, LV_EVENT_ALL, NULL);
#endif

	hor_box = lv_obj_create(screen);
	lv_obj_align(hor_box, LV_ALIGN_BOTTOM_MID, 0, 5);
	lv_obj_set_size(hor_box, 220, 80);
	lv_obj_set_flex_flow(hor_box, LV_FLEX_FLOW_ROW);

	button_dec = lv_button_create(hor_box);
	lv_obj_set_size(button_dec, LV_PCT(25), LV_PCT(100));
	lv_obj_t *label_dec = lv_label_create(button_dec);
	lv_obj_center(label_dec);
	lv_label_set_text(label_dec, "-");
#ifdef CONFIG_LV_Z_POINTER_INPUT
	lv_obj_add_event_cb(button_dec, button_dec_event_handler, LV_EVENT_ALL, NULL);
#endif

	spinbox = lv_spinbox_create(hor_box);
	lv_obj_set_size(spinbox, LV_PCT(50), LV_PCT(100));
	lv_spinbox_set_rollover(spinbox, true);
	lv_spinbox_set_digit_format(spinbox, 4, 2);
	lv_spinbox_set_range(spinbox, 0, 9999);
	lv_spinbox_set_value(spinbox, 5000);

	button_inc = lv_button_create(hor_box);
	lv_obj_set_size(button_inc, LV_PCT(25), LV_PCT(100));
	lv_obj_t *label_inc = lv_label_create(button_inc);
	lv_obj_center(label_inc);
	lv_label_set_text(label_inc, "+");
#ifdef CONFIG_LV_Z_POINTER_INPUT
	lv_obj_add_event_cb(button_inc, button_inc_event_handler, LV_EVENT_ALL, NULL);
#endif

	label = lv_label_create(button);
	lv_obj_center(label);
	lv_label_set_text(label, "Hello world!");

	lv_task_handler();
	display_blanking_off(display_dev);

	button_place_randon(button);

	uint32_t next_time = k_uptime_get_32() + NORMAL_DELTA_TIME;
	while (true) {
		k_sleep(K_MSEC(lv_task_handler()));

		uint32_t curr_time = k_uptime_get_32();
		if (curr_time > next_time) {
			next_time = curr_time;

			if (add_extra_time) {
				lv_label_set_text(label, "Hello world!");

				next_time += EXTRA_DELTA_TIME;
				add_extra_time = false;
			} else {
				char str[4];

				snprintf(str, 4, "%03"PRIu32, ++click_counter % 1000);
				lv_label_set_text(label, str);
				button_place_randon(button);

				next_time += NORMAL_DELTA_TIME;
			}
		}
	}
/*
	while (true) {
		k_sleep(K_MSEC(1000));
		LOG_DBG("Testando...");
	}
*/
	return 0;
}
