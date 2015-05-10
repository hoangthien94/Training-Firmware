//*****************************************************************************
//
// Application Function
//
//*****************************************************************************

#include "library.h"

//*****************************************************************************
// Store temporary state
//*****************************************************************************
//volatile uint32_t ui32tempState;
//volatile sState g_sState;
//bool flagRun;

//*****************************************************************************
//
//	LEDs indication for each STATE:
//	STATE		LED1			LED2
//-----------------------------------------------------------------------------
//	Soft-start	Fast Blinky		Off
//	Idle		Slow Blinky		Off
//	Run			Always On		PWM Control
//	Error		Fast Blinky		Fast Blinky
//
//*****************************************************************************
void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	switch(g_sState.ui8State)
	{
	case STATE_SOFTSTART:
	case STATE_IDLE:
		// Read the current state of the GPIO pin and
		// write back the opposite state (toggle)
		if(LED1_IS_ON)
		{
			GPIOPinWrite(GPIO_PORTF_BASE, ALL_LEDS, ALL_LEDS_OFF);
		}
		else
		{
			GPIOPinWrite(GPIO_PORTF_BASE, LED1, LED1);
		}
		break;

	case STATE_RUN:
	{
		static unsigned long ulDutyCycle = 0;	//Duty cycle of PWM
		static bool bBrighter = true;
		PWMGenDisable(PWM1_BASE, PWM_GEN_3);

		if(ulDutyCycle>=100)
			bBrighter = false;
		else if (ulDutyCycle <= 2)
			bBrighter = true;

		if(bBrighter)
			ulDutyCycle = ulDutyCycle + 2;
		else
			ulDutyCycle = ulDutyCycle - 2;
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, (SysCtlClockGet()/1000/100) *ulDutyCycle/100);
		PWMGenEnable(PWM1_BASE, PWM_GEN_3);
		break;
	}
	case STATE_ERROR:
		if(LED1_IS_ON)
		{
			GPIOPinWrite(GPIO_PORTF_BASE, ALL_LEDS, ALL_LEDS_OFF);
			GPIOPinWrite(GPIO_PORTF_BASE, LED2, LED2);
		}
		else
		{
			GPIOPinWrite(GPIO_PORTF_BASE, ALL_LEDS, ALL_LEDS_OFF);
			GPIOPinWrite(GPIO_PORTF_BASE, LED1, LED1);
		}
		break;
	default:
		UARTprintf("Wrong input(AppHandler)!!!\n");
		break;
	}
}

//*****************************************************************************
//  Indicate State with UART and LEDs
//*****************************************************************************
void IndicateState(uint8_t ui8State)
{
	switch (ui8State)
	{
	case STATE_SOFTSTART:
		//
		// UART Indication
	    //
		UARTprintf("\n>> State: SOFT-START\n");
	    UARTprintf("...Checking for error...\n");

	    //
	    // LEDs Indication
	    // Initialize Timer0 for LEDs display
	    // period: ui32Period = (SysCtlClockGet() / 10) / 2; --> 10Hz at 50%
	    ConfigTimer0((SysCtlClockGet() / FAST_BLINKY_RATE) / FAST_BLINKY_DUTY);
		break;

	case STATE_IDLE:
		//
		// UART Indication
		//
		UARTprintf("\n>> State: IDLE\n");

		//
		// LEDs Indication
	    //
		ConfigTimer0((SysCtlClockGet() / SLOW_BLINKY_RATE) / SLOW_BLINKY_DUTY);
		break;

	case STATE_RUN:
		//
		//	UART Indication
		//
		UARTprintf("\n>> State: RUNNING\n");

		//
		//	LEDs Indication
		//
		GPIOPinWrite(GPIO_PORTF_BASE, LED1, LED1);		// LED1 is always ON
		ConfigTimer0(SysCtlClockGet() / 10);
		break;

	case STATE_ERROR:
		//
		//	UART Indication
		//
		UARTprintf("\n>> State: ERROR\n");

		//
		//	LEDs Indication
		//
		ConfigTimer0((SysCtlClockGet() / FAST_BLINKY_RATE) / FAST_BLINKY_DUTY);
		break;

	default:
		UARTprintf("Wrong input(IndicateLeds)!!!\n");
		break;
	}

}

//*****************************************************************************
// Check flagError in EEPROM, address 0x00000
// return true if there is error
//		  false if there is no error
//*****************************************************************************
bool IsErrorFlagOn(void)
{
	uint32_t ui32Read;
	EEPROMRead(&ui32Read, 0x0, sizeof(ui32Read));
	if(ui32Read)
		return true;
	else
		return false;
}

void SetErrorFlag(bool bflagError)
{
	uint32_t ui32Data;
	if(bflagError)		// set flag
		ui32Data = 1;
	else				// clear flag
		ui32Data = 0;
	EEPROMProgram(&ui32Data, 0x0, sizeof(ui32Data));
}

//*****************************************************************************
//
// END OF MY_FUNCTIONS.C
//
//*****************************************************************************
