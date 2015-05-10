//*****************************************************************************
//
//	TRAINING FIRMWARE - HOMEWORK 2
//
//	Module		: 	main.c
//	Description :
//  				4 modes (indicating by leds...)
//					     Softstart --> Idle or Error, depending on last state
//				 		 Idle      --> Run or Error
//				 		 Run       --> Idle or Error
//				 		 Error     --> Idle
//					Button_1 (left) to simulate user's action
//					Button_2 (right) to simulate fault, error
//	Author		: 	HT94
//	Notes		:
//
//*****************************************************************************

#include "library.h"

//*****************************************************************************
//
// State structure
//
//*****************************************************************************
volatile sState g_sState;
bool flagRun = false;
bool flagError;
//*****************************************************************************
//
// Called by the NVIC as a result of SysTick Timer rollover interrupt flag
//
// Checks buttons and calls AppHandler to manage button events.
//
// AppHandler function will determine which button was pressed and change program accordingly
//
//*****************************************************************************
void SysTickIntHandler(void)
{
    g_sState.ui32Buttons = ButtonsPoll(0,0);
    AppHandler();
}

//*****************************************************************************
// Enter new state
//*****************************************************************************
void SwitchState(uint8_t ui8newState)
{
	//
	//	Indicate New State (UART and LEDs)
	//
	IndicateState(ui8newState);

	//
	//  Save current state and Update new state
	//
	g_sState.ui8PreviousState = g_sState.ui8State;
	g_sState.ui8State = ui8newState;

	//
	//	Complete switching to new state
	//
	switch(ui8newState)
	{
		case STATE_SOFTSTART:
			break;
		case STATE_IDLE:
		{
			PWMGenDisable(PWM1_BASE, PWM_GEN_3);
		 	//Config GPIO that connect to LED - PIN 1,2,3
		 	GPIOPinTypeGPIOOutput(DISPLAY_PORT, LED_RED|LED_GREEN|LED_BLUE);
		 	GPIOPinWrite(DISPLAY_PORT, ALL_LEDS, ALL_LEDS_OFF);

		 	UARTprintf("To enter RUN mode:\n- Enter yes (y) from Terminal or "
		 			"\n- Press & hold button 1 "
		 			"for more than %d sec(s) to run: ",IDLE_BUTTON_PRESSED_TIMEOUT);

		 	// check to change flagRun, either by UART (follow) or Button1
			while((!flagRun)&&(!flagError))
				if(UARTRxBytesAvail())
				{
					if(UARTgetc() == 'y')
					{
						flagRun = true;
						UARTprintf("\nReceived 'y'.");
					}
				}
			break;
		}
		case STATE_RUN:
		{
		 	//Config LED2 as PWM pin
		 	GPIOPinTypePWM(DISPLAY_PORT, LED2);
		 	UARTprintf("To switch back to Idle mode, hold left button "
		 			"for more than %d sec(s)\n",RUN_BUTTON_PRESSED_TIMEOUT	);
		 	break;
		}
		case STATE_ERROR:
		{
			PWMGenDisable(PWM1_BASE, PWM_GEN_3);
		 	//Config GPIO that connect to LED - PIN 1,2,3
		 	GPIOPinTypeGPIOOutput(DISPLAY_PORT, LED_RED|LED_GREEN|LED_BLUE);
		 	//
		 	//Save error state
		 	//
			SetErrorFlag(flagError);
			UARTprintf("To switch back to Idle mode, quickly press left button "
					" %d time\n",SWITCH_ERROR_TO_IDLE_PRESS_TIME);
			break;
		}
		default:
			UARTprintf("LOL WUT\n");
			break;
	}
}

