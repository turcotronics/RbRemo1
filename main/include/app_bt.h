/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)
    
   The names of Turcotronics, Robello and TuT may not be used to endorse or promote
   products derived from this software without specific prior written permission.

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef _APP_BT_H_
#define _APP_BT_H_

#ifdef __cplusplus
extern "C" {
#endif

void app_bt_startup();
void gpio_task(void* arg);

#ifdef __cplusplus
}
#endif

#endif
