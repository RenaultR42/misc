#ifndef LCD_H
#define LCD_H

#include "gpio.h"

/* Duration in microseconds, POSIX standard */
#define DURATION 250
#define STEPS_INIT 6

/* Screen parameters */
#define NB_LINES 4
#define NB_CHAR_LINES 20

void backToBegin(gpio pins[], int nbPins, int duration);
void refreshLCD(unsigned char donnees, gpio pins[], int nbPins, int duration);
unsigned char writeChar(char c, int section);
void charToLCD(char c, gpio pins[], int nbPins);
void initLCD(gpio pins[], int nbPins);
void enableBlinkCursor(gpio pins[], int nbPins);
void disableBlinkCursor(gpio pins[], int nbPins);
void clearScreen(gpio pins[], int nbPins);
void shutdownLCD(gpio pins[], int nbPins);
void stringToLCD(char *string, gpio pins[], int nbPins);
void placeCursorLine(int numLine, gpio pins[], int nbPins);
void writeLine(int numLine, char *string, gpio pins[], int nbPins);

#endif
