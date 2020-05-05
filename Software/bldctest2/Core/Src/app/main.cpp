/*
 * main.cpp
 *
 *  Created on: May 1, 2020
 *      Author: Justin
 */

#include "main.hpp"

Motor motor;

void main_cpp(void) {
	hardware_init();

	motor.setup(&htim1, nSLEEP_GPIO_Port, nSLEEP_Pin);

	while(1) {
		GPIO_PinState st = HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin);
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, st);

		if (st) {
			motor.disable();
		} else {
			motor.enable();
		}
	}
	/*GPIO_PinState st = HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, st);

	if (st) {
	  __HAL_TIM_SET_COMPARE(&htim1, PH_2_CHANNEL, 0000);
	} else {
	  __HAL_TIM_SET_COMPARE(&htim1, PH_2_CHANNEL, 8000);
	}*/
}
