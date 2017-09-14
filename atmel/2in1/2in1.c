/* Brushed project
2 motors off one ESC code file. 
*/
//Set your controller here
#include "../pindefs/plush10.h"

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#include <avr/eeprom.h>
#include "uart.h"

#define unitialised  0
#define forward  1
#define backward  2
#define brake  3
#define spinLeft 4
#define spinRight 5

void send_uint16_t(uint16_t data){
    uint8_t count = 0;   
    while (data >= 10000){
        count++; 
        data = data - 10000;   
    }
    uart_putc(48+count);
    count = 0;
    while (data >= 1000){
        count++;    
        data = data - 1000;
    }
    uart_putc(48+count);
    count = 0;
    while (data >= 100){
        count++;
        data = data - 100;    
    }
    uart_putc(48+count);
    count = 0;
    while (data >= 10){
        count++;    
        data = data - 10;
    }
    uart_putc(48+count);
    uart_putc(48+data);
}

volatile uint8_t  state = 0;
volatile uint8_t  newState = 0;
volatile uint16_t prevTime = 0;
volatile uint8_t  time0 = 0;
volatile uint8_t  failsafe = 0;
volatile uint8_t  timeout = 50;
volatile uint16_t timeCH1 = 0;
volatile uint16_t timeCH2 = 0;
volatile uint16_t timeCH3 = 0;
volatile uint8_t  lets_get_high = 0;
volatile int16_t  CH1 = 0;
volatile int16_t  CH2 = 0;
volatile uint8_t  rcTick = 0;
volatile uint8_t  newData1 = 0;
volatile uint8_t  newData2 = 0;
volatile uint8_t  chCount = 0;
volatile uint8_t toggle = 0;

#define LOW_PORT_A LOW_A_PORT
#define LOW_PORT_B LOW_B_PORT 
#define LOW_PORT_C LOW_C_PORT

#define HIGH_PORT_A HIGH_A_PORT 
#define HIGH_PORT_B HIGH_B_PORT 
#define HIGH_PORT_C HIGH_C_PORT

#define ABS(a)	   (((a) < 0) ? -(a) : (a))

inline void setALow();
inline void clrALow();
inline void setAHigh();
inline void clrAHigh();
inline void setBLow();
inline void clrBLow();
inline void setBHigh();
inline void clrBHigh();
inline void setCLow();
inline void clrCLow();
inline void setCHigh();
inline void clrCHigh();


//Motor States
void goForwards();
void braking();
void goBackwards();
void goSpinLeft();
void goSpinRight();

uint16_t isqrt (uint16_t x){
   int16_t squaredbit, remainder, root;

   if (x<1) return 0;
  
   /* Load the binary constant 01 00 00 ... 00, where the number
    * of zero bits to the right of the single one bit
    * is even, and the one bit is as far left as is consistant
    * with that condition.)
    */
   squaredbit  = (int16_t) ((((uint16_t) ~0L) >> 1) & 
                        ~(((uint16_t) ~0L) >> 2));
   /* This portable load replaces the loop that used to be 
    * here, and was donated by  legalize@xmission.com 
    */

   /* Form bits of the answer. */
   remainder = x;  root = 0;
   while (squaredbit > 0) {
     if (remainder >= (squaredbit | root)) {
         remainder -= (squaredbit | root);
         root >>= 1; root |= squaredbit;
     } else {
         root >>= 1;
     }
     squaredbit >>= 2; 
   }

   return root;
}



