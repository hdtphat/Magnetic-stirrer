#include "stm32f10x.h"
#include "main.h"
#include "spi1.h"
#include "MAX7219.h"

int main(void){
	Clock_Init();
	GPIO_Init();
__enable_irq();
	TIM4_Init();
	EXTI_Init();
	TIM2_Init();
	TIM3_Init();
	SPI1_Setup();
	SPI1_NSSdisable();
	SPI1_Enable();
	MAX7219_Init();
	
	while(1);
}

void Clock_Init(void)
{
/*
	system clock 72MHz
	APB1 clock PCLK1 = 72/8 = 9MHz
*/
	// Set APB1 prescaler to 1/8 
	RCC->CFGR |= (6<<8);
	// enable GPIOA, GPIOB, AFIO
	RCC->APB2ENR |= (1U<<0) | (1U<<2) | (1U<<3);
	// enable TIM6, TIM7, TIM4, TIM3 and TIM2
	RCC->APB1ENR |= (1U<<4) | (1U<<5) | (1U<<2) | (1U<<1) | (1U<<0);
}

void GPIO_Init(void)
{
/*
	PB6 alternate func ouput -> TIM4 CH1 PWM
	PB4~5 external interupt input -> Encoder CHA and CHB
	PA0~1 external interupt input -> buttons INCREASE, DECREASE
	PA4 general output -> SPI1 CS
	PA5 alternate function output Push Pull -> SPI1 SCLK
	PA6 floating input mode -> SPI1 MISO
	PA7 alternate function output Push Pull -> SPI1 MOSI
*/
	GPIOA->CRL = 0xB4B24444; // PA0~7
	GPIOA->CRH = 0x44444444; // PA8~15
	GPIOB->CRL = 0x4B444444; // PB0~7
	GPIOB->CRH = 0x44444444; // PB8~15
	GPIOA->ODR = 0;
	GPIOB->ODR = 0;
}

void TIM4_Init(void) // TIM4 CH1 PWM output with PB6
{ 
	// enable clock to TIM4
	RCC->APB1ENR |= (1U<<2);
	// Set TIM4 prescaler to 1/18 -> Ftim4 = 1MHz
	TIM4->PSC = 18-1;     
	// Set ARR, CNT, CCR
	TIM4->ARR = 20000;
	// reset counter
	TIM4->CNT = 0;
	// config dutty cycle
	TIM4->CCR1 = 0; // dutty cycle 0%
	// Set PWM1 mode1 to CH1
	TIM4->CCMR1 |= (6U<<4);
	// Enable CH1
	TIM4->CCER |= (1U<<0);
	// Enable counter
	TIM4->CR1 |= (1U<<0);
}

void EXTI_Init(void)
{
	// configure EXTI (0: portA, 1: portB)
	AFIO->EXTICR[0] = 0x0000;	// EXTI1: PA0~1 -> buttons INCREASE, DECREASE, TIMING
	AFIO->EXTICR[1] = 0x0011;	// EXTI2: PB4~5 -> encoder CHA, CHB
	// disable  EXTI mask
	EXTI->IMR |= (1U<<0) | (1U<<1) | (1U<<4) | (1U<<5) | (1U<<3);		
	// enable Rising Edge Trigger
	EXTI->RTSR |= (1U<<0) | (1U<<1) | (1U<<4) | (1U<<5) | (1U<<3); 				
	// disable Falling Edge Trigger
	EXTI->FTSR &= ~((1U<<0) | (1U<<1) | (1U<<4) | (1U<<5) | (1U<<3));
	// enable interupt
	NVIC_SetPriority (EXTI4_IRQn, EXTI4_piority);	
	NVIC_SetPriority (EXTI9_5_IRQn, EXTI9_5_piority); 
	NVIC_SetPriority (EXTI0_IRQn, EXTI0_piority);
	NVIC_SetPriority (EXTI1_IRQn, EXTI1_piority);
	NVIC_SetPriority (EXTI3_IRQn, EXTI3_piority);
	NVIC_EnableIRQ (EXTI4_IRQn); 
	NVIC_EnableIRQ (EXTI9_5_IRQn);
	NVIC_EnableIRQ (EXTI0_IRQn);
	NVIC_EnableIRQ (EXTI1_IRQn);
	NVIC_EnableIRQ (EXTI3_IRQn);
}

