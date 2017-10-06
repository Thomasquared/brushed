/* -----------------------------

ESC Name:         DYS SN16A
Current Rating:   16A
Added by:         Thomas Thomas
Date:             10/06/2017
Tested by:        
Date:             

Comments:
Should have the same pinout as the DYS SN20A
ADC still needs to be set up.
Untested

------------------------------- */
//Need to setup the high and low sides
//Must set direction port, port and pin. 

//Channel A
#define SET_HIGH_A_PORT DDRD
#define HIGH_A_PORT PORTD
#define HIGH_A 2

#define SET_LOW_A_PORT DDRB
#define LOW_A_PORT PORTB
#define LOW_A  1

//Channel B
#define SET_HIGH_B_PORT DDRD
#define HIGH_B_PORT PORTD
#define HIGH_B  3

#define SET_LOW_B_PORT DDRD
#define LOW_B_PORT PORTD
#define LOW_B  7

//Channel C
#define SET_HIGH_C_PORT DDRD
#define HIGH_C_PORT PORTD
#define HIGH_C 4

#define SET_LOW_C_PORT DDRD
#define LOW_C_PORT PORTD
#define LOW_C  5

//Are we using active low.
#define LOW_ACTIVE_LOW 0
#define HIGH_ACTIVE_LOW 1


//Also we need to setup the RC inputs

//RC Pins
#define SET_RC_PORT DDRB

#define RC_PORT PINB

#define RC_PIN  0

#define RC_LOW  150
#define RC_HIGH 400
#define RC_MID 275

/* There is also lots of connections for inputs from ADC and stuff but that can be added later
   if we want to be able to monitor battery voltage could also look at back EMF and some other stuff */

//RC Time variables 
#define DEADTIME 10
#define BUFFERTIME 40
#define MAXOUTTIME 20

//Do we want to be able to calibrate stuff
#define CALIBRATION 0

//Do you want EXPO Rates
#define EXPO 0

//Do you want TEMP LIMITING
#define TEMP_LIMIT 0

//Max Slew rate
#define MAX_SLEW  2 //Change in PWM 0-255 per clock tick (500Hz @ 8MHZ or 1Khz @ 16Mhz)

//Set temperature ADC Pin
#define TEMP_ADMUX 0b00000001  //ADC mux 1
#define I_AM_WARM 180
#define I_AM_HOT  170

//Define cpu speed
#define F_CPU 16000000UL
