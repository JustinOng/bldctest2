/*
 * motor.cpp
 *
 *  Created on: May 1, 2020
 *      Author: Justin
 */

#include "motor.hpp"

void Motor::setup(TIM_HandleTypeDef *_htim, GPIO_TypeDef *_pin_sleep_port, uint16_t _pin_sleep) {
	htim = _htim;
	pin_sleep_port = _pin_sleep_port;
	pin_sleep = _pin_sleep;

	htim->Instance->CCR1 = 0;
	htim->Instance->CCR2 = 0;
	htim->Instance->CCR3 = 0;

	HAL_TIM_Base_Init(htim);
	HAL_TIM_Base_Start_IT(htim);
	HAL_TIM_PWM_Init(htim);
	HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(htim, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(htim, TIM_CHANNEL_3);

	__HAL_TIM_MOE_ENABLE(htim);
	htim->Instance->CCR1 = 2048;

	//enable();

	HAL_GPIO_WritePin(PHASE_EN_1_GPIO_Port, PHASE_EN_1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PHASE_EN_2_GPIO_Port, PHASE_EN_2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PHASE_EN_3_GPIO_Port, PHASE_EN_3_Pin, GPIO_PIN_SET);
}

char tx_data[] = "AAAABB\r\n";
uint16_t enc_spi_tx = 0xFFFF;
uint16_t enc_spi_rx = 0;

void Motor::loop(void) {
	static uint16_t electrical_angle = 0;

	pos = enc_spi_rx & 0x3FFF;

	__HAL_SPI_ENABLE(&hspi1);
	SPI1_CS_GPIO_Port->ODR &= ~SPI1_CS_Pin;
	HAL_SPI_TransmitReceive_IT(&hspi1, (uint8_t *) &enc_spi_tx, (uint8_t *) &enc_spi_rx, 1);

	for(uint8_t i = 0; i < 3; i++) {
		uint16_t offset = electrical_angle + (120 * i);
		uint16_t power = get_sinLUT(offset) * 4;

		if (i == 0) {
			htim->Instance->CCR1 = power;
		} else if (i == 1) {
			htim->Instance->CCR2 = power;
		} else {
			htim->Instance->CCR3 = power;
		}
		//*(&(htim->Instance->CCR1) + 4 * i) = power;
	}

	electrical_angle++;

	if (electrical_angle >= len_sinLUT) {
		electrical_angle = 0;
	}

//	convert_current();
//	HAL_ADC_Start_DMA(&hadc1, (uint32_t *) adc_readings, 3);
//
//	const float CURRENT_KP = 10000.0;
//	const float CURRENT_KI = 0.1;
//
//	const uint8_t PHASE_MEAS = 0;
//	const uint8_t PHASE_DRIVE = 1;
//
//	float setpoint = 0.05;
//	float error = setpoint - current[PHASE_MEAS];
//
//	int16_t power = CURRENT_KP * error;
//
//	if (power > 3072) power = 3072;
//	if (power < 0) power = 0;
//
//	htim->Instance->CCR2 = power;
//
//	//*((float *) tx_data) = current[PHASE_MEAS];
//	//*((int16_t *) &tx_data[4]) = adc_readings[PHASE_MEAS];
//	*((uint16_t *) tx_data) = adc_readings[0];
//	*((uint16_t *) &tx_data[2]) = adc_readings[1];
//	*((uint16_t *) &tx_data[4]) = adc_readings[2];
//
//	huart1.Instance->CR3 |= USART_CR3_DMAT;
//	HAL_DMA_Start_IT(&hdma_usart1_tx, (uint32_t) &tx_data, (uint32_t) &huart1.Instance->DR, 8);
}

void Motor::enable(void) {
	HAL_GPIO_WritePin(pin_sleep_port, pin_sleep, GPIO_PIN_SET);
}

void Motor::disable(void) {
	HAL_GPIO_WritePin(pin_sleep_port, pin_sleep, GPIO_PIN_RESET);
}

void Motor::convert_current(void) {
	for(uint8_t i = 0; i < 3; i++) {
		current[i] = current_from_adc(adc_readings[i]);
	}
}
