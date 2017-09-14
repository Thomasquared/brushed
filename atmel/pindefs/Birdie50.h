/* -----------------------------

ESC Name:         Birdie 50A
Current Rating:   50A
Added by:         Steven Martin
Date:             27/07/2011
Tested by:        
Date:             

Comments:        
Still would not classify as battle tested.

------------------------------- */
//Need to setup the high and low sides
//Must set direction port, port and pin. 

//Channel A
#define SET_HIGH_A_PORT DDRD
#define HIGH_A_PORT PORTD
#define HIGH_A 7

#define SET_LOW_A_PORT DDRD
#define LOW_A_PORT PORTD
#define LOW_A  5

//Channel B
#define SET_HIGH_B_PORT DDRD
#define HIGH_B_PORT PORTD
#define HIGH_B  3

#define SET_LOW_B_PORT DDRD
#define LOW_B_PORT PORTD
#define LOW_B  4

//Channel C
#define SET_HIGH_C_PORT DDRD
#define HIGH_C_PORT PORTD
#define HIGH_C 1

#define SET_LOW_C_PORT DDRD
#define LOW_C_PORT PORTD
#define LOW_C  0

//Are we using active low.
#define LOW_ACTIVE_LOW 0
#define HIGH_ACTIVE_LOW 0


//Also we need to setup the RC inputs

//RC Pins
#define SET_RC_PORT DDRD

#define RC_PORT PIND

#define RC_PIN  2

#define RC_LOW  300
#define RC_HIGH 450


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
