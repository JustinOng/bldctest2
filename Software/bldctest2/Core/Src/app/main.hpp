/*
 * main.hpp
 *
 *  Created on: May 1, 2020
 *      Author: Justin
 */

#ifndef SRC_APP_MAIN_HPP_
#define SRC_APP_MAIN_HPP_

#include "main.h"

#ifdef __cplusplus
	extern "C" {
#endif
	void main_cpp(void);

#ifdef __cplusplus
	}
#endif

#ifdef __cplusplus
	#include "motor.hpp"
	#include "hardware.h"

	extern Motor motor;
#endif

#endif /* SRC_APP_MAIN_HPP_ */
