/*
 * main.cpp
 * Working Clock Firmware
 */
#include <msp430.h>
#include "TM1638.h"

#define RED_LED BIT0
#define GREEN_LED BIT6

// Volatile State for Interrupts
volatile int sec32=0;
volatile int halfSeconds=0;
volatile int currentMinutes=0;
volatile int currentSeconds=0;
volatile int currentHour=0;

// Update Flags
volatile bool sflag=true;
volatile bool mflag=true;
volatile bool hflag=true;
volatile bool hsflag=true;

// Function Prototypes
void InitSystem();
void ShowClock(TM1638& disp);

int main(void) {
    // 1. Core Init
    WDTCTL = WDTPW + WDTHOLD; 
    
    // 2. Visual Alive Check (Fast Red Blink)
    P1DIR |= RED_LED | GREEN_LED;
    P1OUT &= ~GREEN_LED;
    for(int i=0; i<5; i++) {
        P1OUT ^= RED_LED;
        __delay_cycles(50000);
    }
    P1OUT &= ~RED_LED;
    
    // 3. Setup Clock (DCO + Crystal)
    if (CALBC1_1MHZ==0xFF) while(1);
    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    // Use external Crystal 32768Hz
	BCSCTL1 |= DIVA_3;				// ACLK/8
	BCSCTL3 |= XCAP_0;				// XCAP_0 (1pf)
    
    // Wait for crystal to stabilize
    for(int i=0; i<1000; i++) {
        IFG1 &= ~OFIFG;
        __delay_cycles(100);
    }
    
    // 4. Instantiate Display (Local Scope)
    TM1638 disp;
    
    // 5. Init Display
    disp.Init(); 
    
    // 6. Setup Timer
	CCTL0 = CCIE;					// CCR0 interrupt enabled
	CCR0 = 15;						// Period 16. Freq = 32768/8/8/16 = 32Hz.
	TACTL = TASSEL_1 + ID_3 + MC_1;	// ACLK, /8, upmode
	__enable_interrupt();
    
    // 7. Initial Render
    ShowClock(disp);

    // 8. Main Loop
	while(1){
        // Update display if needed
		ShowClock(disp);
        
        // Read Buttons
		int buttons = disp.GetKey();
        
        // Show buttons on LEDs
		for(int i=0;i<8;i++){
		    if(buttons&(1<<i)) disp.ShowLed(i+1, RED_TM1638);
		    else disp.ShowLed(i+1, 0);
		}

        // Button Logic
	    if(buttons&(1<<0)) { // Hour -
		  currentHour--;
		  if(currentHour<0) currentHour=23;
		  hflag=mflag=sflag=hsflag=true;
		  disp.ShowLed(1,GRE_TM1638);
          // Simple debounce delay instead of blocking while loop
		  __delay_cycles(200000); 
	    }
        if(buttons&(1<<1)) { // Hour +
    	  currentHour++;
    	  if(currentHour>23) currentHour=0;
    	  hflag=mflag=sflag=hsflag=true;
    	  disp.ShowLed(2,GRE_TM1638);
    	  __delay_cycles(200000);
        }
        if(buttons&(1<<3)) { // Min +
    	  currentMinutes++;
    	  if(currentMinutes>59) currentMinutes=0;
    	  mflag=sflag=hsflag=true;
    	  disp.ShowLed(4,GRE_TM1638);
    	  __delay_cycles(200000);
        }
        if(buttons&(1<<2)) { // Min -
          currentMinutes--;
          if(currentMinutes<0) currentMinutes=59;
          mflag=sflag=hsflag=true;
          disp.ShowLed(3,GRE_TM1638);
          __delay_cycles(200000);
        }
   }
}

void ShowClock(TM1638& disp){
	if(hsflag) {
        // Blink dots at pos 2 and 5
		disp.ShowSymbol(5,(halfSeconds & 1)?13:10,0);
		disp.ShowSymbol(2,(halfSeconds & 1)?13:10,0);
		hsflag=false;
	}
	if(sflag) {
		disp.ShowSymbol(7,(currentSeconds) % 10,0);
		disp.ShowSymbol(6,(currentSeconds) / 10,0);
		sflag=false;
	}
	if(mflag) {
		disp.ShowSymbol(4,currentMinutes % 10,0);
		disp.ShowSymbol(3,currentMinutes / 10,0);
		mflag=false;
	}
	if(hflag) {
		disp.ShowSymbol(1,currentHour % 10,0);
		disp.ShowSymbol(0,currentHour / 10,0);
		hflag=false;
	}
}


// Timer A0 interrupt service routine
#if defined(__GNUC__)
__attribute__((interrupt(TIMER0_A0_VECTOR)))
void Timer_A(void)
#elif defined(__TI_COMPILER_VERSION__) || defined(__MSP430__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#else
void Timer_A(void)
#endif
{
	sec32++;

	if(sec32==16){ // 0.5 sec
		halfSeconds++;
		P1OUT ^= GREEN_LED; // Blink Green LED on board
		sec32=0;
		hsflag=true;
		sflag=true;
	}

	if(halfSeconds==120) { // 60 seconds
		halfSeconds=0;
		currentMinutes++;
		mflag=true;
	}
	currentSeconds=halfSeconds/2;

	if(currentMinutes==60){
		currentMinutes=0;
		currentHour++;
		hflag=true;
	}

	if(currentHour==24){
			currentHour=0;
	}
}
