/**
 * @file  RbRemo1.c
 * 
 * \brief Robello Remo1 remote utility
 * 
 * Author: Rodolfo Turco, Turcotronics.
 *
 * Date: Created on 20 maggio 2020
 *
 * Link: https://www.turcotronics.it

  (c) Copyright 2020 Turcotronics.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
 
  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
 
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
 
  3. The names of Turcotronics, Robello and TuT may not be used to endorse or promote
     products derived from this software without specific prior written permission.

    It is your responsibility to comply with third party license terms applicable to
    your use of third party software (including open source software) that may
    accompany Turcotronics software.

    THIS SOFTWARE IS SUPPLIED BY TURCOTRONICS "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.
    IN NO EVENT WILL TURCOTRONICS BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF TURCOTRONICS HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, TURCOTRONICS'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO TURCOTRONICS FOR THIS SOFTWARE.
 **/

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
#include <stdio.h>
#include <stdlib.h>

#include "RbRemo1.h"

/*! Return values */
typedef enum {
	RETVAL_OK           	= 0x00U,
	RETVAL_ERROR  			= 0x01U,
	RETVAL_BUSY     		= 0x02U,
	RETVAL_TIMEOUT  		= 0x03U,
	RETVAL_INVALID_INPUT	= 0x66U
} RetVal_t;

/*! RGB led */
typedef struct {
	uint8_t R; //Red
	uint8_t G; //Green
	uint8_t B; //Blue
} color_t;

color_t RGBLed = { 0, 0, 0 };

//! For debug use, set/unset the TODO pin, useful for oscilloscope tests
//TODO #define DEBUG_PULSE_ON  LED_D3_GPIO_Port->BSRR = (uint32_t)LED_D3_Pin << 16U; //! PA7=0, D3 LED on
//TODO #define DEBUG_PULSE_OFF LED_D3_GPIO_Port->BSRR = LED_D3_Pin; //! PA7=1, D3 LED off

/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
/**
 * /brief Set RGB Led color
 *
 */
void RbRemo1_RGBLedWrite(color_t RGBLed) {
		// Chip WS2812B_RGB_LED
		// RGB LED: Tx 24bit, GRB order, MSB first, 26.4μs
		// reset = 0 for >280μs
		// bit0  = 1 for 220ns~380ns (300ns), 0 for 580ns~1μs (800ns);   Tb=1.100μs
		// bit1  = 1 for 580ns~1μs (790ns),   0 for 220ns~420ns (320ns); Tb=1.100μs

		//Green
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.G & 0x80) { //bit7==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit7==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.G & 0x40) { //bit6==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit6==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.G & 0x20) { //bit5==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit5==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.G & 0x10) { //bit4==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit4==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.G & 0x08) { //bit3==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit3==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.G & 0x04) { //bit2==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit2==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.G & 0x02) { //bit1==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit1==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.G & 0x01) { //bit0==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit0==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}

		//Red
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.R & 0x80) { //bit7==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit7==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.R & 0x40) { //bit6==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit6==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.R & 0x20) { //bit5==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit5==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.R & 0x10) { //bit4==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit4==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.R & 0x08) { //bit3==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit3==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.R & 0x04) { //bit2==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit2==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.R & 0x02) { //bit1==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit1==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.R & 0x01) { //bit0==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit0==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}

		//Blue
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.B & 0x80) { //bit7==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit7==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.B & 0x40) { //bit6==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit6==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.B & 0x20) { //bit5==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit5==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.B & 0x10) { //bit4==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit4==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.B & 0x08) { //bit3==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit3==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.B & 0x04) { //bit2==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit2==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.B & 0x02) { //bit1==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit1==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
		HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_SET);
		if (RGBLed.B & 0x01) { //bit0==1?
			delay_ns = 9;
			while (delay_ns--)
				;
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 0;
			while (delay_ns--)
				;
		} else { //bit0==0
			HAL_GPIO_WritePin(LEDRGB_GPIO_Port, LEDRGB_Pin, GPIO_PIN_RESET);
			delay_ns = 10;
			while (delay_ns--)
				;
		}
}

