
#include "hw_config.h"
#include "ANSTAT.h"
#include "variables.h"
#include "AnalisRS.h"
#include "stm32f3xx_it.h"
#include <math.h>



void Anstat100uS(void)
{
/*
	if (MState.RUNFLAG==RF_MAN_PLUS)
	{
	    On_PB2_EC1; //servo on
	    PulseCounter+=3; //~10% speed
	    pRgMx->PulsesSendAcc+=3; //virtual current pos
		if ((TIM16->CR1&TIM_CR1_CEN)==0) //if counter is Stopped reStart
		{
			_Up_Pulses();
		}
	    pRgMx->rState&=~rSta_POSWORK; //stop regulator
	}
	*/
exii_int:;

  AnalisRS();
}
//===========================================================================================//
