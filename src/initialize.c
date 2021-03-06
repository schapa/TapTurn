/*
 * gpio.cpp
 *
 *  Created on: Dec 10, 2015
 *      Author: shapa
 */

#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_exti.h"

#include "api.h"


void initGpio(void) {

	GPIO_InitTypeDef initStructure;
	initStructure.GPIO_Mode = GPIO_Mode_IN;
	initStructure.GPIO_OType = GPIO_OType_OD;
	initStructure.GPIO_Pin = LEFT_INPUT | RIGHT_INPUT | HAZARD_INPUT | CONFIG_INPUT;
	initStructure.GPIO_PuPd = GPIO_PuPd_UP;
	initStructure.GPIO_Speed = GPIO_Speed_Level_1;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_Init(PORT, &initStructure);

	initStructure.GPIO_Pin = LEFT_OUTPUT | RIGHT_OUTPUT | HAZARD_OUTPUT;
	initStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(PORT, &initStructure);
}

void initInterrupts(void) {
	EXTI_InitTypeDef initStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_ClocksTypeDef RCC_ClockFreq;

	RCC_GetClocksFreq(&RCC_ClockFreq);

	initStructure.EXTI_Line = LEFT_INTERRUPT | RIGHT_INTERRUPT | HAZARD_INTERRUPT | CONFIG_INTERRUPT;
	initStructure.EXTI_LineCmd = ENABLE;
	initStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	initStructure.EXTI_Trigger = EXTI_Trigger_Falling;

	EXTI_Init(&initStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource10);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource5);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_EnableIRQ(EXTI2_3_IRQn);
	NVIC_EnableIRQ(EXTI4_15_IRQn);

	SysTick_Config(RCC_ClockFreq.HCLK_Frequency / TICKS_PER_SECOND);
}

void reconfigureInterruptsTrigger(bool isConfigure) {
	EXTI_InitTypeDef initStructure;

	initStructure.EXTI_Line = LEFT_INTERRUPT | RIGHT_INTERRUPT | HAZARD_INTERRUPT;
	initStructure.EXTI_LineCmd = ENABLE;
	initStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	initStructure.EXTI_Trigger = isConfigure ? EXTI_Trigger_Rising_Falling : EXTI_Trigger_Falling;

	EXTI_Init(&initStructure);
}

void setLeftOutputState(FunctionalState state) {
	BitAction val = (state == DISABLE) ? Bit_RESET : Bit_SET;
	GPIO_WriteBit(PORT, RIGHT_OUTPUT, Bit_RESET);
	GPIO_WriteBit(PORT, LEFT_OUTPUT, val);
}

void setRightOutputState(FunctionalState state) {
	BitAction val = (state == DISABLE) ? Bit_RESET : Bit_SET;
	GPIO_WriteBit(PORT, LEFT_OUTPUT, Bit_RESET);
	GPIO_WriteBit(PORT, RIGHT_OUTPUT, val);
}

void setHazardOutputState(FunctionalState state) {
	BitAction val = (state == DISABLE) ? Bit_RESET : Bit_SET;
	GPIO_WriteBit(PORT, HAZARD_OUTPUT, val);
}

FunctionalState getLeftInputState(void) {
	GPIO_ReadInputDataBit(PORT,LEFT_INPUT) ? DISABLE : ENABLE;
}

FunctionalState getRightInputState(void) {
	GPIO_ReadInputDataBit(PORT,RIGHT_INPUT) ? DISABLE : ENABLE;
}

FunctionalState getHazardInputState(void) {
	GPIO_ReadInputDataBit(PORT,HAZARD_INPUT) ? DISABLE : ENABLE;
}

