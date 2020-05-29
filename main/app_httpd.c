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

#include "app_httpd.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "app_settings.h"
#ifdef CONFIG_SNTP_ENABLED
#include <time.h>
#include <sys/time.h>
#include "app_sntp.h"
#endif
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"

static const char* TAG = "remo1_httpd";

httpd_handle_t remo1_httpd = NULL;

extern EventGroupHandle_t event_group;

extern uint32_t spphandle;
static char spp_data[256];

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
static esp_err_t cmd_handler(httpd_req_t *req){
    char*  buf;
    size_t buf_len;
    char variable[32] = {0,};
    char value[32] = {0,};

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = (char*)malloc(buf_len);
        if(!buf){
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            if (httpd_query_key_value(buf, "var", variable, sizeof(variable)) == ESP_OK &&
                httpd_query_key_value(buf, "val", value, sizeof(value)) == ESP_OK) {
            } else {
                free(buf);
                httpd_resp_send_404(req);
                return ESP_FAIL;
            }
        } else {
            free(buf);
            httpd_resp_send_404(req);
            return ESP_FAIL;
        }
        free(buf);
    } else {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    int val = atoi(value);
    ESP_LOGI(TAG, "%s = %d", variable, val);
    int res = 0;

    if(!strcmp(variable, "hostname")) strncpy(settings.hostname,value,LEN_HOSTNAME);
    else if(!strcmp(variable, "wifi_ssid")) strncpy(settings.wifi_ssid,value,LEN_WIFI_SSID);
    else if(!strcmp(variable, "wifi_password")) strncpy(settings.wifi_password,value,LEN_WIFI_PASSWORD);
    #ifdef CONFIG_MDNS_ENABLED
    else if(!strcmp(variable, "mdns_instance")) strncpy(settings.mdns_instance,value,LEN_MDNS_INSTANCE);
    #endif
    else if(!strcmp(variable, "dhcp")) settings.dhcp = val;
    #ifdef CONFIG_SNTP_ENABLED
    else if(!strcmp(variable, "ntp_server")) strncpy(settings.ntp_server,value,LEN_NTP_SERVER); 
    else if(!strcmp(variable, "timezone")) { strncpy(settings.timezone,value,LEN_TIMEZONE); setenv("TZ", settings.timezone, 1); tzset(); } 
    #endif
    else if(!strcmp(variable, "ip")) settings.ip.addr = ipaddr_addr(value);
    else if(!strcmp(variable, "netmask")) settings.netmask.addr = ipaddr_addr(value);
    else if(!strcmp(variable, "gateway")) settings.gateway.addr = ipaddr_addr(value);
    else if(!strcmp(variable, "dns1")) settings.dns1.addr = ipaddr_addr(value);
    else if(!strcmp(variable, "dns2")) settings.dns2.addr = ipaddr_addr(value);
    else if(!strcmp(variable, "led_intensity")) settings.led_intensity = val;
    else if(!strcmp(variable, "susinButton")) {
		ets_printf("§Ba%d",val);
		sprintf(spp_data, "§Ba%d",val);
        if(spphandle)
			esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
		}
    else if(!strcmp(variable, "suButton")) {
        ets_printf("§Bb%d",val);
        sprintf(spp_data, "§Bb%d",val);
        if(spphandle)
			esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
		}
    else if(!strcmp(variable, "sudesButton")) {
        ets_printf("§Bc%d",val);
        sprintf(spp_data, "§Bc%d",val);
        if(spphandle)
			esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
		}
    else if(!strcmp(variable, "sinButton")) {
		ets_printf("§Bd%d",val);
		sprintf(spp_data, "§Bd%d",val);
        if(spphandle)
			esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
		}
    else if(!strcmp(variable, "cenButton")) {
		ets_printf("§Be%d",val);
		sprintf(spp_data, "§Be%d",val);
        if(spphandle)
			esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
		}
    else if(!strcmp(variable, "desButton")) {
		ets_printf("§Bf%d",val); 
		sprintf(spp_data, "§Bf%d",val);
        if(spphandle)
			esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
		}
    else if(!strcmp(variable, "giusinButton")) {
		ets_printf("§Bg%d",val); 
		sprintf(spp_data, "§Bg%d",val);
        if(spphandle)
			esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
		}
    else if(!strcmp(variable, "giuButton")) {
		ets_printf("§Bh%d",val);
		sprintf(spp_data, "§Bh%d",val);
        if(spphandle)
			esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
		}
    else if(!strcmp(variable, "giudesButton")) {
		ets_printf("§Bi%d",val);
		sprintf(spp_data, "§Bi%d",val);
        if(spphandle)
			esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
		}
    else {
      res = -1;
    }

    if(res){
      return httpd_resp_send_500(req);
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
static esp_err_t store_handler(httpd_req_t *req) {
    app_settings_save();    
    return httpd_resp_send(req, NULL, 0);
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
static esp_err_t reboot_handler(httpd_req_t *req) {
    esp_err_t ret = httpd_resp_send(req, NULL, 0);
    vTaskDelay(250 / portTICK_PERIOD_MS); // Short delay to ensure the http response is sent
    esp_restart();
    return ret;
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
static esp_err_t reset_handler(httpd_req_t *req) {
    app_settings_reset();
    return httpd_resp_send(req, NULL, 0);
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
static esp_err_t status_handler(httpd_req_t *req){
    static char json_response[1024];

    char * p = json_response;
    *p++ = '{';

    p+=sprintf(p, "\"hostname\":\"%s\",", settings.hostname);
    p+=sprintf(p, "\"wifi_ssid\":\"%s\",", settings.wifi_ssid);
    p+=sprintf(p, "\"wifi_password\":\"%s\",", settings.wifi_password);
    #ifdef CONFIG_MDNS_ENABLED
    p+=sprintf(p, "\"mdns_instance\":\"%s\",", settings.mdns_instance);
    #endif
    #ifdef CONFIG_SNTP_ENABLED
    p+=sprintf(p, "\"ntp_server\":\"%s\",", settings.ntp_server);
    p+=sprintf(p, "\"timezone\":\"%s\",", settings.timezone);
    #endif
    p+=sprintf(p, "\"dhcp\":%u,", settings.dhcp);
    p+=sprintf(p, "\"ip\":\"%s\",", ip4addr_ntoa(&settings.ip));
    p+=sprintf(p, "\"netmask\":\"%s\",", ip4addr_ntoa(&settings.netmask));
    p+=sprintf(p, "\"gateway\":\"%s\",", ip4addr_ntoa(&settings.gateway));
    p+=sprintf(p, "\"dns1\":\"%s\",", ip4addr_ntoa(&settings.dns1));
    p+=sprintf(p, "\"dns2\":\"%s\",", ip4addr_ntoa(&settings.dns2));
    p+=sprintf(p, "\"led_intensity\":%u,", settings.led_intensity);
    p+=sprintf(p, "\"led_max_intensity\":%u", 255);
    
    *p++ = '}';
    *p++ = 0;
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json_response, strlen(json_response));
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
static esp_err_t stylesheet_handler(httpd_req_t *req){
    extern const unsigned char style_css_gz_start[] asm("_binary_style_css_gz_start");
    extern const unsigned char style_css_gz_end[]   asm("_binary_style_css_gz_end");
    size_t style_css_gz_len = style_css_gz_end - style_css_gz_start;

    httpd_resp_set_type(req, "text/css");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    return httpd_resp_send(req, (const char *)style_css_gz_start, style_css_gz_len);
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
static esp_err_t script_handler(httpd_req_t *req){
    extern const unsigned char script_js_gz_start[] asm("_binary_script_js_gz_start");
    extern const unsigned char script_js_gz_end[]   asm("_binary_script_js_gz_end");
    size_t script_js_gz_len = script_js_gz_end - script_js_gz_start;

    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    return httpd_resp_send(req, (const char *)script_js_gz_start, script_js_gz_len);
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
static esp_err_t favicon_handler(httpd_req_t *req){
    extern const unsigned char favicon_ico_gz_start[] asm("_binary_favicon_ico_gz_start");
    extern const unsigned char favicon_ico_gz_end[]   asm("_binary_favicon_ico_gz_end");
    size_t favicon_ico_gz_len = favicon_ico_gz_end - favicon_ico_gz_start;

    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    return httpd_resp_send(req, (const char *)favicon_ico_gz_start, favicon_ico_gz_len);
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
static esp_err_t index_handler(httpd_req_t *req){
    extern const unsigned char index_html_gz_start[] asm("_binary_index_html_gz_start");
    extern const unsigned char index_html_gz_end[]   asm("_binary_index_html_gz_end");
    size_t index_html_gz_len = index_html_gz_end - index_html_gz_start;

    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    return httpd_resp_send(req, (const char *)index_html_gz_start, index_html_gz_len);
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
void app_httpd_startup(){
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.max_uri_handlers = 10;

    httpd_uri_t index_uri = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = index_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t stylesheet_uri = {
        .uri       = "/style.css",
        .method    = HTTP_GET,
        .handler   = stylesheet_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t script_uri = {
        .uri       = "/script.js",
        .method    = HTTP_GET,
        .handler   = script_handler,
        .user_ctx  = NULL
    };
    
    httpd_uri_t favicon_uri = {
        .uri       = "/favicon.ico",
        .method    = HTTP_GET,
        .handler   = favicon_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t status_uri = {
        .uri       = "/status",
        .method    = HTTP_GET,
        .handler   = status_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t cmd_uri = {
        .uri       = "/control",
        .method    = HTTP_GET,
        .handler   = cmd_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t store_uri = {
        .uri       = "/store",
        .method    = HTTP_GET,
        .handler   = store_handler,
        .user_ctx  = NULL
    };

   httpd_uri_t reboot_uri = {
        .uri       = "/reboot",
        .method    = HTTP_GET,
        .handler   = reboot_handler,
        .user_ctx  = NULL
    };

   httpd_uri_t reset_uri = {
        .uri       = "/reset",
        .method    = HTTP_GET,
        .handler   = reset_handler,
        .user_ctx  = NULL
    };

    ESP_LOGI(TAG, "Starting web server on port: '%d'", config.server_port);
    if (httpd_start(&remo1_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(remo1_httpd, &index_uri);
        httpd_register_uri_handler(remo1_httpd, &cmd_uri);
        httpd_register_uri_handler(remo1_httpd, &store_uri);
        httpd_register_uri_handler(remo1_httpd, &status_uri);
        httpd_register_uri_handler(remo1_httpd, &reboot_uri);
        httpd_register_uri_handler(remo1_httpd, &reset_uri);
        httpd_register_uri_handler(remo1_httpd, &stylesheet_uri);
        httpd_register_uri_handler(remo1_httpd, &script_uri);
        httpd_register_uri_handler(remo1_httpd, &favicon_uri);
    }

    config.server_port += 1;
    config.ctrl_port += 1;
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
void app_httpd_shutdown() {
	ESP_LOGI(TAG, "httpd shutdown");
}
