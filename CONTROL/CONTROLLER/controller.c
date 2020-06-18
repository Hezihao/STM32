#include "controller.h"

u16 pwmval[4]={3999,3999,3999,3999};
float Motor[4];
float old_pwm[4];
float roll_effort,pitch_effort,yaw_effort,thrust_effort,additional_thrust;
float sign[12]={-1,1,1,1,-1,1,1,1,-1,-1,-1,-1};
// original Kp: 5.0, 4.0, 2.0
float Kp[3]={15.0, 12.0, 10.0};
float C_Kd = 200;
float old_gyro[3]={0,0,0};

// the PD controller or motors
void calculate_motor(void)
{
	u8 i;
	//u16 scale_roll,scale_pitch,scale_yaw;
	// if CMD=stop was sent
	if(!swt)
	{
		for(i=0;i<4;i++)
		{
			pwmval[i]=3999;
			old_pwm[i]=pwmval[i];
		}
		// test
		thrust_effort=3999;
	}
	else
	{
		// first read data from Gyroscope
		Read_DMP();
		// do the calculation here, it's irrelevant with where to get the input of controller
		roll_effort  = Kp[0] * Roll + Kp[0] * (Roll - old_gyro[0]);
		pitch_effort = Kp[1] * Pitch+ Kp[1] * (Pitch- old_gyro[1]);
		yaw_effort	 = Kp[2] * Yaw  + Kp[2] * (Yaw	- old_gyro[2]);
		thrust_effort= PWM_PEND + additional_thrust;
		//scale_roll = roll_effort/(roll_effort+pitch_effort);
		//scale_pitch= 1-scale_roll;
		// recale 
		//roll_effort  = roll_effort * scale_roll;
		//pitch_effort = pitch_effort * scale_pitch;
		Motor[0]= sign[0] * roll_effort + sign[1] * pitch_effort + thrust_effort;
		Motor[1]= sign[3] * roll_effort + sign[4] * pitch_effort + thrust_effort;
		Motor[2]= sign[6] * roll_effort + sign[7] * pitch_effort + thrust_effort;
		Motor[3]= sign[9] * roll_effort + sign[10]* pitch_effort + thrust_effort;
		check_limit();
		// store history values of gyro
		old_gyro[0]=Roll;
		old_gyro[1]=Pitch;
		old_gyro[2]=Yaw;
	}
}

// limit check before output to motors
void check_limit(void)
{
	u8 i;
	for(i=0;i<4;i++)
	{
		Motor[i] = Motor[i] + sign[3*i+2] * yaw_effort;
		if(Motor[i]>7500) Motor[i]=7500;
		pwmval[i] = (int)Motor[i];
	}
}

// exponential filter
void exp_filter(void)
{
	u8 i;
	for(i=0;i<4;i++)
	{
		pwmval[i]=old_pwm[i] * 0.6 + pwmval[i]*0.4;
		old_pwm[i]=pwmval[i];
	}
}

// where the PWM values written to motors
void pub_to_motor(void)
{
	exp_filter();
	TIM_SetCompare1(TIM4,pwmval[0]);			// motor1
	TIM_SetCompare2(TIM4,pwmval[1]);			// motor2
	TIM_SetCompare3(TIM4,pwmval[2]);			// motor3
	TIM_SetCompare4(TIM4,pwmval[3]);			// motor4
	// only for testing basic thrust
	//if(!swt)thrust_effort=0;
	/*
	TIM_SetCompare1(TIM4,thrust_effort);			// motor1
	TIM_SetCompare2(TIM4,thrust_effort);			// motor2
	TIM_SetCompare3(TIM4,thrust_effort);			// motor3
	TIM_SetCompare4(TIM4,thrust_effort);			// motor4
	*/
}
