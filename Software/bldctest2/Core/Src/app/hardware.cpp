/*
 * hardware.cpp
 *
 *  Created on: May 2, 2020
 *      Author: Justin
 */

#include "hardware.h"

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim1) {
		if (__HAL_TIM_IS_TIM_COUNTING_DOWN(htim)) return;
		motor.loop();
	}
}

void UART_DMA_Complete(DMA_HandleTypeDef *hdma) {
	huart1.Instance->CR3 &= ~USART_CR3_DMAT;
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
	SPI1_CS_GPIO_Port->ODR |= SPI1_CS_Pin;
    __HAL_SPI_DISABLE(&hspi1);
}

void hardware_init(void) {
	HAL_DMA_RegisterCallback(&hdma_usart1_tx, HAL_DMA_XFER_CPLT_CB_ID, &UART_DMA_Complete);
}
