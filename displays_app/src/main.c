#include <app_version.h>
#include <cfb_mono_04B.h>
#include <cfb_mono_COMICBD.h>

#include <zephyr/kernel.h>
#include <zephyr/display/cfb.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

const struct device *display_device;

uint32_t counter, counter_copy, alarm_time;

int main(void) {
  uint16_t x_res, y_res;
  uint16_t rows, cols;
  uint8_t width, height;
  uint8_t ppt;
  uint32_t fps_copy;

  LOG_INF("Display sample apllication");

  /* Init display */
  display_device = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
  if (!device_is_ready(display_device)) {
    LOG_ERR("Display %s not found. Aborting...", display_device->name);
  } else {
    LOG_INF("Display %s is ready", display_device->name);
  }

  /* Init routing for display */
  if (display_set_pixel_format(display_device, PIXEL_FORMAT_MONO10) != 0) {
    if (display_set_pixel_format(display_device, PIXEL_FORMAT_MONO01) != 0) {
      LOG_ERR("Failed to set required pixel format");
    }
  }

  if (cfb_framebuffer_init(display_device)) {
    LOG_ERR("Framebuffer initialization failed!\n");
  }

  cfb_framebuffer_clear(display_device, true);
  display_blanking_off(display_device);

  x_res = cfb_get_display_parameter(display_device, CFB_DISPLAY_WIDTH);
  y_res = cfb_get_display_parameter(display_device, CFB_DISPLAY_HEIGHT);
  rows = cfb_get_display_parameter(display_device, CFB_DISPLAY_ROWS);
  cols = cfb_get_display_parameter(display_device, CFB_DISPLAY_COLS);
  ppt = cfb_get_display_parameter(display_device, CFB_DISPLAY_PPT);

  // log display parameters
  LOG_INF("Display parameters: x_res %d, y_res %d, ppt %d, rows %d, cols %d",
          x_res, y_res, ppt, rows, cols);

  // log supported display fonts
  LOG_INF("number of fonts: %d, indexes and sizes:",
          cfb_get_numof_fonts(display_device));
  for (int idx = 0; idx < cfb_get_numof_fonts(display_device); idx++) {
    cfb_get_font_size(display_device, idx, &width, &height);
    LOG_INF("font %d -> width %d, height %d", idx, width, height);
  }

  // get display width and height of font with index 0
  cfb_get_font_size(display_device, 0, &width, &height);

  /* cfb_framebuffer_invert(display_device); */

  // Set font kerning (spacing between individual letters).
  cfb_set_kerning(display_device, 0);

  // Display welcome message
  cfb_framebuffer_clear(display_device, false);
  /*k_sleep(K_MSEC(100));*/

  cfb_framebuffer_set_font(display_device, 2);
  cfb_print(display_device, "WonderBox", 0, 0);
  cfb_print(display_device, "Timegrapher!", 0, height);
  cfb_print(display_device, "By", 0, 2 * height);
  cfb_print(display_device, "Team", (x_res - 4 * width), 3 * height);
  cfb_framebuffer_finalize(display_device);

  counter = 0;
  counter_copy = counter;
  fps_copy = 0;
  alarm_time = k_uptime_get_32() + 1000;
  while (1) {
    char str[7];
    uint32_t up_time = k_uptime_get_32();

    if (up_time > alarm_time) {
      uint32_t fps = counter - counter_copy;

      if (counter < counter_copy) {
        fps += UINT32_MAX;
      }

      if (fps != fps_copy) {
        LOG_DBG("counter_copy:%"PRIu32, counter_copy);
        LOG_DBG("counter:%"PRIu32, counter);
        LOG_DBG("fps:%"PRIu32, fps);
        fps_copy = fps;
      }

      snprintf(str, sizeof(str), "%02" PRIu32 " fps", fps);
      cfb_print(display_device, str, x_res - 6 * width, 2 * height);

      counter_copy = counter;
      alarm_time = up_time + 1000;
    }
    snprintf(str, sizeof(str), "[%04" PRIu32 "]", (counter % 10000));
    cfb_print(display_device, str, 0 * width, 3 * height);
    /* cfb_invert_area(display_device, 0 * width, 3 * height, 5 * width, 3
     * height); */
    cfb_framebuffer_finalize(display_device);
    counter++;

    /* k_sleep(K_MSEC(10)); */
  }

  return 0;
}
