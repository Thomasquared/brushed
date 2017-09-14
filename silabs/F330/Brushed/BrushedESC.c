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
#define backward 2
#define brake  3

//Initialization Functions
void SYSCLK_Init(void);
void Timer0_Init(void);
void Timer1_Init(void);
void PORT_Init(void);
void PCA_Init(void);

void goForwards();
void braking();
void goBackwards();

void calcTime();

//Stored data variables
unsigned int rcLow = RC_LOW;
unsigned int rcHigh = RC_HIGH;
unsigned int rcMid = (RC_LOW + RC_HIGH) / 2;
char maxSlew = MAX_SLEW;
char tempLimit = TEMP_LIMIT;
char goExpo = EXPO;

//Variables
char state = 0;
unsigned int time = 0;
char failsafe = 0;
char timeout = 50;
char newState = 0;
char lets_get_high = 0;
char tempState = 0;
char rcTick = 0;
unsigned int desiredPWM = 0;
char newTime = 0;

//SiLabs Specific Values
unsigned int Next_Compare_Value = 0;
char pwm_state = 0;
int duty_cycle = 0;
void main(){

	SYSCLK_Init();
	PORT_Init();
    PCA_Init();
	Timer0_Init();
    Timer1_Init();
	EA = 0;

    clrBackwardLow();
    clrBackwardHigh();
    clrForwardLow();
    clrForwardHigh();

	clrSpareLow();
	clrSpareHigh();
	delay_ms(100);
	

	EA = 1;
	PCA0MD |= 0x40; //Start watchdog timer
	while(1){
		PCA0CPH2 = 0x00; //Set watchdog reset
		if (rcTick) {
            //Manage State
            if (state != newState) {
                //If we are in the brake state then we can go in either direction
                //if (state == brake){
                if (newState == forward && state == brake) {
                    duty_cycle = 0;
                    goForwards();
                } else if (newState == backward && state == brake) {
                    duty_cycle = 0;
                    goBackwards();
                } else {
                    //If we have ramped down then go to braking mode
                    if (duty_cycle <= 1*maxSlew) {
                        braking();
                        duty_cycle = PCA_TIMEOUT;
                    }//Other wise we want to change our state so first we need to ramp down, then we can change state.
                    else {
                        duty_cycle = duty_cycle - maxSlew;
						if (duty_cycle < 0){
							duty_cycle = 3;
						}
                    }
                }
            }//Manage slew rate
            else if (duty_cycle != desiredPWM && state != brake) {
                if (desiredPWM > (duty_cycle + maxSlew)) {
                    duty_cycle = duty_cycle + maxSlew;
                } else if ((int)desiredPWM < (duty_cycle - maxSlew)) {
					duty_cycle = duty_cycle - maxSlew;
                } else {
                    duty_cycle = desiredPWM;
                }
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
void goForwards() {
    if (state != forward) {
        EA = 0; //Disable interrupts

        //Clear unused pins
        clrBackwardHigh();
        clrBackwardLow();
        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
        setForwardLow();
        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
        setForwardHigh();
        state = forward; //Up
		pwm_state = 1;
        EA = 1; //Enable interrupts
    }
}



void braking() {
    if (state != brake) {
        EA = 0; //Disable interrupts

        //Clear unused pins
        clrForwardHigh();
        clrBackwardHigh();
        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
        setForwardLow();
        setBackwardLow();
        state = brake;
        EA = 1; //Enable interrupts
    }
}



void goBackwards() {
    if (state != backward) {
        EA = 0; //Disable interrupts
   
   		//Clear high
        clrForwardHigh();
        clrForwardLow();
        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
        setBackwardLow();
        uSecDelay(uSecToInstCycles(200)); //Fet switch delay
        setBackwardHigh();
        state = backward;
		pwm_state = 1;
        EA = 1; //Enable interrupts
    }
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
    time = TL0 + 256 * TH0;
    TH0 = 0x00; // Reinit Timer0 High register
    TL0 = 0x00; // Reinit Timer0 Low register
	newTime = 1;
	EA = 1;
	
}

void calcTime(){	
    //Update PWM and direction
    if (((time > (rcLow - buffer)) && (time < (rcHigh + buffer)))) { //Check for valid pulse
        if (failsafe > 10) { //Need 10 good pulses before we start
            failsafe = 15; //Set valid pulse counter (15-10 = 5) therfore we can receive 5 bad pulses before we stop
            timeout = 50; //Set timeout counter

            //Vaild signal
            if (time > (rcMid - deadzone) && time < (rcMid + deadzone)) { //If center brake
                desiredPWM = PCA_TIMEOUT;
                newState = brake;
            } else if (time < rcMid) { //If low then go backwards
                if (time < (rcLow + maxOut)) {
                    desiredPWM = PCA_TIMEOUT;
                } else {
                    desiredPWM = (long) (rcMid - deadzone - time) * PCA_TIMEOUT / (rcMid - deadzone - rcLow);
                    if (EXPO) {
                        desiredPWM = (long) desiredPWM * desiredPWM / PCA_TIMEOUT;
                    }
                }
                newState = backward;
            } else { //Else go forward
                if (time > (rcHigh - maxOut)) {
                    desiredPWM = PCA_TIMEOUT;
                } else {
                    desiredPWM = (long) (time - rcMid - deadzone) * PCA_TIMEOUT / (rcHigh - rcMid - deadzone);
                    if (EXPO) {
                        desiredPWM = (long) desiredPWM * desiredPWM / PCA_TIMEOUT;
                    }
                }
                newState = forward;
            }
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