//Motor State Functions
int main(){
    //Initialisation
    uart_init( UART_BAUD_SELECT(38400,F_CPU) );
    //Setup Outputs
    _delay_ms(10);
    SET_HIGH_A_PORT |= (1 << HIGH_A);
    SET_HIGH_B_PORT |= (1 << HIGH_B);
    SET_HIGH_C_PORT |= (1 << HIGH_C);

    SET_LOW_A_PORT |= (1 << LOW_A);
    SET_LOW_B_PORT |= (1 << LOW_B);
    SET_LOW_C_PORT |= (1 << LOW_C);
    _delay_ms(100);

    clrALow();
    clrAHigh();
    clrBLow();
    clrBHigh();
    clrCLow();
    clrCHigh();
 
    RC_PORT &= ~(1 << RC_PIN);

    //Setup Timer
    TCCR0 = 0b00000011;  //Setup Timer 0 - scaling of ???? (go check the datasheet and fill it in)
    TIMSK = (1 << TOIE0)|(1<<OCIE1A)|(1<<OCIE1B)|(1<<TOIE1); //Enable Timer0 Overflow interrupt, Timer1 Output Compare A/B and Timer1 Overflow interrupt

    //Setup PWM Timer 
	TCCR1A = (1 << WGM10); //WGM bits set it to 8 bit fast PWM mode 
	TCCR1B = (1 << WGM12)|(1 << CS11); //CS11 Sets prescaler to 8 ~ 4khz @ 8Mhz

	OCR1A = 100;
	OCR1B = 128;

    //Enable RC Interrupt
    MCUCR |= (1 << ISC00) | (1 << ISC10);  //Set INT0 to trigger on both edges
    GICR |= (1 << INT0);//|(1 << INT1);    //Enable INT0 & INT1
    _delay_ms(100);
     sei();
    //End initialisation 
     
    sei();
    uart_puts("Running\n");
    //Main loop
    //Here we manage what mode we are in everything else is done in interrupts
    while(1){         
        //_delay_ms(50);
        
        const uint8_t deadzone = 20;
    	const uint8_t buffer = 20;              

        if(newData1 == 1 && newData2 == 1){
            //Send Data

            //valid pulse
            if ((timeCH1 > (RC_LOW - buffer)) && (timeCH1 < (RC_HIGH + buffer)) && (timeCH2 > (RC_LOW - buffer)) && (timeCH2 < (RC_HIGH + buffer))){
                failsafe++;
                
                if (failsafe > 10){   //Need 10 good pulses before we start
                    failsafe = 15;    //Set valid pulse counter (15-10 = 5) therfore we can receive 5 bad pulses before we stop
                    timeout = 50;     //Set timeout counter

                }
            }
            else {
                failsafe--;
            }
            newData1 = 0; newData2 = 0;
        }

        if(failsafe > 10 && timeout > 0){
            if (rcTick > 0){
                //goBackwards();
                //Need to handle states here 

				//Manage Slew
				if (ABS(timeCH1 - CH1) > MAX_SLEW){
					if (timeCH1 > CH1){
						CH1 = CH1 + MAX_SLEW;
					}
					else {
						CH1 = CH1 - MAX_SLEW;
					}
				} else {
					CH1 = timeCH1;
				}
				if (ABS(timeCH2 - CH2) > MAX_SLEW){
					if (timeCH2 > CH2){
						CH2 = CH2 + MAX_SLEW;
					}
					else {
						CH2 = CH2 - MAX_SLEW;
					}
				} else {
					CH2 = timeCH2;
				}
                //CH1 = timeCH1;
                //CH2 = timeCH2;
				//Calc Norms
		        int16_t normSteer = CH1 - RC_MID;
        		int16_t normForward = CH2 - RC_MID;	
		        //Work out the magnitude of the RC signal.
		        uint16_t squared = normForward*normForward + normSteer*normSteer; //this gives a squared distance up to 15 bits. 
		        //SQRT for linear or just scale it to keep exponential rates
		        int16_t magnitude = isqrt(squared)*255/(RC_MID - RC_LOW); //Scale to 8 bit from 15.
                int16_t angle = 10000 *atan2(normForward,normSteer);

                if (magnitude > 255){
                    magnitude = 255;
                }		        
				
				const uint16_t HYSTERESIS = 500;
				//Manage State Hysteresis
				if ((angle >= 7854) && (angle <= 23462)){
					//Forward
                    //newState = forward;
					if (state != forward) {
						if (((angle >= (7854+HYSTERESIS)) && (angle <= (23562-HYSTERESIS))) || state == brake){
							newState = forward;
						}
					}
				} else if ((angle <= 7854) && (angle >= -7854)){
					//Right
                   // newState = spinRight;
					if (state != spinRight) {
						if (((angle >= (7854-HYSTERESIS)) && (angle <= (-7854+HYSTERESIS))) || state == brake){
							newState = spinRight;
						}
					}
				} else if ((angle <= -7854) && (angle >= -23562)){
					//Back
                    //newState = backward;
					if (state != backward) {
						if (((angle >= (-7854-HYSTERESIS)) && (angle <= (-23562+HYSTERESIS))) || state == brake){
							newState = backward;
						}
					}					
				} else {
					//Left (has +/- wrap around)
                    //newState = spinLeft;
					if (state != spinLeft) {
						if (angle > 0){
							if ((angle >= (23562+ HYSTERESIS)) || state == brake){
								newState = spinLeft;
							}
						}
						else {							
							if ((angle <= (-23562+HYSTERESIS)) || state == brake){
								newState = spinLeft;
							}
						}
					}					
				}

                //If we are in the center
                if(magnitude < deadzone){
                    braking();
                } else {  	                
                    //Back Forward Mode
                    if (newState == forward){
                        goForwards();                        
                            int16_t temp = magnitude - (normSteer*magnitude/(RC_MID - RC_LOW));
                            if (temp < 0){
                                temp = 10
                            } else if (temp > 255){
                                temp = 255;
                            }
                            OCR1A = temp ;
                            temp = magnitude + (normSteer*magnitude/(RC_MID - RC_LOW));
                            if (temp < 0){
                                temp = 10
                            } else if (temp > 255){
                                temp = 255;
                            }
                            OCR1B = temp;

                    
                    }
                    else if (newState == backward) {
                        goBackwards();
                            int16_t temp = magnitude + (normSteer*magnitude/(RC_MID - RC_LOW));
                            if (temp < 0){
                                temp = 10
                            } else if (temp > 255){
                                temp = 255;
                            }
                            OCR1A = temp ;
                            temp = magnitude - (normSteer*magnitude/(RC_MID - RC_LOW));
                            if (temp < 0){
                                temp = 1-
                            } else if (temp > 255){
                                temp = 255;
                            }
                            OCR1B = temp;    
                    }
                    else {
                        if (newState == spinRight){
                            goSpinRight();  
                            int16_t temp = magnitude/2 + (normForward*magnitude/(RC_MID - RC_LOW)/2);
                            if (temp < 0){
                                temp = 10
                            } else if (temp > 255){
                                temp = 255;
                            }
                            OCR1A = temp;     
                            OCR1B = magnitude;
                        }
                        else {;
                            goSpinLeft();  
                            int16_t temp = magnitude/2 - (normForward*magnitude/(RC_MID - RC_LOW)/2);
                            if (temp < 0){
                                temp = 10;
                            } else if (temp > 255){
                                temp = 255;
                            }
                            OCR1A = temp ;
                            OCR1B = magnitude;
                        }
                    }
                }                
                rcTick = 0; 
            }
        }
        else {
           braking();
        }
              
    }  
}

