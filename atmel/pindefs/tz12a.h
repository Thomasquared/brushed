/* -----------------------------

ESC Name:         HobbyKing 12A BlueSeries Brushless Speed Controller
Current Rating:   12Amps
Added by:         Aaron Knight
Date:             29/03/2011
Tested by:        
Date:             

Comments:
HobbyKing 12A BlueSeries Brushless Speed Controller - http://hobbyking.com/hobbyking/store/uh_viewItem.asp?idProduct=11429

Compiled from Steve's tracing of the TZ12A PCB, still need to confirm the hi/low states before public release
------------------------------- */
//Need to setup the high and low sides
//Must set direction port, port and pin. 


//Channel A
//PC.3
#define SET_HIGH_A_PORT DDRC
#define HIGH_A_PORT PORTC
#define HIGH_A 3

//PB.0
#define SET_LOW_A_PORT DDRB
#define LOW_A_PORT PORTB
#define LOW_A 0 

//Channel B
//PC.5
#define SET_HIGH_B_PORT DDRC
#define HIGH_B_PORT PORTC
#define HIGH_B 5

//PC.4
#define SET_LOW_B_PORT DDRC
#define LOW_B_PORT PORTC
#define LOW_B  4

//Channel C
//PD.4
#define SET_HIGH_C_PORT DDRD
#define HIGH_C_PORT PORTD
#define HIGH_C 4

//PD.5
#define SET_LOW_C_PORT DDRD
#define LOW_C_PORT PORTD
#define LOW_C  5

//Also we need to setup the RC inputs
//RC Pins
//PD.2
#define SET_RC_PORT DDRD
#define RC_PORT PIND
#define RC_PIN  2
#define RC_PIN2 3

#define RC_LOW  290
#define RC_HIGH 460
#define RC_MID 375

/* There is also lots of connections for inputs from ADC and stuff but that can be added later
   if we want to be able to monitor battery voltage could also look at back EMF and some other stuff */

//Are we using active low.
#ifndef LOW_ACTIVE_LOW
    #define LOW_ACTIVE_LOW 0
#endif

#ifndef HIGH_ACTIVE_LOW
    #define HIGH_ACTIVE_LOW 1
#endif

//RC Time variables 
#define DEADTIME 10 
#define BUFFERTIME 20
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
