#ifdef SYSCLK
#include <intrins.h>

//Bob Horeck, 2002, keil forum

// NOTE: "Crystal" as used below is best defined in some other header unique to your project.

unsigned char Delay8Plus2xCycles (unsigned char x);


#define NanoSecondsPerCycle (1000000000L / SYSCLK)
#define uSecToInstCycles(x) ((1000L * (x)) / NanoSecondsPerCycle)

#define uSecDelay(InstCycles) \ 
     (((InstCycles) &  1) ? _nop_() : 0), \ 
     (((InstCycles) &  2) ? _nop_(),_nop_() : 0), \ 
     (((InstCycles) & ~7) ? Delay8Plus2nCycles(((InstCycles/4)-2)*2) \ 
     : (((InstCycles) & 4) ? _nop_(),_nop_(),_nop_(),_nop_() : 0))


unsigned char Delay8Plus2nCycles (unsigned char LoopCount)
{
    while (--LoopCount);
    return (LoopCount);
}

void delay_ms(unsigned long loop){
	while (loop > 1){
		uSecDelay (uSecToInstCycles(200));
		uSecDelay (uSecToInstCycles(200));
		uSecDelay (uSecToInstCycles(200));
		uSecDelay (uSecToInstCycles(200));
		uSecDelay (uSecToInstCycles(200));
		loop--;
	}
} 
#endif