//This interrupt manages failsafe and time0 for upper half of RC timer (lower half is TCNT0) 
ISR(TIMER0_OVF_vect){
    //Check failsafe
    if (timeout > 0){  //If we are out of counts go to braking mode
        timeout--;      //otherwise decrease failsafe counter
    }
    time0++; //time0 is upper bit for RC timer.
    rcTick++; 

}


ISR(INT0_vect){
    //Read in the current time
    uint16_t time = TCNT0; //Read lower
    time = time + time0*256;  //Add upper
    
    if ((RC_PORT & ( 1 << RC_PIN))){
            chCount++;        
    }
    else {
        uint16_t tempTime = time - prevTime;
        if (tempTime > F_CPU/8000){
            chCount = 0;
        } else if (chCount ==1){
            timeCH1 = tempTime;       
            newData1 = 1;
            //
        } else if (chCount == 2) { 
            timeCH2 = tempTime;            
            newData2 = 1;
        } else if (chCount == 3) { 
            timeCH3 = tempTime;            
        } 
    } 
    prevTime = time;          //then save the current time       
}


ISR(TIMER1_COMPA_vect ){
    if (state == forward){
        //A trigger
        clrCHigh();
    }
    else if (state == backward){
        //A trigger
        clrCLow();
    }
    else if (state == spinLeft){
        clrBHigh();
        _delay_us(10);
        setBLow();
    }
    else if (state == spinRight){
        clrBLow();
        _delay_us(10);
        setBHigh();
    } 
}

