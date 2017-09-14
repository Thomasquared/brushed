/* -----------------------------

ESC Name:         Sentilon
Current Rating:   100A
Added by:         Steven Martin
Date:             28/03/2011
Tested by:        
Date:             

Comments:         
Beetle ESC not yet battle tested

------------------------------- */
//Need to setup the high and low sides
//Must set direction port, port and pin. 

//Channel A
#define SET_HIGH_A_PORT DDRD
#define HIGH_A_PORT PORTD
#define HIGH_A 3

#define SET_LOW_A_PORT DDRB
#define LOW_A_PORT PORTB
#define LOW_A  2


//Channel B
#define SET_HIGH_B_PORT DDRD
#define HIGH_B_PORT PORTD
#define HIGH_B 4

#define SET_LOW_B_PORT DDRB
#define LOW_B_PORT PORTB
#define LOW_B  1

//Channel C
#define SET_HIGH_C_PORT DDRD
#define HIGH_C_PORT PORTD
#define HIGH_C 5

#define SET_LOW_C_PORT DDRB
#define LOW_C_PORT PORTB
#define LOW_C  0

//Also we need to setup the RC inputs
//RC Pins
#define SET_RC_PORT DDRD
#define RC_PORT PIND
#define RC_PIN  2

#define RC_LOW  140
#define RC_HIGH 240
#define RC_MID 190

/* There is also lots of connections for inputs from ADC and stuff but that can be added later

   if we want to be able to monitor battery voltage could also look at back EMF and some other stuff */

//RC Time variables 
#define DEADTIME 5
#define BUFFERTIME 20
#define MAXOUTTIME 10

//Do we want to be able to calibrate stuff
#define CALIBRATION 0

#define MAX_SLEW 4

//Do you want EXPO Rates
#define EXPO 0

//Do you want TEMP LIMITING
#define TEMP_LIMIT 0

//Are we using active low.
#define LOW_ACTIVE_LOW 1
#define HIGH_ACTIVE_LOW 0


//Set temperature ADC Pin
#define TEMP_ADMUX 0b00000001  //ADC mux 1
#define I_AM_WARM 190
#define I_AM_HOT  170

//Define cpu speed
#define F_CPU 8000000UL
