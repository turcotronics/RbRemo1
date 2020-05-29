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

#ifndef _APP_WIFI_H_
#define _APP_WIFI_H_

#include "freertos/event_groups.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_CONNECTED_BIT  BIT0  // Event group bit is set high while connected
#define WIFI_SOFTAP_BIT     BIT1

int wifi_connection_count();
int wifi_get_rssi();
float wifi_get_tx_power();

void app_wifi_startup();
void app_wifi_shutdown();

#ifdef __cplusplus
}
#endif

#endif
