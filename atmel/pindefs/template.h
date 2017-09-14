/* -----------------------------

ESC Name:         Template Header
Current Rating:   1000000Amps
Added by:         Steven Martin
Date:             28/03/2011
Tested by:        Your Mother
Date:             28/03/2011

Comments:         
This is a template for defining all of the pins available on 
your brushless ESC. The use of this header structure is to 
keep the code generic and hopefully make it easier for others
to develop and use.

------------------------------- */
//Need to setup the high and low sides
//Must set direction port, port and pin. 

//Channel A
#define SET_HIGH_A_PORT DDRD
#define HIGH_A_PORT PORTD
#define HIGH_A 4

#define SET_LOW_A_PORT DDRD
#define LOW_A_PORT PORTD
#define LOW_A  5


//Channel B
#define SET_HIGH_B_PORT DDRC
#define HIGH_B_PORT PORTC
#define HIGH_B 5

#define SET_LOW_B_PORT DDRC
#define LOW_B_PORT PORTC
#define LOW_B  4

//Channel C

#define SET_HIGH_C_PORT DDRC
#define HIGH_C_PORT PORTC
#define HIGH_C 3

#define SET_LOW_C_PORT DDRB
#define LOW_C_PORT PORTB
#define LOW_C  0

//Also we need to setup the RC inputs

//RC Pins
#define SET_RC_PORT DDRD
#define RC_PORT PIND
#define RC_PIN  2

#define RC_LOW  280
#define RC_HIGH 480
#define RC_MID 380

/* There is also lots of connections for inputs from ADC and stuff but that can be added later
   if we want to be able to monitor battery voltage could also look at back EMF and some other stuff */

//RC Time variables 
#define DEADTIME 20
#define BUFFERTIME 20
#define MAXOUTTIME 20

#define MAXSLEW 2

//Do we want to be able to calibrate stuff
#define CALIBRATION 0

//Do you want EXPO Rates
#define EXPO 0

//Do you want TEMP LIMITING
#define TEMP_LIMIT 0

//Are we using active low.
#ifndef LOW_ACTIVE_LOW
    #define LOW_ACTIVE_LOW 0
#endif

#ifndef HIGH_ACTIVE_LOW
    #define HIGH_ACTIVE_LOW 0
#endif

//Set temperature ADC Pin
#define TEMP_ADMUX 0b00000001  //ADC mux 1
#define I_AM_WARM 190
#define I_AM_HOT  170

//Define cpu speed
#define F_CPU 8000000UL
