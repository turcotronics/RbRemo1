/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)
    
   The names of Turcotronics, Robello and TuT may not be used to endorse or promote
   products derived from this software without specific prior written permission.

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "driver/gpio.h"

#include "time.h"
#include "sys/time.h"

#define KEYUP    36
#define KEYDOWN  39
#define KEYLEFT  34
#define KEYRIGHT 35
#define GPIO_KEYS ((1ULL<<KEYUP) | (1ULL<<KEYDOWN) | (1ULL<<KEYLEFT) | (1ULL<<KEYRIGHT))

#define SPP_TAG "Robello_Remo1"
#define SPP_SERVER_NAME "Robello_Remo1"
#define EXCAMPLE_DEVICE_NAME "Robello_Remo1"
#define SPP_SHOW_DATA 0
#define SPP_SHOW_SPEED 1
#define SPP_SHOW_MODE SPP_SHOW_DATA    /*Choose show mode: show data or speed*/

static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;

uint32_t spphandle = 0;

#if (SPP_SHOW_MODE != SPP_SHOW_DATA)
static struct timeval time_new;
static long data_num = 0;
#endif
static struct timeval time_old;

//static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_NONE;
static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;
static const esp_spp_role_t role_master = ESP_SPP_ROLE_MASTER;

static esp_bd_addr_t peer_bd_addr;
static uint8_t peer_bdname_len;
static char peer_bdname[ESP_BT_GAP_MAX_BDNAME_LEN + 1];
static const char remote_device_name[] = "Robello_T1";
static const esp_bt_inq_mode_t inq_mode = ESP_BT_INQ_MODE_GENERAL_INQUIRY;
static const uint8_t inq_len = 30;
static const uint8_t inq_num_rsps = 0;

