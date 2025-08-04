#include <app_version.h>

#include <zephyr/kernel.h>
#include <zephyr/display/cfb.h>
#include <zephyr/logging/log.h>

#include <lvgl.h>

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

const struct device *display_device;


int main(void) {
	const struct device *display_dev;
	lv_obj_t *label;

  LOG_INF("Display sample apllication");

	/* Init display */
	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));;
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return 0;
	}

	display_blanking_off(display_dev);

	label = lv_label_create(lv_scr_act());
	lv_label_set_text(label, "Hello world!");
	lv_obj_center(label);

	lv_task_handler();

	while (1) {
		k_sleep(K_MSEC(lv_task_handler()));
	}

	return 0;
}
