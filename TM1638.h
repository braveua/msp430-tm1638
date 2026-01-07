/*
 * TM1638.h
 *
 *  Created on: 22.12.2013
 *      Author: brave
 */
#include <msp430g2553.h>
#ifndef TM1638_H_
#define TM1638_H_

#define STBUP   P1OUT|=STB;
#define STBDOWN P1OUT &= ~(STB);
#define DIOOUT  P1DIR|=DIO;
#define DIOIN   P1DIR &= ~(DIO);

#define STB BIT5 //Strobe
#define DIO BIT2 //Data in/out
#define CLK BIT4 //clock

#define RED_TM1638 0x02 //RED Led
#define GRE_TM1638 0x01 //Green Led

#define DATA_WRITE_INCR_ADDR 0x40 //Command to switch TM1638 for automatic increment address mode
#define DATA_WRITE_FIX_ADDR 0x44 //Command to switch TM1638 for fix address mode
#define DATA_READ_KEY_SCAN_MODE 0x42 //Command for read key code from TM1638
#define ADDRSET 0xC0 //Command to set address 0x00
#define DISP_ON 0x8F //Command to Display ON and set (max) brightness
#define DISP_OFF 0x80 //Command to Display OFF

const unsigned int Num[]= //Code table of symbols
{
0x3F, //0
0x06, //1
0x5B, //2
0x4F, //3
0x66, //4
0x6D, //5
0x7D, //6
0x07, //7
0x7F, //8
0x6F, //9
0x40, //Minus
0x63, //Degree
0x39, //"C"
0x00, //Blank
0x3E //"U"
};

const unsigned int ERROR_DATA[] = {
0x79, // E
0x50, // r
0x50, // r
0x5C, // o
0x50, // r
0,
0,
0
};



class TM1638 {
public:
	TM1638();
    void Init();
	void ShowSymbol(char pos, char symb, char dot);
	void ShowLed(int Number, int Color);
	int GetKey();
	void On();
	void Off();
	void Clear();

private:
	void Send(unsigned char data);
	char Read();
	void SendData(unsigned char addr, unsigned char data);
	void SendCommand(unsigned char data);
	char intensity;
};




#endif /* TM1638_H_ */
