#include "timer.h"
 	  
//arr£ºarrange of counting
//psc£ºprescale

// TIM3 is initialized
// instead of using it to output PWM any more, it'll be initialized as a 5ms timer dealing with control cycle.
// there's no CCR in interrupt mode
// for 5ms control cycle, f_tim3 = 1/5e-3 = 200Hz = 72MHz/(ARR*psc) -> ARR*psc = 3.6e5
// so let's set arr=ARR-1=9999, psc=36
void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 				// enable the CLK

	TIM_TimeBaseStructure.TIM_Period = arr; 		 				 				// setting up arrange value
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	 				 				// precaling factor
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 				 				// scale it again with this factor
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM counting incrementally
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 		 				// Initialize the timer according to the given structure, this is acctually a drawback of using C, could be simpler in C++
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 	 				 				// Enable interrupt of TIM3

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  		 				// interrupt of TIM3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  	// pre-group of priority 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  				// subpriority 0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 						// enable IRQ
	NVIC_Init(&NVIC_InitStructure);  														// pass the structure to initialize NVIC

	TIM_Cmd(TIM3, ENABLE);  																		// Enable the corresponding pin
							 
}

/*
// initialize interrupt of TIM4
// it's not initialized in the main function, cause all 4 channels are already occupied
void TIM4_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 		// enable the clk of TIM4
	
	TIM_TimeBaseStructure.TIM_Period=arr;										// period length
	TIM_TimeBaseStructure.TIM_Prescaler=psc; 								// prescaling the clk
	TIM_TimeBaseStructure.TIM_ClockDivision=0; 							// no more clock division
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);								// update all setup for TIM4 interrupt
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; 				// also should name the handler after this prefix
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;	// group in NVIC from [0:3] in this case
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;				// subpriority from [0:3] in this case
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM4, ENABLE);																	// enable the device
}
*/

// Interrupt request handler of TIM3
// using 
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) // check if interrupt is issued
		{
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  	 // clean up the flag when interrupt is already taken care of
			//LED1=!LED1;																	 // there's no PE5 pin on my board
			// but something else could be done this way,
			// such as the timer callback for sensors
			calculate_motor();
			pub_to_motor();
		}
}

/*
// handler of interrupt TIM4
// currently not needed, but save it as a placeholder.
// most possibly there will be another interrupt handler of TIMx used for reading sensors
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update)!=RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}
*/

// TIM3 initialized as PWM timer
void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	// enable CLK of TIM3
 
	TIM_TimeBaseStructure.TIM_Period = arr; // setup ARR of counter
	TIM_TimeBaseStructure.TIM_Prescaler =psc; // prescale of CLK
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  // CNT incremental(++)
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); // init the timebase(just clk frequency) of TIM3
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; // PWM mode
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // when to activate the OUTPUT
	//TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // polarity of output, the OUTPUT when ENABLED
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  // initializing TIM3 according to setups in the structure

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  // preload register enabled
 
	TIM_Cmd(TIM3, ENABLE);  //TIM3 enabled
	

}

// initializing PWM output, basically configuring all involved GPIOs
void TIM4_PWM_Init(u16 arr,u16 psc)
{
	// load empty template of initialization
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	// we only use TIM4 for all 4 motors, by setting 4 different CCRx
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	// Initialize TIM4
	TIM_TimeBaseStructure.TIM_Period=arr;	// set up period length(frequency of PWM)
	TIM_TimeBaseStructure.TIM_Prescaler=psc;
	TIM_TimeBaseStructure.TIM_ClockDivision=0;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	// Initialize PWM of 4 channels
	// it counts up, and when CNT < CCRx, then OUT=ENABLED=LOW
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;
	// apply structure to CH1
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	// apply structure to CH2
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	// apply structure to CH3
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
		// apply structure to CH4
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM4, ENABLE);
}

// initialize all the pins for PWM output
void GPIO_PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //enable GPIO ports and CLK
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); // TIM3_CH2->PB5    
	// Output mode setup
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9; //TIM3_CH2 & TIM4_CHx
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);// GPIO initialized
}
