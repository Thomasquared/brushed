#include <c8051f330.h>    // SFR declarations

//Setup Delays
#define SYSCLK  24500000  // SYSCLK frequency in Hz
#include "delay.h"
#include "../../pindefs/HK10.h"
#include "motor.h"
#include "init.h"

//Define RC States
#define unitialised  0
#define forward  1
#define backward  2
#define brake  3
#define spinLeft 4
#define spinRight 5

//Macros
#define ABS(a)	   (((a) < 0) ? -(a) : (a))

//Initialization Functions
void SYSCLK_Init(void);
void Timer0_Init(void);
void Timer1_Init(void);
void PORT_Init(void);
void PCA_Init(void);

//Motor States
void goForwards();
void braking();
void goBackwards();
void goSpinLeft();
void goSpinRight();

void calcTime();

//Stored data variables
const unsigned int rcLow = RC_LOW;
const unsigned int rcHigh = RC_HIGH;
const unsigned int rcMid = (RC_LOW + RC_HIGH) / 2;
const char maxSlew = MAX_SLEW;
const char tempLimit = TEMP_LIMIT;
const char goExpo = EXPO;

//Variables
char state = 0;
unsigned int timeCH1 = 0;
unsigned int timeCH2 = 0;
unsigned int timeCH3 = 0;
char failsafe = 0;
char timeout = 50;
char newState = 0;
char lets_get_high = 0;
char tempState = 0;
char rcTick = 0;
unsigned int desiredPWM = 0;
char newData1 = 0;
char newData2 = 0;
char chCount = 0;

//SiLabs Specific Values
unsigned int Next_Compare_Value = 0;
char pwm_state = 0;
int duty_cycleA = 0;
int duty_cycleB = 0;

void OCRA();
void OCRB();
void Overflow();

void main(){

	SYSCLK_Init();
	PORT_Init();
    PCA_Init();
	Timer0_Init();
    Timer1_Init();
	EA = 0;

    clrALow();
    clrAHigh();
    clrBLow();
    clrBHigh();
    clrCLow();
    clrCHigh();
	delay_ms(100);
	

	EA = 1;
	PCA0MD |= 0x40; //Start watchdog timer
	while(1){
		PCA0CPH2 = 0x00; //Set watchdog reset
		if (rcTick) {
            //Manage State
            if (state != newState) {

            }//Manage slew rate
            else if (1) {

            }
            rcTick = 0;
		}
		if (newTime) {
			calcTime();
			newTime = 0;
		}
	}	
}


//Motor State Functions
void goForwards(){
    EA = 0; //Disable interrupts
    if (state != forward){
        //Clear unused pins
        clrALow();
        clrBHigh();
        clrCLow();

        uSecDelay(uSecToInstCycles(200)); //Fet switch delay

        setBLow(); //Enable Common pin
	
	    uSecDelay(uSecToInstCycles(200)); //Fet switch delay
	    setAHigh(); //Enable one motor	
    	//Both motors are enabled at slightly different times
    	//to hopefully be a little bit nicer to the fets
    	//200us should have no noticible effect on driving straight
        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
        setCHigh(); //Enable 2nd motor

        state = forward; //Up
    }
    EA = 1; //Enable interrupts
}

void braking(){
    EA = 0; //Disable interrupts
    if (state != brake){
        //Clear unused pins
        clrAHigh();
        clrBHigh();
        clrCHigh();

        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
	    setALow();
        setBLow();
        setCLow();

        state = brake;
    }
    EA = 1; //Enable interrupts
}

void goBackwards(){
    EA = 0; //Disable interrupts
    if (state != backward){

        //Clear unused pins
        clrAHigh();
        clrBLow();
        clrCHigh();

        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
        setBHigh(); //Enable Common pin
	
	    uSecDelay(uSecToInstCycles(200)); //Fet switch delay
	    setALow(); //Enable one motor
	
	    //Both motors are enabled at slightly different times
	    //to hopefully be a little bit nicer to the fets
	    //200us should have no noticible effect on driving straight
        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
        setCLow(); //Enable 2nd motor
	
        state = backward;
    }
    EA = 1; //Enable interrupts
}

void goSpinLeft(){
    EA = 0; //Disable interrupts
    if (state != spinLeft){

        //Clear unused pins
        clrAHigh();
        clrBLow();
        clrBHigh();
        clrCLow();

        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
        setALow(); //Enable Common pin
	
	    uSecDelay(uSecToInstCycles(200)); //Fet switch delay
	    setCHigh(); //Enable one motor
	
        state = spinLeft;
    }
    EA = 1; //Enable interrupts
}

