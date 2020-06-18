#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "led.h"
#include "usart.h"
#include "controller.h"

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
void TIM4_PWM_Init(u16 arr,u16 psc);
void GPIO_PWM_Init(void);
#endif
