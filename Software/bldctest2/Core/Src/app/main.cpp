/*
 * main.cpp
 *
 *  Created on: May 1, 2020
 *      Author: Justin
 */

#include "main.hpp"

Motor motor;

uint32_t adc_data;

char adc_str[] = "AAAA\r\n";
//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
//	*((uint32_t *) adc_str) = adc_data;
//	huart1.Instance->CR3 |= USART_CR3_DMAT;
//	HAL_DMA_Start_IT(&hdma_usart1_tx, (uint32_t) &adc_str, (uint32_t) &huart1.Instance->DR, 6);
//}

void main_cpp(void) {
	hardware_init();

	motor.setup(&htim1, nSLEEP_GPIO_Port, nSLEEP_Pin);

	GPIO_PinState prev_st = HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin);

	bool calibrated = false;
	while(1) {
		GPIO_PinState st = HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin);
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, st);

		if (!st && prev_st) {
			if (calibrated) {
				motor.active();
			}
			else {
				motor.calibrate();
				while(motor.is_calibrating());
				calibrated = true;
			}
		}

		if (st && !prev_st) {
			motor.disable();
		}

		prev_st = st;

//		HAL_ADC_Start(&hadc2);
//		HAL_ADCEx_MultiModeStart_DMA(&hadc1, &adc_data, 1);
	}
}
