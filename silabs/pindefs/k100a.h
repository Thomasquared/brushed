/* -----------------------------
ESC Name:         Kforce 100A
Current Rating:   100Amps
Added by:         Steven Martin
Date:             12/06/2011
Tested by:        
Date:             

Comments:         


------------------------------- */
//Need to setup the high and low pins. 
//Bits
sbit LOW_A = P0^1;    
sbit HIGH_A = P0^0;    
sbit LOW_B = P0^7;    
sbit HIGH_B = P0^6;    
sbit LOW_C = P3^1;   
sbit HIGH_C = P0^2;  

//Are we using active low.

#define LOW_ACTIVE_LOW 0
#define HIGH_ACTIVE_LOW 0

//IO defs
//All pins which are outputs should be 1s
#define PORT0_DIR 0x00;		//Set digital outputs
#define PORT1_DIR 0x00;		//Set digital outputs

//Set which pin the interrupt is attatched to
#define INT0_PIN 0x05;			//Set interrupt active high & on pin 0.4

#define INT0_SKIP 0x30;

#define PCA_TIMEOUT 6000 //Run at 4khz
sfr16 PCA0CP0 = 0xFB; // PCA0 Compare Register Definition

//Also we need to setup the RC times

#define RC_LOW  6500

#define RC_HIGH 11700

#define RC_MID 9000

#define RC_DEADZONE 100
#define RC_BUFFER 1000
#define RC_MAXOUT 500


   //Do you want EXPO Rates

#define EXPO 0



//Do you want TEMP LIMITING

#define TEMP_LIMIT 1



//Max Slew rate

#define MAX_SLEW  80 //Change in PWM 0-255 per clock tick (500Hz @ 8MHZ or 1Khz @ 16Mhz)



//Set temperature ADC Pin

#define TEMP_ADMUX P2^2  //ADC mux 1

#define I_AM_WARM 180

#define I_AM_HOT  170
