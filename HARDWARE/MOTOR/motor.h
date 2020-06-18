#ifndef _MOTOR_H
#define _MOTOR_H
#include <sys.h>
#include <delay.h>
#include "controller.h"
// initialization of motor control
extern u16 MOTOR_MAX;
extern u16 MOTOR_MIN;

void motor_init(void);
#endif
