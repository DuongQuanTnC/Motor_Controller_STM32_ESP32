#include "main.h"
char signal = 0;
volatile int uart_request = 0; // 0: Không có yêu c?u, 1: Yêu c?u g?i d? li?u
volatile uint8_t updateFlag = 0; 
volatile uint8_t sendFreqDutyOnce = 0; // C? d? ch? g?i Freq và Duty m?t l?n


void USART1_IRQHandler(void) {
    if (USART1->SR & USART_SR_RXNE) { // Ki?m tra n?u có d? li?u nh?n
				NVIC_DisableIRQ(TIM2_IRQn);
				NVIC_DisableIRQ(TIM3_IRQn);			// T?m th?i vô hi?u hóa ng?t TIM2
        char receivedByte = USART1->DR; // Ð?c d? li?u
        if (receivedByte == '1') { // L?nh "Open" t? Qt
            sendFreqDutyOnce = 1; // B?t c? g?i Freq và Duty
						stringSent_ch1 = 1;
						stringSent_ch2 = 1;
        }
    }
}

typedef struct {
    volatile float IC1Val;
    volatile float IC2Val;
    volatile float Freq;
    volatile uint32_t Duty;
    uint8_t UpdateFlag;
} TimerChannel;

volatile TimerChannel Timer2Channel = {0};
volatile TimerChannel Timer3Channel = {0};

void TIM2_IRQHandler(void) {
		//volatile TimerChannel Timer2Channel = {0};
		static uint16_t ovf = 0;
		static uint16_t OverflowCount = 0;
    if (TIM2->SR & TIM_SR_CC1IF) {
        TIM2->SR &= ~TIM_SR_CC1IF;
        Timer2Channel.IC1Val = TIM2->CCR1 + OverflowCount * 0xFFFF;
        ovf++;
				if(stringSent_ch1) {
					USART_send(USART1, '1');
				}
        
    } else if (TIM2->SR & TIM_SR_CC2IF) {
        TIM2->SR &= ~TIM_SR_CC2IF;
        Timer2Channel.IC2Val = TIM2->CCR2 + OverflowCount * 0xFFFF;
        ovf++;
        //USART_send(USART1, '1');
    }

    if (ovf == 2) {
        OverflowCount = 0;
        ovf = 0;
    }

    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;
        OverflowCount++;
    }

    if (Timer2Channel.IC1Val != 0) {
        Timer2Channel.Duty = (Timer2Channel.IC2Val - OverflowCount * 0xFFFF) * 100 /
                        (Timer2Channel.IC1Val - OverflowCount * 0xFFFF);
        Timer2Channel.Freq = (float)(RCC_GetAbp1Clk() / ((Timer2Channel.IC1Val - OverflowCount * 0xFFFF) * (TIM2->PSC + 1)));
        Timer2Channel.IC1Val = 0;
        Timer2Channel.IC2Val = 0;
				
				if (old_freq_ch1 != Timer2Channel.Freq || old_duty_ch1 != (uint32_t)Timer2Channel.Duty) {
            update_ch1 = 1; // Set update flag
            stringSent_ch1 = 0; // Reset stringSent flag
        }
				if (update_ch1 == 1 && (old_freq_ch1 == Timer2Channel.Freq || old_duty_ch1 == (uint32_t)Timer2Channel.Duty))
				{
				char buffer[30];
				old_freq_ch1 = Timer2Channel.Freq; // Update old frequency
				old_duty_ch1 = Timer2Channel.Duty; // Update old duty cycle
				sprintf(buffer, "NC1F:%.5fD:%u\n", Timer2Channel.Freq, Timer2Channel.Duty);
				USART_str(USART1, (unsigned char *)buffer); // G?i chu?i qua UART
				update_ch1 = 0;
				stringSent_ch1 = 1;
				}
    }
}

void TIM3_IRQHandler(void) {
    //volatile TimerChannel Timer3Channel = {0};
		static uint16_t ovf2 = 0;
		static uint16_t OverflowCount2 = 0;
    if (TIM3->SR & TIM_SR_CC1IF) {
        TIM3->SR &= ~TIM_SR_CC1IF;
        Timer3Channel.IC1Val = TIM3->CCR1 + OverflowCount2 * 0xFFFF;
        ovf2++;
        if(stringSent_ch2) {
					USART_send(USART1, '2');
				}
        
    } else if (TIM3->SR & TIM_SR_CC2IF) {
        TIM3->SR &= ~TIM_SR_CC2IF;
        Timer3Channel.IC2Val = TIM3->CCR2 + OverflowCount2 * 0xFFFF;
        ovf2++;
        //USART_send(USART1, '2');
    }

    if (ovf2 == 2) {
        OverflowCount2 = 0;
        ovf2 = 0;
    }

    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF;
        OverflowCount2++;
    }

    if (Timer3Channel.IC1Val != 0) {
        Timer3Channel.Duty = (Timer3Channel.IC2Val - OverflowCount2 * 0xFFFF) * 100 /
                        (Timer3Channel.IC1Val - OverflowCount2 * 0xFFFF);
        Timer3Channel.Freq = (float)(RCC_GetAbp1Clk() / ((Timer3Channel.IC1Val - OverflowCount2 * 0xFFFF) * (TIM3->PSC + 1)));
        Timer3Channel.IC1Val = 0;
        Timer3Channel.IC2Val = 0;
			if (old_freq_ch2 != Timer3Channel.Freq || old_duty_ch2 != (uint32_t)Timer3Channel.Duty) {
            update_ch2 = 1; // Set update flag
            stringSent_ch2 = 0; // Reset stringSent flag
        }
			if (update_ch2 == 1 &&(old_freq_ch2 == Timer3Channel.Freq || old_duty_ch2 == (uint32_t)Timer3Channel.Duty))
			{
				char buffer[30];
				old_freq_ch2 = Timer3Channel.Freq; // Update old frequency
				old_duty_ch2 = Timer3Channel.Duty; // Update old duty cycle
				//update_ch2 = 0; // Reset update flag
				sprintf(buffer, "NC2F:%.5fD:%u\n", Timer3Channel.Freq, Timer3Channel.Duty); // Format frequency and duty cycle string
				USART_str(USART1, (unsigned char *)buffer);
				update_ch2 = 0;
				stringSent_ch2 = 1;
			}
    }
}

