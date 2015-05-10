#include "library.h"

//*****************************************************************************
// Init System
//*****************************************************************************
void InitSystem(void)
{
	//
	// Initialize System Clock
	//
	ConfigSystemClock();

    //
    // Initialize Display
    //
	ConfigDisplay();

    //
    // Initialize Interface
    //
    ButtonsInit();
    // Config Interrupt for SW2
	GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_PIN_0, GPIO_FALLING_EDGE);
	GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0);
	IntEnable(INT_GPIOF);

    //
    // Initialize Communication via UART
    //
    ConfigUART();
    UARTprintf("\n*** TRAINING FIRMWARE 2 *** \n");
    //
    // Initialize the SysTick interrupt to process buttons, states
    //
    ConfigSysTick();

    //
    // Initialize PWM module
    //
    ConfigPWM();

    //
    // Initialize EEPROM function
    //
    ConfigEEPROM();

    //
    // Enable all interrupts
    //
    IntMasterEnable();
}

//*****************************************************************************
//
//*****************************************************************************
void ConfigSystemClock(void)
{
 	// Configure clock 40 MHz
 	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
}

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
// Source: UART TUT
//
//*****************************************************************************
void
ConfigUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);


}

//*****************************************************************************
//
// Initialize the SysTick interrupt to process colors and buttons.
//
//*****************************************************************************
void ConfigSysTick(void)
{
    SysTickPeriodSet(SysCtlClockGet() / APP_SYSTICKS_PER_SEC);
    SysTickEnable();
    SysTickIntEnable();
}

//*****************************************************************************
// Initialize display for user interface
//*****************************************************************************
void ConfigDisplay(void)
{
 	//PortF Enable
 	SysCtlPeripheralEnable(DISPLAY_PORT_PERIPHERAL);
 	//Config GPIO that connect to LED - PIN 1,2,3
 	GPIOPinTypeGPIOOutput(DISPLAY_PORT, LED_RED|LED_GREEN|LED_BLUE);
 	//Turn off all LEDs
 	GPIOPinWrite(DISPLAY_PORT, LED_RED|LED_GREEN|LED_BLUE, ALL_LEDS_OFF);
}

//*****************************************************************************
// Initialize Timer0 for LEDs display
//*****************************************************************************
void ConfigTimer0(uint32_t ui32Period)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);
}

//*****************************************************************************
// Initialize PWM
//*****************************************************************************
void ConfigPWM(void)
{
	//Configure PWM clock to match system
	SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

	//Enable the peripherals used by this program.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);			//Tiva Launchpad has 2 modules (0 and 1) and module 1 covers led pins
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	//Configure PF2 pins as PWM
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);

	//Configure PWM Options
	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, SysCtlClockGet()/1000/100);//PWM frequency = 100 KHz

	//Turn on the Output pins
	PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);

//	//Enable the PWM generator
//	PWMGenEnable(PWM1_BASE, PWM_GEN_2);
}

//*****************************************************************************
// Initialize EEPROM
//*****************************************************************************
void ConfigEEPROM(void)
{
// 1: Turns on the EEPROM peripheral.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
// 2: Performs a recovery if power failed during a previous write operation.
	EEPROMInit();
}
//*****************************************************************************
//
// END OF SYSTEMCONFIG.C
//
//*****************************************************************************
