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

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "sdkconfig.h"
#include "lwip/ip4_addr.h"
#include "app_settings.h"

static const char* TAG = "settings";
static const char* NVS_KEY = "settings";

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
static void log_settings() {
  ESP_LOGI(TAG," hostname=%s",settings.hostname);
  ESP_LOGI(TAG," wifi_ssid=%s",settings.wifi_ssid);
  ESP_LOGI(TAG," wifi_password=%s",settings.wifi_password);
  #ifdef CONFIG_MDNS_ENABLED
  ESP_LOGI(TAG," mdns_instance=%s",settings.mdns_instance);
  #endif
  #ifdef CONFIG_SNTP_ENABLED
  ESP_LOGI(TAG," ntp_server=%s",settings.ntp_server);
  ESP_LOGI(TAG," timezone=%s",settings.timezone);
  #endif
  ESP_LOGI(TAG," dhcp=%u",settings.dhcp);
  ESP_LOGI(TAG," ip=%s",ip4addr_ntoa(&settings.ip));
  ESP_LOGI(TAG," netmask=%s",ip4addr_ntoa(&settings.netmask));
  ESP_LOGI(TAG," gateway=%s",ip4addr_ntoa(&settings.gateway));
  ESP_LOGI(TAG," dns1=%s",ip4addr_ntoa(&settings.dns1));
  ESP_LOGI(TAG," dns2=%s",ip4addr_ntoa(&settings.dns2));
  ESP_LOGI(TAG," led_intensity=%u",settings.led_intensity);
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
void app_settings_reset() {
  nvs_handle_t handle;

  ESP_LOGI(TAG,"Erasing settings from NVS");
  if (nvs_open(NVS_KEY,NVS_READWRITE,&handle) == ESP_OK) {
    nvs_erase_all(handle);
    nvs_close(handle);
  }
  ESP_LOGI(TAG,"Restoring default settings");
  memset(&settings,0,sizeof(settings));
  strncpy(settings.wifi_ssid,CONFIG_ESP_WIFI_SSID,LEN_WIFI_SSID );
  strncpy(settings.wifi_password,CONFIG_ESP_WIFI_PASSWORD,LEN_WIFI_PASSWORD);
  strncpy(settings.hostname,CONFIG_LWIP_LOCAL_HOSTNAME,LEN_HOSTNAME);
  #ifdef CONFIG_MDNS_ENABLED
  strncpy(settings.mdns_instance,CONFIG_MDNS_INSTANCE,LEN_MDNS_INSTANCE);
  #endif
  #ifdef CONFIG_SNTP_ENABLED
  strncpy(settings.ntp_server,CONFIG_NTP_SERVER,LEN_NTP_SERVER);
  strncpy(settings.timezone,CONFIG_TIMEZONE,LEN_TIMEZONE);
  #endif
  settings.dhcp = false;
  #ifdef CONFIG_CLIENT_DHCP
  settings.dhcp = true;
  #endif
  ip4addr_aton(CONFIG_CLIENT_GATEWAY, &settings.gateway);
  ip4addr_aton(CONFIG_CLIENT_IP, &settings.ip);
  ip4addr_aton(CONFIG_CLIENT_NETMASK, &settings.netmask);
  ip4addr_aton(CONFIG_CLIENT_DNS1, &settings.dns1);
  ip4addr_aton(CONFIG_CLIENT_DNS2, &settings.dns2);
  settings.led_intensity = CONFIG_REMO1_LED_INTENSITY;
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
void app_settings_save() {
  nvs_handle_t handle;
  esp_err_t ret;

  ret = nvs_open(NVS_KEY,NVS_READWRITE,&handle);
  if (ret == ESP_OK) {
    settings.size = sizeof(settings);
    ret = nvs_set_blob(handle,"settings",&settings,sizeof(settings));
    if (ret == ESP_OK) {
      nvs_commit(handle);
      ESP_LOGI(TAG,"Saved settings to NVS");
      log_settings();
    } else {
      ESP_LOGE(TAG,"§>Error (%d) saving settings to NVS<§",ret);
    }
    nvs_close(handle);
  } else {
    ESP_LOGE(TAG,"§>Error (%d) opening settings<§",ret);
  }
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
void app_settings_startup() {
  nvs_handle_t handle;

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);  

  ESP_LOGI(TAG,"NVS Flash Init");
  
  ret = nvs_open(NVS_KEY,NVS_READONLY,&handle);
  if (ret == ESP_OK) {
    size_t size = sizeof(settings);
    ret = nvs_get_blob(handle,"settings",&settings,&size);
    if (ret == ESP_OK) { 
      if (settings.size == sizeof(settings)) {
        ESP_LOGI(TAG,"Settings loaded from NVS");
        log_settings();
      } else {
        app_settings_reset();
        app_settings_save();
      }
    } else {
      app_settings_reset();
      app_settings_save();
    }
    nvs_close(handle);
  } else {
    app_settings_reset();
    app_settings_save(); 
  } 
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
void app_settings_shutdown() {
  app_settings_save();
  nvs_flash_deinit();
}
