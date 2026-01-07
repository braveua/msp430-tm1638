/*
 * TM1638.cpp
 * Clean, safe implementation with delays
 */

#include "TM1638.h"

TM1638::TM1638() {
}

void TM1638::Init() {
  // Ensure pins are Outputs
  P1DIR |= (STB | DIO | CLK);
  // Idle state: STB High, CLK High
  P1OUT |= STB;
  P1OUT |= CLK;
  
  // Power On Wait
  __delay_cycles(100000); 
  
  SendCommand(DISP_ON); // 0x8F
  
  intensity=7;
  Clear();
}

void TM1638::Clear(){
  for(int i=0;i<17;i++){
	SendData(i,0);
  }
}

void TM1638::ShowSymbol(char pos, char symb, char dot){
  SendData(pos<<1, Num[(unsigned char)symb]|(dot?80:0));
}

void TM1638::On(){
  SendCommand(DISP_ON);
}

void TM1638::Off(){
  SendCommand(DISP_OFF);
}

int TM1638::GetKey() {
  unsigned int KeyData = 0;
  
  STBDOWN;
  __delay_cycles(10);
  Send(DATA_READ_KEY_SCAN_MODE); // 0x42
  
  P1DIR &= ~DIO; // Input
  P1OUT |= DIO;  // Pullup (just in case)
  
  for (char i = 0; i < 4; i++) {
        unsigned char byte = 0;
        for(int b=0; b<8; b++) {
             byte >>= 1;
             P1OUT &= ~CLK;
             __delay_cycles(10);
             if(P1IN & DIO) byte |= 0x80;
             P1OUT |= CLK;
             __delay_cycles(10);
        }
		KeyData |= byte << i; // Use raw logic for now
        // Note: TM1638 sends 4 bytes. 
	}
    
  P1DIR |= DIO; // Output
  STBUP;
  
  return KeyData;
}

void TM1638::ShowLed(int Number, int Color)
{
  SendData((Number << 1)-1, Color);
}

// Low-level bit bang
// LSB First
void TM1638::Send(unsigned char data){ 
  for(int i=0;i<8;i++){
    P1OUT &= ~CLK; // Fall
    __delay_cycles(10);
    
    if(data & 1) P1OUT |= DIO;
    else P1OUT &= ~DIO;
    data >>= 1;
    
    __delay_cycles(10); 
    P1OUT |= CLK; // Rise
    __delay_cycles(10); 
  }
}

// Function Read() is inlined in GetKey for now to avoid linking errors or complexity
// But we need it if referenced elsewhere.
char TM1638::Read(){
    unsigned char byte = 0;
    P1DIR &= ~DIO; 
    P1OUT |= DIO; 
    for(int b=0; b<8; b++) {
         byte >>= 1;
         P1OUT &= ~CLK;
         __delay_cycles(10);
         if(P1IN & DIO) byte |= 0x80;
         P1OUT |= CLK;
         __delay_cycles(10);
    }
    P1DIR |= DIO; 
    return byte;
}

void TM1638::SendData(unsigned char addr, unsigned char data){ 
	SendCommand(DATA_WRITE_FIX_ADDR); // 0x44
    
    // Single transaction for Address + Data
	STBDOWN;
    __delay_cycles(10);
	Send(addr|0xC0); // Address command
	Send(data);      // Data
	STBUP;
    __delay_cycles(10);
}

void TM1638::SendCommand(unsigned char data){ 
  STBDOWN;
  __delay_cycles(10);
  Send(data);
  STBUP;
  __delay_cycles(10);
}
