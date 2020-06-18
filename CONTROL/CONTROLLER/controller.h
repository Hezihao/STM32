#ifndef _CONTROLL_H
#define _CONTROLL_H
#include "sys.h"
#include "MPU6050.h"
#include "filter.h"

#define Pi 3.14159
#define PWM_MAX  7700
#define PWM_PEND 4100	// to be updated due to test, a proper value for pending state is expected
#define PWM_MIN	 4000

extern u8 swt;
extern u16 pwmval[4];
extern float Motor[4];
extern float old_pwm[4];
extern float roll_effort,pitch_effort,yaw_effort,thrust_effort,additional_thrust;
extern float sign[12];
extern float Kp[3];
extern float C_Kd;			// Kd = Kp/C according to tau=5e-03s
extern float old_gyro[3];

void calculate_motor(void);
void check_limit(void);
void exp_filter(void);
void pub_to_motor(void);
u8 commanded_stop(void);
#endif
