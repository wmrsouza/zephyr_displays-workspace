#include "esp_attr.h"
#include <app_version.h>

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);


int main(void) {
  LOG_INF("Touch sample apllication");

	return 0;
}