void clearCharArray(char* str) {
    for(int i = 0; str[i] != '\0'; i++) {
        str[i] = '\0';
    }
}
int main() {
    // Configure GPIO and timers  
    TIM1_PWMOC(555, 80);
		TIM4_PWMOC(16, 60);
		USART1_Config(115200);
		TIM2_PWMIC();
		TIM3_PWMIC();
		//char buffer[50];

    while (1) {
						
					if (sendFreqDutyOnce) {
						char buffer[30];
						old_freq_ch1 = Timer2Channel.Freq; // Update old frequency
						old_duty_ch1 = Timer2Channel.Duty; // Update old duty cycle
						sprintf(buffer, "NC1F:%.5fD:%u\n", Timer2Channel.Freq, Timer2Channel.Duty);
						USART_str(USART1, (unsigned char *)buffer); // G?i chu?i qua UART
						
						
						old_freq_ch2 = Timer3Channel.Freq; // Update old frequency
						old_duty_ch2 = Timer3Channel.Duty; // Update old duty cycle
						update_ch2 = 0; // Reset update flag
						update_ch1 = 0; // Reset update flag
						sprintf(buffer, "NC2F:%.5fD:%u\n", Timer3Channel.Freq, Timer3Channel.Duty); // Format frequency and duty cycle string
						USART_str(USART1, (unsigned char *)buffer);
						
						sendFreqDutyOnce = 0; // T?t c? sau khi g?i xong
						NVIC_EnableIRQ(TIM2_IRQn); 
						NVIC_EnableIRQ(TIM3_IRQn);// Kích ho?t l?i ng?t TIM2
					}
				}
    return 0;
}

//Configure USART1
void USART1_Config(uint32_t BAUD_RATE)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	// Initialize UART1
  USART_Init(USART1, BAUD_RATE);
	// Configure GPIO for UART
  GPIO_SetMode(GPIOA, 9, GPIO_ALT_MODE_50M_PP);
  GPIO_SetMode(GPIOA, 10, GPIO_INPUT_MODE_FLOAT);
	// UART1 Enable
  USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
	USART1->CR1 |= USART_CR1_RXNEIE; // B?t ng?t nh?n d? li?u
	NVIC_SetPriority(USART1_IRQn, 0); // Ð?t uu tiên ng?t
	NVIC_EnableIRQ(USART1_IRQn);      // Kích ho?t ng?t USART1
}

// Configure GPIO for Timer 1 and UART
void TIM1_PinConfig() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIO_SetMode(GPIOA, 8, GPIO_ALT_MODE_50M_PP);
}

// Initialize PWM output for Timer 1 function
void TIM1_PWMOC(uint32_t Frequency, uint32_t Duty_Cycle) {
    TIM1_PinConfig();
    TIM_OC1_PWMM1_Init(TIM1, Frequency, Duty_Cycle);
}

void TIM4_PinConfig() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIO_SetMode(GPIOB, 6, GPIO_ALT_MODE_50M_PP);
}

// Initialize PWM output for Timer 1 function
void TIM4_PWMOC(uint32_t Frequency, uint32_t Duty_Cycle) {
    TIM4_PinConfig();
    TIM_OC1_PWMM1_Init(TIM4, Frequency, Duty_Cycle);
}

// Configure GPIO for Timer 2
void TIM2_PinConfig() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIO_SetMode(GPIOA, 0, GPIO_INPUT_MODE_FLOAT);
}

void TIM3_PinConfig() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIO_SetMode(GPIOA, 6, GPIO_INPUT_MODE_FLOAT);
}

// Initialize PWM input capture for Timer 2 function
void TIM2_PWMIC() {
    TIM2_PinConfig();
    TIM_PWMICMInit(TIM2);
    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM3_PWMIC() {
    TIM3_PinConfig();
    TIM_PWMICMInit(TIM3);
    NVIC_SetPriority(TIM3_IRQn, 1);
    NVIC_EnableIRQ(TIM3_IRQn);
}