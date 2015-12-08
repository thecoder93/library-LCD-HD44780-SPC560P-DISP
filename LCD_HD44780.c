/*
 ChibiOS/RT - Copyright (C) 2006-2014 Giovanni Di Sirio

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

/* Inclusion of the main header files of all the imported components in the
 order specified in the application wizard. The file is generated
 automatically.*/
#include "components.h"

#define CMD_CLR 0x01
#define CMD_HOME 0x02


int g_num_lines = 2;

void pulseEnablePin(void)
{

    palClearPad(PORT_D, PIN_EN_LCD);
	osalThreadSleepMicroseconds(1); //wait
	//send a pulse to enable
	palSetPad(PORT_D, PIN_EN_LCD);
	osalThreadSleepMicroseconds(5); //wait
    palClearPad(PORT_D, PIN_EN_LCD);
}

void digitalWriteG(int pin, uint8_t value)
{
	if(value == 1)
	{
		palSetPad(PORT_G, pin);
	}
	else
	{
		palClearPad(PORT_G, pin);
	}
}


void pushNibble(uint8_t value)
{

	//clear value E' un and che mette i primi 4
	//bit a 0 e gli altri 4 ad 1 in modo da prendere gli ultimi.
	//Si vede dalla tabella di verità dell'AND
	uint8_t val_nibble = value;
	int i=0;
	for(i=0; i<4; i++)
	{
		digitalWriteG(DB[i], val_nibble & 0x01);
		val_nibble >>= 1;
	}

	pulseEnablePin();

}

void pushByte(uint8_t value){
  uint8_t val_lower = value & 0x0F;
  uint8_t val_upper = value >> 4;
  val_upper = val_upper & 0x0F;
  pushNibble(val_upper);
  pushNibble(val_lower);
}


void commandWriteNibble(uint8_t nibble) {
  palClearPad(PORT_D, PIN_RS);
  palClearPad(PORT_D, PIN_RW);
  pushNibble(nibble);
}

void commandWrite(uint8_t value) {
	  palClearPad(PORT_D, PIN_RS);
	  palClearPad(PORT_D, PIN_RW);
  pushByte(value);
  //chThdSleepMilliseconds(5);
}


void cursorTo(int line_num, int x){
  //first, put cursor home
  commandWrite(CMD_HOME);

  //if we are on a 1-line display, set line_num to 1st line, regardless of given
  if (g_num_lines==1){
    line_num = 1;
  }
  //offset 40 chars in if second line requested
  if (line_num == 2){
    x += 40;
  }
  int i;
  //advance the cursor to the right according to position. (second line starts at position 40).
  for (i=0; i<x; i++) {
    commandWrite(0x14);
  }
}

void printIn(char msg[]) {
  uint8_t i;  //fancy int.  avoids compiler warning when comparing i with strlen()'s uint8_t

  for (i=0; i<strlen(msg);i++)
  {
	  if(i<=14)
	  {
	  	print(msg[i]);
	  }
	  else if(i==15)
	  {
	  	printIn("-");
	  	cursorTo(2,0);
	  	print(msg[i]);
	  }
	  else
	  	print(msg[i]);
  }

}

void clear(void){
  commandWrite(CMD_CLR);
  chThdSleepMilliseconds(1);
}

void initLCD(void)
{
	//The first 4 nibbles and timings are not in my DEM16217 SYH datasheet, but apparently are HD44780 standard...


	chThdSleepMilliseconds(100);
	pulseEnablePin();

	//Switch to 4-bit mode
		commandWriteNibble(0x03);
		chThdSleepMilliseconds(5);
		commandWriteNibble(0x03);
		chThdSleepMicroseconds(200);
		commandWriteNibble(0x03);
		chThdSleepMicroseconds(200);



	    commandWriteNibble(0x20);
		chThdSleepMicroseconds(200);

	    commandWriteNibble(0x02);

	    commandWriteNibble(0x08);
		chThdSleepMicroseconds(100);


	    commandWriteNibble(0x00);


	    commandWriteNibble(0x08);
		chThdSleepMicroseconds(100);


	    commandWriteNibble(0x00);


	    commandWriteNibble(0x01);
		chThdSleepMilliseconds(4);


	    commandWriteNibble(0x00);


	    commandWriteNibble(0x06);
		chThdSleepMicroseconds(100);

}
