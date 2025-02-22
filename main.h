#ifndef __MAIN_H__
#define __MAIN_H__

#include "GPIO.h"
#include "RCC.h"
#include "USART.h"
#include "TIM.h"
#include <stdio.h>

volatile uint32_t IC1Val = 0; // CCR value when catching rising edge
volatile uint32_t IC2Val = 0; // CCR value when catching falling edge
volatile uint32_t Freq = 0;		// Input frequency
volatile int Duty = 0;				// Input Duty Cycle
uint32_t old_freq = 0;
uint32_t old_duty = 0;
volatile int stringSent = 0; // Flag to check if the frequency and duty cycle string has been sent
volatile int update = 0; 		 // Flag to indicate if the frequency and duty cycle need to be updated
// Declare global variables and flags
volatile uint32_t IC1Val_ch1 = 0; // CCR value when catching rising edge
volatile uint32_t IC2Val_ch1 = 0; // CCR value when catching falling edge
volatile uint32_t Freq_ch1 = 0;		// Input frequency
volatile uint32_t Duty_ch1 = 0;				// Input Duty Cycle
volatile float old_freq_ch1 = 0;
volatile float old_duty_ch1 = 0;
volatile int stringSent_ch1 = 0; // Flag to check if the frequency and duty cycle string has been sent
volatile int update_ch1 = 0; 		 // Flag to indicate if the frequency and duty cycle need to be updated

volatile uint8_t StringSent = 1;

volatile uint32_t IC1Val_ch2 = 0; // CCR value when catching rising edge
volatile uint32_t IC2Val_ch2 = 0; // CCR value when catching falling edge
volatile uint32_t Freq_ch2 = 0;		// Input frequency
volatile int Duty_ch2 = 0;				// Input Duty Cycle
volatile float old_freq_ch2 = 0;
volatile float old_duty_ch2 = 0;
volatile int stringSent_ch2 = 0; // Flag to check if the frequency and duty cycle string has been sent
volatile int update_ch2 = 0; 		 // Flag to indicate if the frequency and duty cycle need to be updated

//Configure USART1
void USART1_Config(uint32_t BAUD_RATE);
// Initialize PWM output for Timer 1 function
void TIM1_PWMOC(uint32_t Frequency, uint32_t Duty_Cycle);
void TIM4_PWMOC(uint32_t Frequency, uint32_t Duty_Cycle);
// Initialize PWM input capture for Timer 2 function
void TIM2_PWMIC();
void TIM3_PWMIC();
#endif /* __MAIN_H__ */