ISR(TIMER1_COMPB_vect){     
	if (state == forward){
        //B trigger
        clrAHigh();
    }
    else if (state == backward){
        //B trigger
        clrALow();
    }
    else if (state == spinLeft){
        clrCHigh();
    }
    else if (state == spinRight){
        clrCLow();
    }
}

ISR(TIMER1_OVF_vect){
    if (state == backward){
        setALow();
        setBHigh();
        setCLow();
        
        lets_get_high++; //Gotta keep the charge pump circuit charged
        if (lets_get_high > 100) { //If it hasn't had a chance to charge in a while           
            clrBHigh();
            lets_get_high = 0;
        }
    }
    else if (state == forward){
        setAHigh();
        setBLow();
        setCHigh();
        
        if ((OCR1A > 250) || (OCR1B > 250)){   
            lets_get_high++; //Gotta keep the charge pump circuit charged
            if (lets_get_high > 100) { //If it hasn't had a chance to charge in a while           
                clrAHigh();
                clrCHigh();
                lets_get_high = 0;
            }
        }
    }

    else if (state == spinLeft){
        setALow();
        clrBLow();
        _delay_us(10);
        setBHigh();
        setCHigh();
        if ((OCR1A > 250) || (OCR1B > 250)){   
            lets_get_high++; //Gotta keep the charge pump circuit charged
            if (lets_get_high > 100) { //If it hasn't had a chance to charge in a while           
                clrBHigh();
                clrCHigh();
                lets_get_high = 0;
            }
        }
   }
   else if (state == spinRight){
        setAHigh();
        clrBHigh();
        _delay_us(10);
        setBLow();
        setCLow();
        if ((OCR1A < 5) || (OCR1B > 250)){   
            lets_get_high++; //Gotta keep the charge pump circuit charged
            if (lets_get_high > 100) { //If it hasn't had a chance to charge in a while           
                clrBHigh();
                clrCHigh();
                lets_get_high = 0;
            }
        }
    }
}


//Motor Pin functions
inline void setALow(){
    if (LOW_ACTIVE_LOW){
        LOW_A_PORT &= ~(1 << LOW_A); 
    }
    else {
        LOW_A_PORT |= (1 << LOW_A);     
    }
}

inline void clrALow(){
    if (LOW_ACTIVE_LOW){
        LOW_A_PORT |= (1 << LOW_A); 
    }
    else {
        LOW_A_PORT &= ~(1 << LOW_A);
    }
}

inline void setAHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_A_PORT &= ~(1 << HIGH_A);
    }
    else {
        HIGH_A_PORT |= (1 << HIGH_A);
    }
}



inline void clrAHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_A_PORT |= (1 << HIGH_A);
    }
    else {
        HIGH_A_PORT &= ~(1 << HIGH_A);
    }
}

inline void setBLow(){
    if (LOW_ACTIVE_LOW){
        LOW_B_PORT &= ~(1 << LOW_B); 
    }
    else {
        LOW_B_PORT |= (1 << LOW_B);     
    }
}

inline void clrBLow(){
    if (LOW_ACTIVE_LOW){
        LOW_B_PORT |= (1 << LOW_B); 
    }
    else {
        LOW_B_PORT &= ~(1 << LOW_B);
    }
}

inline void setBHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_B_PORT &= ~(1 << HIGH_B);
    }
    else {
        HIGH_B_PORT |= (1 << HIGH_B);
    }
}

inline void clrBHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_B_PORT |= (1 << HIGH_B);
    }
    else {
        HIGH_B_PORT &= ~(1 << HIGH_B);
    }
}

