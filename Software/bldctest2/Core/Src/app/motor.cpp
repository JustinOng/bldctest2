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

char tx_data[] = "ABCDEF\r\n";
uint16_t enc_spi_tx = 0xFFFF;
uint16_t enc_spi_rx = 0;

void Motor::loop(void) {
	pos = enc_spi_rx & 0x3FFF;

//	*((uint16_t *) tx_data) = pos;
//	*((uint16_t *) &tx_data[2]) = electrical_angle;
//	huart1.Instance->CR3 |= USART_CR3_DMAT;
//	HAL_DMA_Start_IT(&hdma_usart1_tx, (uint32_t) &tx_data, (uint32_t) &huart1.Instance->DR, 6);

	__HAL_SPI_ENABLE(&hspi1);
	SPI1_CS_GPIO_Port->ODR &= ~SPI1_CS_Pin;
	HAL_SPI_TransmitReceive_IT(&hspi1, (uint8_t *) &enc_spi_tx, (uint8_t *) &enc_spi_rx, 1);

	switch(state) {
		case STATE_START_CALIBRATE:
		case STATE_CALIBRATE:
			loop_calibrate();
			break;
		case STATE_ACTIVE:
			loop_active();
			break;
		case STATE_IDLE:
		default:
			break;
	}

	prev_pos = pos;
}

void Motor::loop_active(void) {
	// scale corrected position into corresponding electrical angle
	uint16_t base_angle = (float) (get_corrected_pos() % encoder_ticks_per_cycle) * len_sinLUT / encoder_ticks_per_cycle;

	int32_t pos_delta = get_pos_delta();
	// scale up the delta to encoder ticks in one second
	int32_t error = target_velocity - (pos_delta * UPDATE_FREQUENCY / LEN_POS_DELTAS);

	int8_t direction = (error >= 0 ? 1 : -1) * (len_sinLUT / 4);

	float power = KP_VEL * abs(error);

	if (power > MAX_POWER) power = MAX_POWER;

	*((int32_t *) tx_data) = error;
	*((uint16_t *) &tx_data[4]) = pos_delta;
	huart1.Instance->CR3 |= USART_CR3_DMAT;
	HAL_DMA_Start_IT(&hdma_usart1_tx, (uint32_t) &tx_data, (uint32_t) &huart1.Instance->DR, 8);

	for(uint8_t i = 0; i < 3; i++) {
		int16_t angle = base_angle + direction + (len_sinLUT * i / 3);
		uint16_t phase_power = get_sinLUT(angle) * power;

		if (i == 0) {
			htim->Instance->CCR1 = phase_power;
		} else if (i == 1) {
			htim->Instance->CCR2 = phase_power;
		} else {
			htim->Instance->CCR3 = phase_power;
		}
	}
}

int32_t Motor::get_pos_delta(void) {
	// computes the sum of position deltas over the last LEN_POS_DELTAS * 1 / UPDATE_FREQUENCY

	// assume at this point, encoder direction has been calibrated so that
	// the encoder direction increases with electrical direction
	int16_t delta = pos - prev_pos;

	// if changes too much, assume that the encoder just wrapped around
	// add/remove ENCODER_CPR to account for the change
	if (delta < -(ENCODER_CPR / 2)) delta += ENCODER_CPR;
	else if (delta > (ENCODER_CPR / 2)) delta -= ENCODER_CPR;

	// add in new delta value
	pos_delta_sum += delta;
	// subtract oldest delta value
	pos_delta_sum -= pos_deltas[pos_delta_head];

	// save new delta value into oldest slot
	pos_deltas[pos_delta_head] = delta;

	pos_delta_head++;
	if (pos_delta_head >= LEN_POS_DELTAS) pos_delta_head = 0;

	return pos_delta_sum;
}

void Motor::loop_calibrate(void) {
	static uint16_t count = 0;
	static int16_t total_delta = 0;

	int16_t delta = pos - prev_pos;

	if (state == STATE_START_CALIBRATE) {
		count = 0;

		total_delta = 0;
		state = STATE_CALIBRATE;

		enable();
		return;
	}

	uint16_t electrical_angle = count / CALIBRATION_DELAY;

	// detect extreme delta as encoder wrap around
	if (delta > (ENCODER_CPR / 2) || delta < -(ENCODER_CPR / 2)) {
		// at this point, current electrical angle can be treated as the offset
		// if total_delta < 0, need to invert direction
		// because encoder dir is opposite from electrical dir

		calibration_offset = electrical_angle;

		state = STATE_IDLE;
		disable();

		return;
	}

	if (count % CALIBRATION_DELAY == 0) {
		total_delta += delta;

		for(uint8_t i = 0; i < 3; i++) {
			uint16_t offset = electrical_angle + (len_sinLUT * i / 3);
			uint16_t power = get_sinLUT(offset) * 2;

			if (i == 0) {
				htim->Instance->CCR1 = power;
			} else if (i == 1) {
				htim->Instance->CCR2 = power;
			} else {
				htim->Instance->CCR3 = power;
			}
		}
	}

	count++;
	if ((count / CALIBRATION_DELAY) >= len_sinLUT) count = 0;
}

uint16_t Motor::get_corrected_pos(void) {
	// this could be optimised by using a single & instead
	// but ENCODER_CPR has to be a power of 2
	int16_t corrected_pos = pos - calibration_offset;
	if (corrected_pos < 0) corrected_pos += ENCODER_CPR;
	else if (corrected_pos >= ENCODER_CPR) corrected_pos -= ENCODER_CPR;

	return (uint16_t) corrected_pos;
}

void Motor::calibrate(void) {
	if (state == STATE_START_CALIBRATE || state == STATE_CALIBRATE) return;

	state = STATE_START_CALIBRATE;
}

void Motor::active(void) {
	enable();
	state = STATE_ACTIVE;
}

void Motor::enable(void) {
	htim->Instance->CCR1 = 0;
	htim->Instance->CCR2 = 0;
	htim->Instance->CCR3 = 0;

	pin_sleep_port->BSRR = pin_sleep;
}

void Motor::disable(void) {
	htim->Instance->CCR1 = 0;
	htim->Instance->CCR2 = 0;
	htim->Instance->CCR3 = 0;

	pin_sleep_port->BRR = pin_sleep;
}

void Motor::convert_current(void) {
	for(uint8_t i = 0; i < 3; i++) {
		current[i] = current_from_adc(adc_readings[i]);
	}
}
