#ifndef MAIN_H
#define MAIN_H
#include <stdint.h>

#define TIM2_piority 0 // define motor speed evey 100ms
#define EXTI9_5_piority 1 // encoder CHB
#define EXTI4_piority 2 // encoder CHA
#define EXTI0_piority 3 // button DECREASE
#define EXTI1_piority 4 // button INCREASE
#define EXTI3_piority 5 // button MODE
#define TIM3_piority 6 // decrease Time every 1s

#define Encoder_1_Round 198 // number of encoder count per round, x2 mode
#define Sampling_Time 100 // define motor speed every 100ms
#define Step_Value 2.5 // button increase (decrease) PWM duty cycle by 2.5%

double PWM_DuttyCycle = 0;
uint32_t Encoder_Count = 0;
uint32_t Motor_Speed = 0;
uint8_t MODE = 0;
uint8_t Count = 0;
uint32_t Time = 0, Previous_Time = 0;

void Clock_Init(void);
void GPIO_Init(void);
void TIM4_Init(void);
void EXTI_Init(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI3_IRQHandler(void);
void TIM2_Init(void);
void TIM2_IRQHandler(void);
void TIM3_Init(void);
void TIM3_IRQHandler(void);

#endif
