//*****************************************************************************
//
// DEFINE.H
//
//*****************************************************************************

//*****************************************************************************
//	DEFINE BY USER
//*****************************************************************************

#define STATE_ERROR										0
#define STATE_SOFTSTART									1
#define STATE_IDLE										2
#define STATE_RUN										3

#define FAST_BLINKY_RATE								10		// Hz
#define FAST_BLINKY_DUTY								1/2		// 50%
#define SLOW_BLINKY_RATE								1		// Hz
#define SLOW_BLINKY_DUTY								1/2		// 50%

#define IDLE_BUTTON_PRESSED_TIMEOUT						1		// sec
#define RUN_BUTTON_PRESSED_TIMEOUT						1		// sec
#define ERROR_BUTTON_PRESSED_TIMEOUT					0.5		// sec
#define SWITCH_ERROR_TO_IDLE_PRESS_TIME					2

#define LED1											LED_RED
#define LED2											LED_BLUE

//*****************************************************************************
//  DEFINE BY PROGRAMMERS
//*****************************************************************************

//#define UART_BUFFERED
#define APP_SYSTICKS_PER_SEC             				60
#define APP_BUTTON_POLL_DIVIDER          				30

#define APP_FAST_BLINKY_RATE							(APP_SYSTICSK_PER_SEC / FAST_BLINKY_RATE)
#define APP_SLOW_BLINKY_RATE							(APP_SYSTICSK_PER_SEC / SLOW_BLINKY_RATE)
#define APP_STATE_IDLE_TIMEOUT							(APP_SYSTICKS_PER_SEC * IDLE_BUTTON_PRESSED_TIMEOUT)
#define APP_STATE_RUN_TIMEOUT							(APP_SYSTICKS_PER_SEC * RUN_BUTTON_PRESSED_TIMEOUT)
#define APP_STATE_ERROR_TIMEOUT							(APP_SYSTICKS_PER_SEC * ERROR_BUTTON_PRESSED_TIMEOUT)

//#define APP_STATE_3_TIMEOUT				(APP_SYSTICKS_PER_SEC * USER_TIMEOUT_3)
//#define APP_STATE_4_TIMEOUT				(APP_SYSTICKS_PER_SEC * USER_TIMEOUT_4)

//*****************************************************************************
//  DEFINE FOR READABILITY
//*****************************************************************************

#define DISPLAY_PORT_PERIPHERAL			SYSCTL_PERIPH_GPIOF
#define DISPLAY_PORT					GPIO_PORTF_BASE

#define LED_RED							GPIO_PIN_1
#define LED_BLUE						GPIO_PIN_2
#define LED_GREEN						GPIO_PIN_3
#define ALL_LEDS						LED_RED|LED_GREEN|LED_BLUE

#define LED1_IS_ON						GPIOPinRead(DISPLAY_PORT, LED1)
#define LED2_IS_ON						GPIOPinRead(DISPLAY_PORT, LED2)

#define ALL_LEDS_OFF					0
#define ALL_LEDS_ON						0xFF

#define STATE_HAS_CHANGED				1//	g_sState.ui8State != g_sState.ui8PreviousSTATE

//*****************************************************************************
//  DEFINE FOR TESTING
//*****************************************************************************

//#define DEBUG_TEST_LEDS
//#define DEBUG_TEST_BUTTONS

//#define APP_STATE_TEST_BUTTONS

//*****************************************************************************
//
// Structure typedef to make storing USER application state data simpler.
//      ui32State:		The current USER application STATE
//      ui32Buttons:	bit map representation of buttons being pressed
//		ui3StateTimer:	Inner timer to clarify if the action sequence produce STATE 3 or STATE 4
//		cNumPush:		number of presses counted since the first edge, in the period of 3 secs
//
//*****************************************************************************
typedef struct
{
	uint8_t ui8State;
	uint8_t ui8PreviousState;
    uint32_t ui32StateTimer;
    uint32_t ui32Buttons;
    uint8_t ui8NumPush;

}sState;

//*****************************************************************************
//
// Global variables
//
//*****************************************************************************
extern volatile sState g_sState;
//extern volatile uint32_t ui32tempState;
extern bool flagRun;
extern bool flagError;
//*****************************************************************************
//
// Functions prototypes
//
//*****************************************************************************
void AppHandler(void);

//*****************************************************************************
//
// END OF DEFINE.H
//
//*****************************************************************************
