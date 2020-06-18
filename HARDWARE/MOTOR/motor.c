#include "motor.h"

void set_rcc(u16 delay)
{
	u8 i;
	TIM_SetCompare1(TIM4,pwmval[0]);			// motor1
	TIM_SetCompare2(TIM4,pwmval[1]);			// motor2
	TIM_SetCompare3(TIM4,pwmval[2]);			// motor3
	TIM_SetCompare4(TIM4,pwmval[3]);			// motor4
	for(i=0;i<(delay/500);i++)
	{
		delay_ms(500);
	}
}

void motor_init(void)
{
	u8 i;
	// set throtle to full for 6 seconds
	for(i=0;i<4;i++)
	{
		pwmval[i]=7999;
	}
	set_rcc(6000);
	// set throtle to mid for 6 seconds
	for(i=0;i<4;i++)
	{
		pwmval[i]=7999;
	}
	set_rcc(6000);
}