void goSpinRight(){
    EA = 0; //Disable interrupts
    if (state != spinRight){

        //Clear unused pins
        clrALow();
        clrBLow();
        clrBHigh();
        clrCHigh();

        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
        setAHigh(); //Enable Common pin
	
	    uSecDelay(uSecToInstCycles(200)); //Fet switch delay
	    setCLow(); //Enable one motor
	
        state = spinRight;
    }
    EA = 1; //Enable interrupts
}


void Timer1_ISR(void) interrupt 3 {
    //Check failsafe
    if (timeout == 0) { //If we are out of counts go to braking mode
        newState = brake;
    } else {
        timeout--; //otherwise decrease failsafe counter
    }
    rcTick++;
}

unsigned int deadzone = RC_DEADZONE;
unsigned int buffer = RC_BUFFER;
unsigned int maxOut = RC_MAXOUT;

void INT0_ISR(void) interrupt 0 {
    EA = 0;
     unsigned int time = TL0 + 256 * TH0;
    
    if (time > rcHigh * 2){
        chCount = 0;
    } else {
        chCount++;
        if (chCount == 1){
            timeCH1 = time;
            newData1 = 1;
        } else if (chCount == 2){
            timeCH2 = time;
            newData2 = 1;
        } else if (chCount == 3){
            timeCH3 = time;
        }
    }
    
    TH0 = 0x00; // Reinit Timer0 High register
    TL0 = 0x00; // Reinit Timer0 Low register
	
	EA = 1;
	
}

void calcTime(){	
    //Update PWM and direction
    if (((time > (rcLow - buffer)) && (time < (rcHigh + buffer)))) { //Check for valid pulse
        if (failsafe > 10) { //Need 10 good pulses before we start
            failsafe = 15; //Set valid pulse counter (15-10 = 5) therfore we can receive 5 bad pulses before we stop
            timeout = 50; //Set timeout counter

            //Vaild signal
                      
        } else {
            failsafe++; //If havent got 10 valid pulses yet increase count and keep waiting
        }
    } else { //If there is an invalid pulse
        //Failsafe
        failsafe--; //decrease counter
        if (failsafe < 10) { //If we have had enough invalid pulses
            newState = brake; //go to failsafe breaking mode
        }
    }
}


void PCA0_ISR(void) interrupt 11 {
    EA = 0;
    if (CCF0) // If Module 0 caused the interrupt
    {
        CCF0 = 0; // Clear module 0 interrupt flag.
        // Set up the variable for the following edge
        if (pwm_state) {
            pwm_state = 0;
            if (duty_cycle <= (PCA_TIMEOUT - 5*PCA_TIMEOUT / 100)) { // OCR2 < 255
                Next_Compare_Value = PCA0CP0 + PCA_TIMEOUT - duty_cycle;
                //Turn Pins off
                if (state != brake) {
                    clrForwardHigh();
                    clrBackwardHigh();
                }
            } else {
                Next_Compare_Value = PCA0CP0 + 500;
            }
        } else {
            pwm_state = 1;
            if (duty_cycle > 5*PCA_TIMEOUT / 100) { //OCR2 > 0
                Next_Compare_Value = PCA0CP0 + duty_cycle;
                //Turn Pins on
                if (state == forward) {
                    setForwardHigh();
                } else if (state == backward) {
                    setBackwardHigh();
                }
            }
            if (duty_cycle > (PCA_TIMEOUT -  5*PCA_TIMEOUT / 100) && state != brake) {
                lets_get_high++; //Gotta keep the charge pump circuit charged
                if (lets_get_high > 50) { //If it hasn't had a chance to charge in a while
                    clrForwardHigh(); //Clear it then reset counter
                    clrBackwardHigh(); //Now pumped up and remaining high so we don't nasty shoot through which ends in magic smoke :)
                    lets_get_high = 0;
                }
            }
            Next_Compare_Value = PCA0CP0 +  duty_cycle;
        }
		PCA0CPL0 = (Next_Compare_Value & 0x00FF);
	    PCA0CPH0 = (Next_Compare_Value & 0xFF00) >> 8;
    }
	else // Interrupt was caused by other bits.
	{
    	PCA0CN &= ~0x86; // Clear other interrupt flags for PCA
	}
	EA = 1;
}


void OCRA(){
}

void OCRB(){
}

void Overflow(){
}
