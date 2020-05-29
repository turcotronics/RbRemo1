/*
 Modified ESPRESSIF MIT License
 
 Copyright (c) <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
				Bond Keevil (bkeevil), https://github.com/bkeevil/esp32-cam
				Turco Rodolfo, Turcotronics, https://turcotronics.it/ 
 
 Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 it is free of charge, to any person obtaining a copy of this software and associated
 documentation files (the "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 to do so, subject to the following conditions:

 The names of Turcotronics, Robello and TuT may not be used to endorse or promote
 products derived from this software without specific prior written permission.
 
 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#ifdef CONFIG_SNTP_ENABLED
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "app_settings.h"
#include "app_wifi.h"
#include "esp_sntp.h"


static const char *TAG = "sntp";
extern EventGroupHandle_t event_group;

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
void time_sync_ notification_cb(struct timeval *tv) {
    //ESP_LOGI(TAG, "Notification of a time synchronization event");
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
void app_sntp_startup() {
  time_t now = 0
  struct tm timeinfo = { 0 };
  
  ESP_LOGI(TAG, "Initializing SNTP");
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, settings.ntp_server);
  sntp_set_time_sync_notification_cb(time_sync_notification_cb);
  sntp_init();
  // wait for time to be set
  int retry = 0;
  const int retry_count = 3;
  while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
    ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
  char strftime_buf[64];
  setenv("TZ", settings.timezone, 1);
  tzset();
  ESP_LOGI(TAG, "Timezone set to %s",settings.timezone);
  time(&now);
  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
  ESP_LOGI(TAG, "The current date/time is: %s", strftime_buf);
  return;
}

#endif
