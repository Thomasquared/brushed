/* Brushed project
Main code file. 
*/

//Set your controller here
#include "../pindefs/tz85a.h"

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>

//Do you want EXPO Rates
#define EXPO 0

//Define RC States
#define unitialised  0
#define forward  1
#define backward  2
#define brake  3
 
//Define Temp States
#define HOT  2
#define WARM 1
#define COOL 0

uint8_t  state = 0;
uint16_t prevTime = 0;
uint8_t  time0 = 0;
uint8_t  failsafe = 0;
uint8_t  timeout = 50;
uint8_t  newState = 0;
uint8_t  lets_get_high = 0;
uint8_t  tempState =0;
uint8_t  rcTick = 0;
uint8_t  desiredPWM = 0;
//Setup 2 ports which we are using for H bridge. e.g.
/* Forward = B High, A Low

   Reverse = B Low,  A High
*/

#define FORWARD_LOW  LOW_A
#define FORWARD_LOW_PORT   LOW_A_PORT

#define FORWARD_HIGH HIGH_B
#define FORWARD_HIGH_PORT  HIGH_B_PORT

#define BACKWARD_LOW  LOW_B
#define BACKWARD_LOW_PORT  LOW_B_PORT

#define BACKWARD_HIGH HIGH_A
#define BACKWARD_HIGH_PORT HIGH_A_PORT

inline void setForwardLow(){
    if (LOW_ACTIVE_LOW){
        FORWARD_LOW_PORT &= ~(1 << FORWARD_LOW); 
    }
    else {
        FORWARD_LOW_PORT |= (1 << FORWARD_LOW);     
    }
}

inline void clrForwardLow(){
    if (LOW_ACTIVE_LOW){
        FORWARD_LOW_PORT |= (1 << FORWARD_LOW);
    }
    else {
        FORWARD_LOW_PORT &= ~(1 << FORWARD_LOW);
    }
}

inline void setForwardHigh(){
    if (HIGH_ACTIVE_LOW){
        FORWARD_HIGH_PORT &= ~(1 << FORWARD_HIGH);
    }
    else {
        FORWARD_HIGH_PORT |= (1 << FORWARD_HIGH);
    }
}

inline void clrForwardHigh(){
    if (HIGH_ACTIVE_LOW){
        FORWARD_HIGH_PORT |= (1 << FORWARD_HIGH);        
    }
    else {
        FORWARD_HIGH_PORT &= ~(1 << FORWARD_HIGH);
    }
}

inline void setBackwardLow(){
    if (LOW_ACTIVE_LOW){
        BACKWARD_LOW_PORT &= ~(1 << BACKWARD_LOW); 
    }
    else {
        BACKWARD_LOW_PORT |= (1 << BACKWARD_LOW);     
    }
}

inline void clrBackwardLow(){
    if (LOW_ACTIVE_LOW){
        BACKWARD_LOW_PORT |= (1 << BACKWARD_LOW);
    }
    else {
        BACKWARD_LOW_PORT &= ~(1 << BACKWARD_LOW);
    }
}

inline void setBackwardHigh(){
    if (HIGH_ACTIVE_LOW){
        BACKWARD_HIGH_PORT &= ~(1 << BACKWARD_HIGH);
    }
    else {
        BACKWARD_HIGH_PORT |= (1 << BACKWARD_HIGH);
    }
}

inline void clrBackwardHigh(){
    if (HIGH_ACTIVE_LOW){
        BACKWARD_HIGH_PORT |= (1 << BACKWARD_HIGH);        
    }
    else {
        BACKWARD_HIGH_PORT &= ~(1 << BACKWARD_HIGH);
    }
}

//Motor State Functions
void goForwards(){
    if (state != forward){
        cli();

        //Clear unused pins
        clrBackwardHigh();
        _delay_us(200); //Fet switch delay
        setForwardHigh();
        state = forward; //Up
        sei();
    }
}

void braking(){

    if (state != brake){
        cli();

        //Clear unused pins
        clrForwardHigh();
        clrBackwardHigh();
        _delay_us(200); //Fet switch delay
        state = brake;
        sei();
    }
}

void goBackwards(){
    if (state != backward){
        cli();
        //Clear high
        clrForwardHigh();
        _delay_us(200); //Fet switch delay
        setBackwardHigh();
        state = backward;
        sei();
    }
}

