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

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_log.h"
#include "tcpip_adapter.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "app_settings.h"
#include "app_wifi.h"
#include "app_bt.h"
#include "app_httpd.h"
#ifdef CONFIG_MDNS_ENABLED
#include "mdns.h"
#endif
#ifdef CONFIG_SNTP_ENABLED
//#include "app_sntp.h"
#endif

EventGroupHandle_t event_group;

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
void app_shutdown() {
  app_settings_shutdown();
  #ifdef CONFIG_MDNS_ENABLED
  mdns_free();
  #endif
  app_httpd_shutdown();
  app_wifi_shutdown();
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
void app_main()
{
  EventBits_t uxBits;

  ESP_ERROR_CHECK(esp_event_loop_create_default());   
  event_group = xEventGroupCreate();
  
  app_settings_startup();
  app_wifi_startup();
  app_bt_startup();
  xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);
  
  for (;;) {
	  uxBits = xEventGroupWaitBits(event_group,WIFI_CONNECTED_BIT | WIFI_SOFTAP_BIT,pdFALSE,pdFALSE,500 / portTICK_PERIOD_MS);
	  if (uxBits > 0) {	  
      #ifdef CONFIG_SNTP_ENABLED
      //app_sntp_startup();
      #endif
      #ifdef CONFIG_MDNS_ENABLED
      ESP_ERROR_CHECK(mdns_init());
      ESP_ERROR_CHECK(mdns_hostname_set(settings.hostname));
      ESP_ERROR_CHECK(mdns_instance_name_set(settings.mdns_instance));
      mdns_txt_item_t serviceTxtData[1] = { {"path","/"} };
      ESP_ERROR_CHECK( mdns_service_add(settings.mdns_instance, "_http", "_tcp", 80, serviceTxtData, 1) );
      #endif
      app_httpd_startup();
      return;
    }
  }
  
  esp_register_shutdown_handler(&app_shutdown);
}