#if (SPP_SHOW_MODE == SPP_SHOW_DATA)
#define SPP_DATA_LEN 256
#else
#define SPP_DATA_LEN ESP_SPP_MAX_MTU
#endif
static char spp_data[SPP_DATA_LEN];

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
void gpio_task(void* arg)
{
    bool bKeyUp = false;
    bool bKeyDown = false;
    bool bKeyLeft = false;
    bool bKeyRight = false;

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_KEYS;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    while(1) {
        if((gpio_get_level(KEYUP)==0) && !bKeyUp) {
            bKeyUp=true;
            ets_printf("§Bb1");
            sprintf(spp_data, "§Bb1");
            if(spphandle)
                esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
        }
        if((gpio_get_level(KEYUP)==1) && bKeyUp) {
            bKeyUp=false;
            ets_printf("§Bb0");
            sprintf(spp_data, "§Bb0");
            if(spphandle)
                esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
        }
        if((gpio_get_level(KEYDOWN)==0) && !bKeyDown) {
            bKeyDown=true;
            ets_printf("§Bh1");
            sprintf(spp_data, "§Bh1");
            if(spphandle)
                esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
        }
        if((gpio_get_level(KEYDOWN)==1) && bKeyDown) {
            bKeyDown=false;
            ets_printf("§Bh0");
            sprintf(spp_data, "§Bh0");
            if(spphandle)
                esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
        }
        if((gpio_get_level(KEYLEFT)==0) && !bKeyLeft) {
            bKeyLeft=true;
            ets_printf("§Bd1");
            sprintf(spp_data, "§Bd1");
            if(spphandle)
                esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
        }
        if((gpio_get_level(KEYLEFT)==1) && bKeyLeft) {
            bKeyLeft=false;
            ets_printf("§Bd0");
            sprintf(spp_data, "§Bd0");
            if(spphandle)
                esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
        }
        if((gpio_get_level(KEYRIGHT)==0) && !bKeyRight) {
            bKeyRight=true;
            ets_printf("§Bf1");
            sprintf(spp_data, "§Bf1");
            if(spphandle)
                esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
        }
        if((gpio_get_level(KEYRIGHT)==1) && bKeyRight) {
            bKeyRight=false;
            ets_printf("§Bf0");
            sprintf(spp_data, "§Bf0");
            if(spphandle)
                esp_spp_write(spphandle, strlen(spp_data), (uint8_t *)spp_data);
        }

        vTaskDelay(33 / portTICK_RATE_MS);
    }
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
#if (SPP_SHOW_MODE != SPP_SHOW_DATA)
static void print_speed(void)
{
    float time_old_s = time_old.tv_sec + time_old.tv_usec / 1000000.0;
    float time_new_s = time_new.tv_sec + time_new.tv_usec / 1000000.0;
    float time_interval = time_new_s - time_old_s;
    float speed = data_num * 8 / time_interval / 1000.0;
    ESP_LOGI(SPP_TAG, "speed(%fs ~ %fs): %f kbit/s" , time_old_s, time_new_s, speed);
    data_num = 0;
    time_old.tv_sec = time_new.tv_sec;
    time_old.tv_usec = time_new.tv_usec;
}
#endif

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
static bool get_name_from_eir(uint8_t *eir, char *bdname, uint8_t *bdname_len)
{
    uint8_t *rmt_bdname = NULL;
    uint8_t rmt_bdname_len = 0;

    if (!eir) {
        return false;
    }

    rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME, &rmt_bdname_len);
    if (!rmt_bdname) {
        rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME, &rmt_bdname_len);
    }

    if (rmt_bdname) {
        if (rmt_bdname_len > ESP_BT_GAP_MAX_BDNAME_LEN) {
            rmt_bdname_len = ESP_BT_GAP_MAX_BDNAME_LEN;
        }

        if (bdname) {
            memcpy(bdname, rmt_bdname, rmt_bdname_len);
            bdname[rmt_bdname_len] = '\0';
        }
        if (bdname_len) {
            *bdname_len = rmt_bdname_len;
        }
        return true;
    }

    return false;
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    char buf[1024];
    char spp_data[256];
    switch (event) {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
        esp_bt_dev_set_device_name(EXCAMPLE_DEVICE_NAME);
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        esp_spp_start_srv(sec_mask,role_slave, 0, SPP_SERVER_NAME);
        esp_bt_gap_start_discovery(inq_mode, inq_len, inq_num_rsps);
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT: //Client side
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT status=%d scn_num=%d",param->disc_comp.status, param->disc_comp.scn_num);
        if (param->disc_comp.status == ESP_SPP_SUCCESS) {
            esp_spp_connect(sec_mask, role_master, param->disc_comp.scn[0], peer_bd_addr);
        }
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT"); //Client side
        //esp_spp_write(param->srv_open.handle, SPP_DATA_LEN, (uint8_t *)spp_data);
        spphandle = param->srv_open.handle;
        gettimeofday(&time_old, NULL);
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT");
        spphandle = 0;
        break;
    case ESP_SPP_START_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
        break;
    case ESP_SPP_CL_INIT_EVT: //Client side
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT: //Client Server side
#if (SPP_SHOW_MODE == SPP_SHOW_DATA)
        ESP_LOGI(SPP_TAG, "ESP_SPP_DATA_IND_EVT len=%d handle=%d",
                 param->data_ind.len, param->data_ind.handle);
        if (param->data_ind.len < 1023) {
            snprintf(buf, (size_t)param->data_ind.len+1, (char *)param->data_ind.data);
            printf("%s\n", buf);
            sprintf(spp_data, "Receined characters: %d\n", param->data_ind.len);
            //esp_spp_write(param->write.handle, strlen(spp_data), (uint8_t *)spp_data);
        }
        else {
            esp_log_buffer_hex("",param->data_ind.data,param->data_ind.len);
        }
#else
        gettimeofday(&time_new, NULL);
        data_num += param->data_ind.len;
        if (time_new.tv_sec - time_old.tv_sec >= 3) {
            print_speed();
        }
#endif
        break;
    case ESP_SPP_CONG_EVT:
        #if (SPP_SHOW_MODE == SPP_SHOW_DATA)
        ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT cong=%d", param->cong.cong);
#endif
        if (param->cong.cong == 0) {
            //esp_spp_write(param->cong.handle, SPP_DATA_LEN, (uint8_t *)spp_data);
        }
        break;
    case ESP_SPP_WRITE_EVT: //Client Server side
        #if (SPP_SHOW_MODE == SPP_SHOW_DATA)
        ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT len=%d cong=%d", param->write.len , param->write.cong);
        //esp_log_buffer_hex("",spp_data,SPP_DATA_LEN);
#else
        gettimeofday(&time_new, NULL);
        data_num += param->write.len;
        if (time_new.tv_sec - time_old.tv_sec >= 3) {
            print_speed();
        }
#endif
        if (param->write.cong == 0) {
            //esp_spp_write(param->write.handle, SPP_DATA_LEN, (uint8_t *)spp_data);
        }
        break;
    case ESP_SPP_SRV_OPEN_EVT: //Server side
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT");
        gettimeofday(&time_old, NULL);
        break;
    default:
        break;
    }
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
static void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch(event){
    case ESP_BT_GAP_DISC_RES_EVT: //Client side
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_DISC_RES_EVT");
        esp_log_buffer_hex(SPP_TAG, param->disc_res.bda, ESP_BD_ADDR_LEN);
        for (int i = 0; i < param->disc_res.num_prop; i++){
            if (param->disc_res.prop[i].type == ESP_BT_GAP_DEV_PROP_EIR
                && get_name_from_eir(param->disc_res.prop[i].val, peer_bdname, &peer_bdname_len)){
                esp_log_buffer_char(SPP_TAG, peer_bdname, peer_bdname_len);
                if (strlen(remote_device_name) == peer_bdname_len
                    && strncmp(peer_bdname, remote_device_name, peer_bdname_len) == 0) {
                    memcpy(peer_bd_addr, param->disc_res.bda, ESP_BD_ADDR_LEN);
                    esp_spp_start_discovery(peer_bd_addr);
                    esp_bt_gap_cancel_discovery();
                }
            }
        }
        break;
    case ESP_BT_GAP_DISC_STATE_CHANGED_EVT: //Client side
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_DISC_STATE_CHANGED_EVT");
        break;
    case ESP_BT_GAP_RMT_SRVCS_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_RMT_SRVCS_EVT");
        break;
    case ESP_BT_GAP_RMT_SRVC_REC_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_RMT_SRVC_REC_EVT");
        break;
    case ESP_BT_GAP_AUTH_CMPL_EVT:{
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(SPP_TAG, "authentication success: %s", param->auth_cmpl.device_name);
            esp_log_buffer_hex(SPP_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
        } else {
            ESP_LOGE(SPP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:{
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit) {
            ESP_LOGI(SPP_TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        } else {
            ESP_LOGI(SPP_TAG, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif

    default:
        break;
    }
}

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
void app_bt_startup()
{
    for (int i = 0; i < SPP_DATA_LEN; ++i) {
        spp_data[i] = 0;
    }
    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if (esp_bt_controller_init(&bt_cfg) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize controller failed\n", __func__);
        return;
    }

    if (esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable controller failed\n", __func__);
        return;
    }

    if (esp_bluedroid_init() != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed\n", __func__);
        return;
    }

    if (esp_bluedroid_enable() != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable bluedroid failed\n", __func__);
        return;
    }
    
    if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s gap register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if (esp_spp_register_callback(esp_spp_cb) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp register failed\n", __func__);
        return;
    }

    if (esp_spp_init(esp_spp_mode) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp init failed\n", __func__);
        return;
    }
    
#if (CONFIG_BT_SSP_ENABLED == true)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif

    /*
     * Set default parameters for Legacy Pairing
     * Use variable pin, input pin code when pairing
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);
}
