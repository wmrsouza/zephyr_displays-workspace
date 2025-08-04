/* #include "core/lv_obj_style.h" */
/* #include "font/lv_font.h" */
/* #include "misc/lv_style.h" */
#include "esp_attr.h"
#include <app_version.h>

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/display/cfb.h>
#include <zephyr/logging/log.h>

#include <lvgl.h>
/* #include <core/lv_obj_style.h> */
/* #include_next <font/lv_font.h> */
/* #include <widgets/lv_label.h> */

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

/* const struct device *display_device; */

#ifdef CONFIG_LV_Z_POINTER_INPUT

IRAM_ATTR static void button_event_handler(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		LOG_DBG("Clicked");
	}
}

#endif

int main(void) {
	const struct device *display_dev;
	lv_obj_t *screen;
	lv_obj_t *button;
	lv_obj_t *label;
	/* static lv_style_t label_style; */

  LOG_INF("Display sample apllication");

	/* Init display */
	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));;
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return 0;
	}

	/* lv_style_init(&label_style); */
	/* lv_style_set_prop(&label_style, LV_STYLE_TEXT_FONT, &lv_font_montserrat_14); */

	screen = lv_scr_act();
	button = lv_btn_create(screen);
	lv_obj_center(button);
	label = lv_label_create(button);
	/* label = lv_label_create(screen); */
#ifdef CONFIG_LV_Z_POINTER_INPUT
	lv_obj_add_event_cb(button, button_event_handler, LV_EVENT_ALL, NULL);
	/* lv_obj_add_event_cb(button, button_event_handler, LV_EVENT_CLICKED, NULL); */
#endif
	/* lv_obj_add_style(label, &label_style, 0); */
	lv_obj_center(label);
	lv_label_set_text(label, "<<<<<Hello world!>>>>>");

	lv_task_handler();
	display_blanking_off(display_dev);

	while (1) {
		static uint32_t counter = 0;
		char str[4];

		counter++;
		snprintf(str, 4, "%03"PRIu32, counter % 1000);
		lv_label_set_text(label, str);
		k_sleep(K_MSEC(lv_task_handler()));
	}

	return 0;
}
