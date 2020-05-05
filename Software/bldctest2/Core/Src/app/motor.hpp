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

const uint16_t len_sinLUT = 360;
//const uint8_t sinLUT[len_sinLUT] = {0, 2, 4, 6, 8, 11, 13, 15, 17, 20, 22, 24, 26, 28, 31, 33, 35, 37, 39, 42, 44, 46, 48, 50, 53, 55, 57, 59, 61, 63, 65, 68, 70, 72, 74, 76, 78, 80, 83, 85, 87, 89, 91, 93, 95, 97, 99, 101, 103, 105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 125, 127, 129, 131, 133, 135, 137, 138, 140, 142, 144, 146, 148, 149, 151, 153, 155, 156, 158, 160, 162, 163, 165, 167, 168, 170, 172, 173, 175, 177, 178, 180, 181, 183, 184, 186, 188, 189, 190, 192, 193, 195, 196, 198, 199, 200, 202, 203, 204, 206, 207, 208, 210, 211, 212, 213, 215, 216, 217, 218, 219, 220, 221, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 232, 233, 234, 235, 236, 237, 238, 238, 239, 240, 241, 241, 242, 243, 243, 244, 245, 245, 246, 246, 247,
//							  247, 248, 248, 249, 249, 250, 250, 251, 251, 251, 252, 252, 252, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 255, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 253, 253, 253, 253, 252, 252, 252, 251, 251, 251, 250, 250, 249, 249, 248, 248, 247, 247, 246, 246, 245, 245, 244, 243, 243, 242, 241, 241, 240, 239, 238, 238, 237, 236, 235, 234, 233, 232, 232, 231, 230, 229, 228, 227, 226, 225, 224, 223, 221, 220, 219, 218, 217, 216, 215, 213, 212, 211, 210, 208, 207, 206, 204, 203, 202, 200, 199, 198, 196, 195, 193, 192, 190, 189, 188, 186, 184, 183, 181, 180, 178, 177, 175, 173, 172, 170, 168, 167, 165, 163, 162, 160, 158, 156, 155, 153, 151, 149, 148, 146, 144, 142, 140, 138, 137, 135, 133, 131, 129, 127, 125, 123, 121, 119, 117, 115, 113, 111, 109, 107, 105, 103, 101, 99, 97, 95, 93, 91, 89, 87, 85, 83, 80, 78, 76, 74, 72, 70, 68, 65, 63, 61, 59, 57, 55, 53, 50, 48, 46, 44, 42, 39, 37, 35, 33, 31, 28, 26, 24, 22, 20, 17, 15, 13, 11, 8, 6, 4, 2};

const uint8_t sinLUT[len_sinLUT] = {127, 129, 131, 133, 135, 138, 140, 142, 144, 146, 149, 151, 153, 155, 157, 159, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182, 184, 186, 188, 190, 192, 194, 196, 198, 199, 201, 203, 205, 206, 208, 210, 211, 213, 215, 216, 218, 219, 221, 222, 224, 225, 227, 228, 229, 231, 232, 233, 234, 235, 236, 238, 239, 240, 241, 242, 243, 243, 244, 245, 246, 247, 247, 248, 249, 249, 250, 250, 251, 251, 252, 252, 252, 253, 253, 253, 253, 253, 253, 253, 254, 253, 253, 253, 253,
		253, 253, 253, 252, 252, 252, 251, 251, 250, 250, 249, 249, 248, 247, 247, 246, 245, 244, 243, 243, 242, 241, 240, 239, 238, 236, 235, 234, 233, 232, 231, 229, 228, 227, 225, 224, 222, 221, 219, 218, 216, 215, 213, 211, 210, 208, 206, 205, 203, 201, 199, 198, 196, 194, 192, 190, 188, 186, 184, 182, 180, 178, 176, 174, 172, 170, 168, 166, 164, 162, 159, 157, 155, 153, 151, 149, 146, 144, 142, 140, 138, 135, 133, 131, 129, 127, 124, 122, 120, 118, 115, 113, 111, 109, 107, 104, 102, 100, 98, 96, 94, 91, 89, 87, 85, 83, 81, 79, 77, 75, 73, 71, 69, 67, 65, 63, 61, 59, 57, 55, 54, 52, 50, 48, 47, 45, 43, 42, 40, 38, 37, 35, 34, 32, 31, 29, 28, 26, 25, 24, 22, 21, 20, 19, 18, 17, 15, 14, 13, 12, 11, 10, 10, 9, 8, 7, 6, 6, 5, 4, 4, 3, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 8, 9, 10, 10, 11, 12, 13, 14, 15, 17, 18, 19, 20, 21, 22, 24, 25, 26, 28, 29, 31, 32, 34, 35, 37, 38, 40, 42, 43, 45, 47, 48, 50, 52, 54, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73, 75, 77, 79, 81, 83, 85, 87, 89, 91, 94, 96, 98, 100, 102, 104, 107, 109, 111, 113, 115, 118, 120, 122, 124};

class Motor {
	public:
		void setup(TIM_HandleTypeDef *_htim, GPIO_TypeDef *_pin_sleep_port, uint16_t _pin_sleep);
		void enable(void);
		void disable(void);
		void loop(void);

		uint16_t adc_readings[3] = {0};
		void convert_current(void);
	private:
		TIM_HandleTypeDef *htim;
		GPIO_TypeDef *pin_sleep_port;
		uint16_t pin_sleep;

		float current[3] = {0};

		static float current_from_adc(uint16_t adc) {
			// formula from page 11, figure 2 of the mp6540 datasheet
			// R_ref / 2 because two equal-value resistors are used (see superposition theorem)
			return ((adc - 2048) * 3.3 / 4096 * CS_GAIN) / (CS_RESISTANCE / 2);
		}

		static uint8_t get_sinLUT(uint16_t offset) {
			return sinLUT[offset % len_sinLUT];
		}
};

#endif /* SRC_APP_MOTOR_HPP_ */
