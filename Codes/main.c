/*
 ============================================================================
 Name        : Mini-Project 1 on "AVR Micro-controller Interfacing 1"
 Author      : Mohanad Saeed
 Version     : 30/01/2021
 Description : Use Timer1 CTC Mode to count the time for the stop watch.
 			   INT0(Falling edge using the internal pull up)
 			   --> reset the stop watch
 			   INT1 (raising edge using the external pull down)
 			   --> pause the stop watch
 		   	   INT2(Falling edge using the internal pull up)
 		   	   --> resume the stop watch
 ============================================================================
 */

#define FCPU_1000000
#include "External_Interrupt/external_interrupts.h"
#include "Timer1/timer1.h"
/*time = {sec1 , sec2 , min1 , min2 , hour1 , hour2}*/
unsigned char time[6]={0};
unsigned char i=0 , flag=0;

void reset(void)
{
	// Reset the stop watch
	for (i=0;i<6;i++)
	{
		time[i]=0;
	}
	i=0;
}

void stop(void)
{
	// To Stop Time 1 Clear Clock bits
	TIMER1_stopCount();
}

void resume(void)
{
	// To resume stop watch resume the clock
	TIMER1_startCount(TIMER1_F_CPU_64);
}

void timer1CallBack(void)
{
	// Trigger once each second by timer 1
	flag=1;
}

int main(void)
{
	// Void Setup
	Ei_ConfigType ei0;
	Ei_ConfigType ei1;
	Ei_ConfigType ei2;
	Timer1_ConfigType timer1;

	ei0.interruptevent=INT_FALLING_EDGE;
	ei0.pullup=INT_ACTIVATE;
	ei1.interruptevent=INT_RISING_EDGE;
	ei1.pullup=INT_DEACTIVATE;
	ei2.interruptevent=INT_FALLING_EDGE;
	ei2.pullup=INT_ACTIVATE;

	timer1.clock=TIMER1_F_CPU_64;
	timer1.mode=TIMER1_CTC;
	timer1.initialValue=0;
	timer1.oc1AMode=OC1_A_DISCONNECT;
	timer1.oc1BMode=OC1_B_DISCONNECT;
	timer1.tick=15625;


	INT0_init(&ei0);
	INT0_setCallBack(reset);
	INT1_init(&ei1);
	INT1_setCallBack(stop);
	INT2_init(&ei2);
	INT2_setCallBack(resume);
	TIMER1_init(&timer1);
	TIMER1_setCallBack(timer1CallBack,timer1.mode);

	SREG |= (1<<7);			// Enable I-bit
	DDRC |= 0x0F;			// Configure first 4 pins in PORTC as output
	PORTC &= 0xF0;			// Initial value for the stop watch 0
	DDRA |= 0x3F;			/* Configure first 6 pins in PORTA as output to
	 	 	 	 	 	 	 * select which 7-SEGMENT to be enabled */
	PORTA &= 0xC0;			// Initial value to disable all 7-SEGMENTs

	// Void Loop
	while(1)
	{
		PORTA = (PORTA & 0xC0) | (1<<i);
		PORTC = (PORTC & 0xF0) | (time[i] & 0x0F);
		_delay_ms(3);
		if (PORTA & (1<<5))
		{
			i=0;
			PORTA = (PORTA & 0xC0) | (1<<i);
			PORTC = (PORTC & 0xF0) | (time[i] & 0x0F);
			_delay_ms(3);
		}
		i++;

		if (flag==1)
		{
			time[0]++;
			if (time[0]==10)
			{
				time[1]++;
				time[0]=0;
			}
			if (time[1]==6)
			{
				time[2]++;
				time[1]=0;
				time[0]=0;
			}
			if (time[2]==10)
			{
				time[3]++;
				time[2]=0;
				time[1]=0;
				time[0]=0;
			}
			if (time[3]==6)
			{
				time[4]++;
				time[3]=0;
				time[2]=0;
				time[1]=0;
				time[0]=0;
			}
			if (time[4]==10)
			{
				time[5]++;
				time[4]=0;
				time[3]=0;
				time[2]=0;
				time[1]=0;
				time[0]=0;
			}
			if (time[5]==10)
			{
				time[5]=0;
				time[4]=0;
				time[3]=0;
				time[2]=0;
				time[1]=0;
				time[0]=0;
			}
			flag=0;
		}
	}
	return 0;
}
