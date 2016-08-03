#ifndef GPIO_H
#define GPIO_H

#define BUFFER 255
#define NB_PINS 6

/* Bit order in one byte: ENABLE, RS, D4, D5, D6 et D7 */
#define ENABLE 5
#define RS 4
#define D4 3
#define D5 2
#define D6 1
#define D7 0

/* GPIO structure with id affected by kernel Linux / hardware and suffix to go to associated sysfs /sys/class/gpio/... */
typedef struct {
	char name[BUFFER];
	int id;
} gpio;

void writeValueToGPIO(int value, char *name);
void deleteGPIO(gpio pins[], int namebrePins);
void initGPIO(gpio pins[], int nbPins);
void changeStateGPIO(gpio pins[], unsigned char data, int nbPins);
unsigned char writeBit(unsigned char data, int value, int positionBit);
unsigned char readBit(unsigned char data, int position);
void pulsePin(gpio pins[], int numPins, int duration);

#endif