int main(){
    //Initialisation

    //Setup Outputs
    _delay_ms(10);
    
    clrBackwardLow();
    clrBackwardHigh();
    clrForwardLow();
    clrForwardHigh();
    
    SET_HIGH_A_PORT |= (1 << HIGH_A);
    SET_HIGH_B_PORT |= (1 << HIGH_B);
    SET_HIGH_C_PORT |= (1 << HIGH_C);

    SET_LOW_A_PORT |= (1 << LOW_A);
    SET_LOW_B_PORT |= (1 << LOW_B);
    SET_LOW_C_PORT |= (1 << LOW_C);

    _delay_ms(100);

    //Setup Timer
    TCCR0 = 0b00000011;  //Setup Timer 0 - scaling or (go check the datasheet and fill it in)
    TIMSK = (1 << TOIE0)|(1<<OCIE2)|(1<<TOIE2); //Enable Timer0 Overflow interrupt, Timer2 Output Compare and Timer2 Overflow interrupt

    //Setup PWM Timer
    OCR2 = 0xFF;  //Set the compare value to 0 or off
    TCCR2 = (1<<CS21); //Set the timer 2 scaling of (go check the datasheet and fill it in)

    //Enable RC Interrupt
    MCUCR |= (1 << ISC00);  //Set INT0 to trigger on both edges
    GICR |= (1 << INT0);    //Enable INT0
    
    newState = brake;
    braking();
    _delay_ms(100);   

    sei();  
    //End initialisation 
    
    //Main loop
    //Here we manage what mode we are in everything else is done in interrupts
    while(1){
        //RC Tick runs @ approximately 1kHz on @ 16Mhz or 500hz @ 8Mhz
        if(rcTick > 0){
		    if( state != newState){
		        if((newState != brake)&& (state != brake)){
		            //If we are not currently braking we should not be able to change direction
		            //i.e. we must go through "braking" state before we go forwards
		            //this should help reduce peak currents on reversal
		            braking();
		            _delay_ms(50);
		        } else if (newState == forward){
		            goForwards();
		        } else if (newState == backward){
		            goBackwards();
		        } else {
		            braking();
		        }
		    }
        }    
    } 
}


//This interrupt manages failsafe and time0 for upper half of RC timer (lower half is TCNT0) 
ISR(TIMER0_OVF_vect){
    //Check failsafe
    if (timeout == 0){  //If we are out of counts go to braking mode
        newState = brake;
    }
    else {
        timeout--;      //otherwise decrease failsafe counter
    }
    time0++; //time0 is upper bit for RC timer. 
    rcTick++;
}

//This interrupt manages RC pulses
ISR(INT0_vect){
    //Read in the current time
    uint16_t time = TCNT0; //Read lower
    time = time + time0*256;  //Add upper

    //Read in current time
    if(RC_PORT & ( 1 << RC_PIN)){ //If the pin is high (then its just switched on)
        prevTime = time;          //then save the current time
    }
    else {
        uint16_t time_diff = time - prevTime;
        
        //Update direction
        uint8_t deadzone = 20;
        uint8_t buffer = 20;
        uint8_t maxOut = 40;

        if (((time_diff > (RC_LOW - buffer)) && (time_diff < (RC_HIGH + buffer)))){  //Check for valid pulse
            if (failsafe > 10){   //Need 10 good pulses before we start
                failsafe = 15;    //Set valid pulse counter (15-10 = 5) therfore we can receive 5 bad pulses before we stop
                timeout = 50;     //Set timeout counter

                //Vaild signal
                if (time_diff > (RC_MID - deadzone) && time_diff < (RC_MID + deadzone)){  //If center brake
                    newState = brake;              
                } else if (time_diff < RC_MID){     //If low then go backwards
                    newState = backward;
                } else{ //Else go forward
                    newState = forward;
                }
            } else {
                failsafe++;  //If havent got 10 valid pulses yet increase count and keep waiting
            }
        } else {  //If there is an invalid pulse
            //Failsafe
            failsafe--;  //decrease counter
            if (failsafe < 10){   //If we have had enough invalid pulses
                newState = brake; //go to failsafe breaking mode
            }           
        }
    }
}

ISR(TIMER2_COMP_vect){  //When comparator is triggered clear low ports (off part of PWM cycle)
    if (OCR2 <255){
        if (state != brake){
            //Clear high
            clrForwardHigh();
            clrBackwardHigh();
        }
    }
}

ISR(TIMER2_OVF_vect ){ //When timer overflows enable low port for current state
    if (OCR2 > 0){
       if (state == forward){
           setForwardHigh();
       } else if (state == backward){
           setBackwardHigh();
       }
    }
    if (OCR2 > 250 && state != brake){
        lets_get_high++;    //Gotta keep the charge pump circuit charged 
        if (lets_get_high > 50){    //If it hasn't had a chance to charge in a while
            clrForwardHigh(); //Clear it then reset counter   
            clrBackwardHigh(); //Now pumped up and remaining high so we don't nasty shoot through which ends in magic smoke :)
            lets_get_high = 0;
        }
    }
}