void EXTI4_IRQHandler(void) // PB4 Encoder CHA
{ 
	if(EXTI->PR & (1U<<4)){
		EXTI->PR |= (1U<<4);
		Encoder_Count++;
	}
}

void EXTI9_5_IRQHandler(void) // PB5 Encoder CHB
{ 
	if(EXTI->PR & (1U<<5)){
		EXTI->PR |= (1U<<5);
		Encoder_Count++;
	}
}

void EXTI0_IRQHandler(void) // PA0 Button DECREASE
{
	if(EXTI->PR & (1U<<0)){
		EXTI->PR |= (1U<<0);
		switch (MODE){
			case 0x00: // decrease Speed
				PWM_DuttyCycle-=Step_Value;
				if(PWM_DuttyCycle >= 100) PWM_DuttyCycle = 0; // uint never get negaive
				TIM4->CCR1 = (uint16_t)((PWM_DuttyCycle*20000)/100);
				break;
			case 0xFF: // decrease Time
				if(Time-10 > 0) Time -= 10;
				break;
		}
	}
}

void EXTI1_IRQHandler(void) // PA1 Button INCREASE
{
	if(EXTI->PR & (1U<<1)){
		EXTI->PR |= (1U<<1);
		switch (MODE){
			case 0x00: // increase Speed
				PWM_DuttyCycle+=Step_Value;
				if(PWM_DuttyCycle >= 100) PWM_DuttyCycle = 100;
				TIM4->CCR1 = (uint16_t)((PWM_DuttyCycle*20000)/100);
				break;
			case 0xFF:
				if(Time+10 < 9999) Time += 10;
				break;
		}
	}
}

void EXTI3_IRQHandler(void) // PA3 Button MODE
{
	if(EXTI->PR & (1U<<3)){
		EXTI->PR |= (1U<<3);
		MODE = ~MODE;
		switch (MODE){
			case 0x00: // switch to "Change Speed"
				TIM3->CR1 |= (1<<0);
				break;
			case 0xFF: // switch to "Change Time"
				TIM3->CR1 &= ~(1UL<<0);
				break;
		}
	}
}

void TIM2_Init(void)
{ 
	// enable clock to TIM2
	RCC->APB1ENR |= (1U<<0);
	// set TIM2 prescaler to 1/36 -> Ftim2 = 0.5MHz
	TIM2->PSC = 36-1;
	// config max count value -> interupt every 100ms
	TIM2->ARR = 50000;
	// reset counter
	TIM2->CNT = 0;
	// enable timer interrupt
	TIM2->DIER |= (1U<<0);
	NVIC_SetPriority (TIM2_IRQn, TIM2_piority);
	NVIC_EnableIRQ (TIM2_IRQn);
	// enable counter 
	TIM2->CR1 |= (1<<0);	
}

void TIM2_IRQHandler(void) // refresh motor speed every 100ms
{ 
	TIM2->SR = 0;
	Count++;
	if(Count >= 10) Count = 0;
	switch (MODE){
		case 0x00: // display Speed
			Motor_Speed = (uint32_t)(((60000/Sampling_Time)*Encoder_Count)/Encoder_1_Round);
			if(Count < 5) MAX7219_displayINT(Time*10000+Motor_Speed);
			else MAX7219_displayINT(Motor_Speed);
			Encoder_Count = 0;
			break;
		case 0xFF: // blinking Time
			if(Count < 5) MAX7219_displayINT(Time);
			else MAX7219_displayINT(1);
			break;
	}
}

void TIM3_Init(void)
{ 
	// enable clock to TIM3
	RCC->APB1ENR |= (1U<<1);
	// set TIM3 prescaler to 1/360 -> Ftim2 = 0.05MHz
	TIM3->PSC = 360-1;
	// config max count value -> interupt every 1s
	TIM3->ARR = 50000;
	// reset counter
	TIM3->CNT = 0;
	// enable timer interrupt
	TIM3->DIER |= (1U<<0);
	NVIC_SetPriority (TIM3_IRQn, TIM3_piority);
	NVIC_EnableIRQ (TIM3_IRQn);
	// enable counter 
	TIM3->CR1 |= (1<<0);	
}

void TIM3_IRQHandler(void) // decrease Time every 1s
{
	TIM3->SR = 0;
	Previous_Time = Time;
	if(Time > 0) Time--;
	if(Previous_Time==1 && Time==0){
		PWM_DuttyCycle = 0;
		TIM4->CCR1 = 0;
	}
}	
