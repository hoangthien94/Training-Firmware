//*****************************************************************************
//
// AppFunction.h
// Functions prototypes for AppFunction.c
//
//*****************************************************************************

//*****************************************************************************
//
// Handler to manage the button press events and state machine transitions
// that result from those button events.
//
// This function is called by the SysTickIntHandler if a button event is
// detected. Function will determine which button was pressed and tweak various
// elements of the global state structure accordingly.
//
//*****************************************************************************
//extern void AppHandler(volatile sState*);

//*****************************************************************************
//	Indicate current state on UART and LEDs
//*****************************************************************************
extern void IndicateState(uint8_t);

//*****************************************************************************
//	Check flags
//	return 1 if flag is set, otherwise return 0
//*****************************************************************************
extern bool IsErrorFlagOn(void);

//*****************************************************************************
//	Set/Clear Flag as needed
//*****************************************************************************
extern void SetErrorFlag(bool);

//*****************************************************************************
//
// END OF APPFUCNTION.H
//
//*****************************************************************************