inline void setCLow(){
    if (LOW_ACTIVE_LOW){
        LOW_C_PORT &= ~(1 << LOW_C); 
    }
    else {
        LOW_C_PORT |= (1 << LOW_C);     
    }
}

inline void clrCLow(){
    if (LOW_ACTIVE_LOW){
        LOW_C_PORT |= (1 << LOW_C); 
    }
    else {
        LOW_C_PORT &= ~(1 << LOW_C);
    }
}

inline void setCHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_C_PORT &= ~(1 << HIGH_C);
    }
    else {
        HIGH_C_PORT |= (1 << HIGH_C);
    }
}

inline void clrCHigh(){
    if (HIGH_ACTIVE_LOW){
        HIGH_C_PORT |= (1 << HIGH_C);
    }
    else {
        HIGH_C_PORT &= ~(1 << HIGH_C);
    }
}


//Motor States
void goForwards(){
    cli();
    if (state != forward){
        //Clear unused pins
        clrALow();
        clrBHigh();
        clrCLow();

        _delay_us(200); //Fet switch delay

        setBLow(); //Enable Common pin
	
	    _delay_us(200); //Fet switch delay
	    setAHigh(); //Enable one motor	
    	//Both motors are enabled at slightly different times
    	//to hopefully be a little bit nicer to the fets
    	//200us should have no noticible effect on driving straight
        _delay_us(200); //Fet switch delay
        setCHigh(); //Enable 2nd motor

        state = forward; //Up
    }
    sei();
}


void braking(){
    cli();
    if (state != brake){
        //Clear unused pins
        clrAHigh();
        clrBHigh();
        clrCHigh();

        _delay_us(200); //Fet switch delay
	    setALow();
        setBLow();
        setCLow();

        state = brake;
    }
    sei();
}




void goBackwards(){
    cli();
    if (state != backward){

        //Clear unused pins
        clrAHigh();
        clrBLow();
        clrCHigh();

        _delay_us(200); //Fet switch delay
        setBHigh(); //Enable Common pin
	
	    _delay_us(200); //Fet switch delay
	    setALow(); //Enable one motor
	
	    //Both motors are enabled at slightly different times
	    //to hopefully be a little bit nicer to the fets
	    //200us should have no noticible effect on driving straight
        _delay_us(200); //Fet switch delay
        setCLow(); //Enable 2nd motor
	
        state = backward;
    }
    sei();
}

void goSpinLeft(){
    cli();
    if (state != spinLeft){

        //Clear unused pins
        clrAHigh();
        clrBLow();
        clrBHigh();
        clrCLow();

        _delay_us(200); //Fet switch delay
        setALow(); //Enable Common pin
	
	    _delay_us(200); //Fet switch delay
	    setCHigh(); //Enable one motor
	
        state = spinLeft;
    }
    sei();
}

void goSpinRight(){
    cli();
    if (state != spinRight){

        //Clear unused pins
        clrALow();
        clrBLow();
        clrBHigh();
        clrCHigh();

        _delay_us(200); //Fet switch delay
        setAHigh(); //Enable Common pin
	
	    _delay_us(200); //Fet switch delay
	    setCLow(); //Enable one motor
	
        state = spinRight;
    }
    sei();
}


//This interrupt manages RC pulses
/*ISR(INT0_vect){
    //Read in the current time
    uint16_t time = TCNT0; //Read lower
    time = time + time0*256;  //Add upper

    if (RC_PORT & ( 1 << RC_PIN)){
        prevTime = time;          //then save the current time        
    }
    else {
        timeCH1 = time - prevTime;
        newData1 = 1;        
    }      
}

ISR(INT1_vect){
    //Read in the current time
    uint16_t time = TCNT0; //Read lower
    time = time + time0*256;  //Add upper

    if (RC_PORT & ( 1 << RC_PIN2)){
        prevTime1 = time;          //then save the current time        
    }
    else {
        timeCH2 = time - prevTime1; 
        newData2 = 1;        
    }      
}*/
