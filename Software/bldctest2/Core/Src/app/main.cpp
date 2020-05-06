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

//		HAL_ADC_Start(&hadc2);
//		HAL_ADCEx_MultiModeStart_DMA(&hadc1, &adc_data, 1);
	}
}
