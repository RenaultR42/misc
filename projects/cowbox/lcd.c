#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <time.h>
#include "lcd.h"

/* Reminder, bits order in one byte for each GPIO:
 * ENABLE, RS D4 D5 D6 D7 */

/* Cursor go back to begin of the screen */
void backToBegin(gpio pins[], int nbPins, int duration)
{
	unsigned char data = 0b000000;

	refreshLCD(data, pins, nbPins, duration);
	data = 0b000100;
	refreshLCD(data, pins, nbPins, duration);
}

/* Necessary steps to validate a byte of data on the screen and display */
void refreshLCD(unsigned char data, gpio pins[], int nbPins, int duration)
{
	changeStateGPIO(pins, data, nbPins);
	pulsePin(pins, ENABLE, duration);
	usleep(duration);
}

/* Write character on the screen which is in 4 bits mode. So it's necessary to take half of the character and do that twice.
 * Characters are in ASCII */
unsigned char writeChar(char c, int section)
{
	char splitChar[2];
	unsigned char ret;

	/* Get hexa value of one byte (character) in form XX which is two characters. One half should be used */
	sprintf(splitChar, "%x", c);

	/* Hexa value and associated data bits for GPIO with RS bit set to 1 (because it's a character data) */
	switch (splitChar[section]) {
	case '0':
		ret = 16; 
		break; 

	case '1': 
		ret = 24;
		break;

	case '2': 
		ret = 20;
		break;

	case '3': 
		ret = 28; 
		break;

	case '4': 
		ret = 18; 
		break;

	case '5': 
		ret = 26;
		break;

	case '6':
		ret = 22;
		break;

	case '7': 
		ret = 30;
		break;

	case '8': 
		ret = 17; 
		break;

	case '9': 
		ret = 25; 
		break;

	case 'A':
	case 'a': 
		ret = 21; 
		break;

	case 'B':
	case 'b':
		ret = 29;
		break;

	case 'C':
	case 'c':
		ret = 19; 
		break;

	case 'D':
	case 'd':
		ret = 27; 
		break;

	case 'E':
	case 'e': 
		ret = 23; 
		break;

	case 'F':
	case 'f':
		ret = 31; 
		break;

	default:
		printf("Character invalid\n");
	}

	return ret;
}

/* Write one character on the screen, get it in 4 bits mode */
void charToLCD(char c, gpio pins[], int nbPins)
{
	unsigned char data;

	data = writeChar(c, 0);
	refreshLCD(data, pins, nbPins, DURATION);

	data = writeChar(c, 1);
	refreshLCD(data, pins, nbPins, DURATION);
}


/* Init LCD screen, see the datasheet for steps */
void initLCD(gpio pins[], int nbPins)
{
	unsigned char data;
	int i;

	initGPIO(pins, nbPins);

	for (i = 0; i < STEPS_INIT; i++) {
		switch (i) {
		case 0:
			data = 0b001100;
			break;

		/* 4 bits mode, because we have only 4 data pins */
		case 1:
			data = 0b000100;
			break;

		/* Both following cases should display blink cursor */
		case 2:
			data = 0b000000;
			break;

		case 3:
			data = 0b001111;
			break;

		/* Both following cases should display in 4 lines mode */
		case 4:
			data = 0b000100;
			break;

		case 5:
			data = 0b000011;
			break;

		default:
			printf("Error during screen init\n");
		}

		refreshLCD(data, pins, nbPins, DURATION);
	}
}

void enableBlinkCursor(gpio pins[], int nbPins)
{
	unsigned char data;

	data = 0b000000;
	refreshLCD(data, pins, nbPins, DURATION);

	data = 0b001111;
	refreshLCD(data, pins, nbPins, DURATION);
}

void disableBlinkCursor(gpio pins[], int nbPins)
{
	unsigned char data;

	data = 0b000000;
	refreshLCD(data, pins, nbPins, DURATION);

	data = 0b000011;
	refreshLCD(data, pins, nbPins, DURATION);
}

void clearScreen(gpio pins[], int nbPins)
{
	unsigned char data;

	data = 0b000000; 
	refreshLCD(data, pins, nbPins, DURATION);

	data = 0b001000; 
	refreshLCD(data, pins, nbPins, DURATION);
}

void shutdownLCD(gpio pins[], int nbPins)
{
	unsigned char data;

	data = 0b001100; 
	refreshLCD(data, pins, nbPins, DURATION);

	deleteGPIO(pins, nbPins);
}

/* Get string and display it on the screen.
 * In 4 lines mode, for the LCD the line 3 is just after the line 1 and the line 4 just after the line 2 
 * in case of long string, we display in the good line order (1 2 3 4) and not the hardware order (1 3 2 4) */
void stringToLCD(char *string, gpio pins[], int nbPins)
{
	int i;
	int size = strlen(string);

	if (string != NULL) {
		for (i = 0; i < size && i < NB_CHAR_LINES * NB_LINES; i++) {
			if (i % NB_CHAR_LINES == 0)
				placeCursorLine(i / NB_CHAR_LINES, pins, nbPins);

			charToLCD(string[i], pins, nbPins);
		}
	}
}

void placeCursorLine(int numLine, gpio pins[], int nbPins)
{
	unsigned char data;

	if(numLine == 0)
		backToBegin(pins, nbPins, DURATION);
	else if(numLine == 1) {
		data = 0b000101;
		refreshLCD(data, pins, nbPins, DURATION);
		data = 0b000001;
		refreshLCD(data, pins, nbPins, DURATION);
	} else if (numLine == 2) {
		data = 0b001001;
		refreshLCD(data, pins, nbPins, DURATION);
		data = 0b000010;
		refreshLCD(data, pins, nbPins, DURATION);
	} else if(numLine == 3) {
		data = 0b001011;
		refreshLCD(data, pins, nbPins, DURATION);
		data = 0b000010;
		refreshLCD(data, pins, nbPins, DURATION);
	}
}
void writeLine(int numLine, char *string, gpio pins[], int nbPins)
{
	int i;
	int size = strlen(string);

	if(string != NULL) {
		placeCursorLine(numLine, pins, nbPins);

		for(i = 0; i < NB_CHAR_LINES && i < size; i++)
			charToLCD(string[i], pins, nbPins);
	}
}
