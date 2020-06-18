#include "sys.h"

u8 swt=0;

int main(void)
{		
	//u16 t; 	
	//u8 i;
	//u16 led0pwmval=0;
	//u8 dir=1;	
	u16 times=0;
	u16 len;	
	delay_init();		  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // NVIC grouping mode, 2:2, 2 pregroups and 2 subpriorities
	uart_init(115200);	 		// baudrate = 115200
	IIC_Init();
	MPU6050_initialize();
	LED_Init();
	TIM3_Int_Init(9999,36);	// init interrupt of TIM3 with f=200Hz, which supports control cycle of 5ms
	TIM4_PWM_Init(7999,18);	// init PWM with frequency = 72000000 / 144000 = 500Hz
													// 999 means including 1000 clks in a cycle of PWM
	GPIO_PWM_Init();
	Gyro_offsets_Init();
	while(1)
	{
		// read command
		if(USART_RX_STA&0x8000)
		{
			len=USART_RX_STA&0x3fff;									// read the length of msg
			printf("Command received.\n");
			/*
			for(i=0;i<len;i++)
			{
				printf("%d",USART_RX_BUF[i]);
			}
			printf("\r\n");*/
			if(check_eq(len, USART_RX_BUF,CMD_START))				// if CMD==START
			{
				printf("start received");
				swt=1;
			}
			else if(check_eq(len, USART_RX_BUF,CMD_STOP))		// if CMD==STOP
			{
				printf("stop received");
				//led0pwmval=0;
				/*
				for(i=0;i<4;i++)
				{
					pwmval[i]=3999;
				}*/
				//dir=1;
				swt=0;
			}
			else if(read_thrust(len,USART_RX_BUF))
			{
				printf("Thrust changed to: %f",thrust_effort);
			}
			/*
			else if(check_eq(len, USART_RX_BUF,CMD_INIT))
			{
				printf("initialization...");
				motor_init();
			}*/
			printf("\r\n");
			/*
			// sending data from STM32
			for(t=0;t<len;t++)
			{
				USART_SendData(USART1, USART_RX_BUF[t]);// send the msg to USART1
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);	// wait for transfering
			}
			printf("\r\n\r\n");
			*/
			USART_RX_STA=0;
		}else
		{// if there's no interruption on USART detected
		 // 1) just wait for CMD to come in
		 // 2) or activate motors accordingly
			times++;
			if(swt)
			{
				// test output, remove after there's a controller for velocities
				/*
				if(dir)
				{
					for(i=0;i<4;i++)
					{
						pwmval[i]++;
					}
				}
				else
				{
					for(i=0;i<4;i++)
					{
						pwmval[i]--;
					}
				}*/
				if(times%100==1)
				{
					u8 i;
					for(i=0;i<4;i++)
					{
						//printf("PWM value is: %d \n",pwmval[1]);
						printf("PWM%d: %d \n",i,pwmval[i]);
					}
					printf("Roll: %f Pitch: %f Yaw: %f \n",Roll,Pitch,Yaw);
					printf("roll_effort: %f pitch_effort: %f yaw_effort: %f\n",roll_effort,pitch_effort,yaw_effort);
				}
			}
			else
			{
				if(times%300==1)printf("Waiting for command...\n");
			}
			if(times%300==1) LED=!LED;
			//if(pwmval[0]>4149)dir=0;
			//if(pwmval[0]==3999)dir=1;		
			//pub_to_motor();
			delay_ms(10);
		}
	}	 
}