//*****************************************************************************
//
// Main: performs init and manages system.
//
//*****************************************************************************
void main(void)
{
	//
	//	Init System
	//
	InitSystem();
	UARTprintf("\n**********************************************");
    UARTprintf("\n***          TRAINING FIRMWARE 2           ***");
    UARTprintf("\n**********************************************\n");

	//
	//	Enter Soft-start state
	//
	SwitchState(STATE_SOFTSTART);
//	SetErrorFlag(false);
    while(1)
    {
    	switch(g_sState.ui8State)
		{
			case STATE_SOFTSTART:
			{
				// delay 1s to indicate soft-start state
				SysCtlDelay(SysCtlClockGet()/3);
				if(IsErrorFlagOn())
				{
					flagError = true;
					UARTprintf("> Error from last run, check PLZ!\n");
					SwitchState(STATE_ERROR);
				}
				else if(flagError)
				{
					UARTprintf("> Error occured during Soft-start, check PLZ!\n");
					SwitchState(STATE_ERROR);
				}
				else
				{
					flagError = false;
					UARTprintf("> No Error found! Enter Idle mode.\n");
					SwitchState(STATE_IDLE);
				}
				break;
			}
			case STATE_IDLE:
			{
				// check if flagRun is set
				if(flagRun)
				{
					UARTprintf("\n> flagRun is set! Enter Run mode.\n");
					SwitchState(STATE_RUN);
				}
				if(flagError)
				{
					UARTprintf("\n> Shit happened! Enter Error mode.\n");
					flagRun = false;
					SwitchState(STATE_ERROR);
				}
				break;
			}
			case STATE_RUN:
			{
				//
				//	Check flagRun and flagError
				//
				if(!flagRun)
				{
					UARTprintf("> flagRun is clear! Enter Idle mode.\n");
					SwitchState(STATE_IDLE);
				}
				if(flagError)
				{
					UARTprintf("> Error happened! Enter Error mode\n");
					flagRun = false;
					SwitchState(STATE_ERROR);
				}
				break;
			}
			case STATE_ERROR:
			{
				if(!flagError)
				{
					// save error state (no error)
					SetErrorFlag(flagError);
					UARTprintf("> flagError is clear! Enter Idle mode.\n");
					SwitchState(STATE_IDLE);
				}
				break;
			}
			default:
				UARTprintf("Wrong State(Main)!!!\n");
				break;
		}
    }
}

//*****************************************************************************
// Interrupt for RIGHT BUTTON
//*****************************************************************************
void ButtonISR(void)
{
	GPIOIntClear(GPIO_PORTF_BASE, RIGHT_BUTTON);
	flagError = true;
}
//*****************************************************************************
//
// Handler to manage the button press events and state machine transitions
// that result from those button events.
//
// This function is called by the SysTickIntHandler if a button event is
// detected. Function will determine which button was pressed and change various
// elements of the global state structure accordingly.
//
//*****************************************************************************
void AppHandler(void)
{
	// number of tick since the last pulse up
	static uint32_t ui32TickCounter = 0;

	// increase counter
	ui32TickCounter++;
	//
	//	Complete Task
	//
	switch(g_sState.ui8State)
	{
	case STATE_SOFTSTART:
		ui32TickCounter = 0;
		break;
	case STATE_IDLE:
	{
		//
		// Switch based on buttons
		//
		switch(g_sState.ui32Buttons & ALL_BUTTONS)
		{
			case LEFT_BUTTON:
			{
				//
				// DO SOMETHING
				//
				break;
			}
			default:
			{
				//
				// Check if the button has been pressed long enough to enter RUN state
				//
				if(ui32TickCounter >= APP_STATE_IDLE_TIMEOUT)
				{
					UARTprintf("\nLeft Button is pressed longer than %d sec. Set flagRun.",IDLE_BUTTON_PRESSED_TIMEOUT);
					flagRun = true;
				}

				//
				// reset the tick counter when no buttons are pressed
				//
				ui32TickCounter = 0;
				break;
			}
		}
		break;
	}
	case STATE_RUN:
		//
		// Switch based on buttons
		//
		switch(g_sState.ui32Buttons & ALL_BUTTONS)
		{
			case LEFT_BUTTON:
			{
				//
				// LET THE COUNTER BUILD UP
				//
				break;
			}
			default:
			{
				//
				// Check if the button has been pressed long enough to enter idle state
				//
				if(ui32TickCounter >= APP_STATE_RUN_TIMEOUT)
				{
					flagRun = false;
				}

				//
				// reset the tick counter when no buttons are pressed
				//
				ui32TickCounter = 0;
				break;
			}
		}
		break;

	case STATE_ERROR:
	{
		static bool flagPress = false;
		static char ctemp = 0;
		//
		// Switch based on buttons
		//
		switch(g_sState.ui32Buttons & ALL_BUTTONS)
		{
			case LEFT_BUTTON:
			{
				// if button changed state, increase number of push by 1
				if(!flagPress)
				{
					flagPress = true;	// button has been pressed
					g_sState.ui8NumPush++;
				}
				// indicate that start counting number of push in a small period of time
				ctemp = 1;
				break;
			}
			default:
			{
				// button is not being pressed
				flagPress = false;

				// if pressed enough timeS in limited time
				if( (ui32TickCounter >= APP_STATE_ERROR_TIMEOUT))
				{
					// reset variables
					ctemp = 0;
					ui32TickCounter = 0;

					// check if condition is met
					if(g_sState.ui8NumPush == SWITCH_ERROR_TO_IDLE_PRESS_TIME)
					{
						flagError = false;
					}
					g_sState.ui8NumPush = 0;
				}
				// if nothing happens, reset counter
				if(!ctemp)
				{
					ui32TickCounter = 0;
				}
				break;
			}
		}
		break;
	}
	default:
		UARTprintf("Wrong input(AppHandler)!!!\n");
		break;
	}
}

//*****************************************************************************
//
// END OF MAIN.C
//
//*****************************************************************************
