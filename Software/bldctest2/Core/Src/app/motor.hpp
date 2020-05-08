/*
 * motor.hpp
 *
 *  Created on: May 1, 2020
 *      Author: Justin
 */

#ifndef SRC_APP_MOTOR_HPP_
#define SRC_APP_MOTOR_HPP_

#include "main.h"

#define CS_RESISTANCE 4990
// gain of the current sense amplifiers in the MP6540
// varies by temperature, might be worth measuring temp and compensating accordingly?
#define CS_GAIN 9200

#define ENCODER_CPR 16384
#define POLE_PAIRS 10

// when calibrating, how many update events before incrementing electrical angle
// CALIBRATION_DELAY * len_sinLUT should not exceed 65535
#define CALIBRATION_DELAY 16

const uint16_t len_sinLUT = 256;
const uint8_t sinLUT[len_sinLUT] = {127, 122, 117, 113, 108, 104, 99, 94, 90, 85, 81, 77, 72, 67, 63, 58, 54, 49, 45, 41, 38, 33, 31, 30, 28, 27, 26, 25, 24, 23, 22, 21, 21, 20, 19, 19, 18, 18, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 19, 19, 20, 20, 21, 22, 23, 23, 25, 25, 27, 28, 29, 30, 32, 30, 29, 28, 27, 25, 25, 23, 23, 22, 21, 20, 20, 19, 19, 18, 18, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 19, 19, 20, 21, 21, 22, 23, 24, 25, 26, 27, 28, 30, 31, 33, 38, 41, 45, 49, 54, 58, 63, 67, 72, 77, 81, 85, 90, 94, 99, 104, 108, 113, 117, 122, 126, 132, 136, 141, 145, 150, 155, 159, 164, 168, 173, 177, 182, 186, 190, 195, 199, 204, 208, 213, 216, 220, 223, 224, 225, 227, 228, 229, 230, 230, 232, 232, 233, 233, 234, 235, 235, 236, 236, 236, 236, 237, 237, 237, 237, 237, 236, 236, 236, 235, 235, 234, 234, 233, 233, 232,
		231, 230, 229, 228, 227, 226, 224, 223, 222, 223, 224, 226, 227, 228, 229, 230, 231, 232, 233, 233, 234, 234, 235, 235, 236, 236, 236, 237, 237, 237, 237, 237, 236, 236, 236, 236, 235, 235, 234, 233, 233, 232, 232, 230, 230, 229, 228, 227, 225, 224, 223, 220, 216, 213, 208, 204, 199, 195, 190, 186, 182, 177, 173, 168, 164, 159, 155, 150, 145, 141, 136, 132};

const uint16_t encoder_ticks_per_cycle = (float) ENCODER_CPR / (POLE_PAIRS / 2);

class Motor {
	public:
		enum State {
			STATE_IDLE,
			STATE_ACTIVE,
			STATE_START_CALIBRATE,
			STATE_CALIBRATE
		};

		void setup(TIM_HandleTypeDef *_htim, GPIO_TypeDef *_pin_sleep_port, uint16_t _pin_sleep);
		void enable(void);
		void disable(void);
		void loop(void);
		void calibrate(void);
		void active(void);

		uint16_t adc_readings[3] = {0};
		void convert_current(void);

		State get_state(void) {
			return state;
		}

		bool is_calibrating(void) {
			return state == STATE_START_CALIBRATE || state == STATE_CALIBRATE;
		}
	private:
		State state = STATE_IDLE;

		TIM_HandleTypeDef *htim;
		GPIO_TypeDef *pin_sleep_port;
		uint16_t pin_sleep;

		float current[3] = {0};
		uint16_t pos = 0;

		// calibration (under loop_calibrate) sets this
		// commutation subtracts this offset from the encoder position
		// to align encoder cycle start with electrical cycle start
		uint16_t calibration_offset = 136;

		static float current_from_adc(uint16_t adc) {
			// formula from page 11, figure 2 of the mp6540 datasheet
			// R_ref / 2 because two equal-value resistors are used (see superposition theorem)
			return ((adc - 2048) * 3.3 / 4096 * CS_GAIN) / (CS_RESISTANCE / 2);
		}

		static uint8_t get_sinLUT(int16_t offset) {
			return sinLUT[offset % len_sinLUT];
		}

		uint16_t get_corrected_pos(void);

		void loop_active(void);
		void loop_calibrate(void);
};

#endif /* SRC_APP_MOTOR_HPP_ */
