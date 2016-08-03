#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "gpio.h"

/* Write new GPIO value in associated sysfs file */
void writeValueToGPIO(int value, char *nom)
{
	FILE *file;
	char path[BUFFER];

	if (nom != NULL) {
		sprintf(path, "/sys/class/gpio/gpio%s/value", nom);
		file = fopen(path, "w");

		if (file != NULL) {
			fprintf(file, "%d", value);
			fclose(file);
		} else
			printf("Error during openning %s to write value %d\n", path, value);
	}
}

/* Remove access to GPIO */
void deleteGPIO(gpio pins[], int nbPins)
{
	FILE *file;
	int i;

	if (pins !=NULL) {
		for (i = 0; i < nbPins; i++) {
			file = fopen("/sys/class/gpio/unexport", "w");
			if (file != NULL) {
				fprintf(file, "%d", pins[i].id);
				fclose(file);
			} else
				printf("Error when openning file /sys/class/gpio/unexport\n");
		}
	}
}

/* Enable GPIO in Linux kernel */
void initGPIO(gpio pins[], int nbPins)
{
	FILE *file;
	int i;
	char path[BUFFER];

	if (pins != NULL) {
		for (i = 0; i < nbPins; i++) {
			file = fopen("/sys/class/gpio/export", "w");

			/* For each GPIO, we associate name chosen by Linux kernel and its id.
			 * The order is: ENABLE RS D4 D5 D6 D7 */
			if (file != NULL) {
				switch (i) {
				case ENABLE:
					pins[i].id = 1;
					strcpy(pins[i].name, "1_pe0");
					break;

				case RS:
					pins[i].id = 2;
					strcpy(pins[i].name, "2_pe1");
					break;

				case D7:
					pins[i].id = 15;
					strcpy(pins[i].name, "15_pi0");
					break;

				case D6:
					pins[i].id = 16;
					strcpy(pins[i].name, "16_pi1");
					break;

				case D5:
					pins[i].id = 17;
					strcpy(pins[i].name, "17_pi2");
					break;

				case D4:
					pins[i].id = 18;
					strcpy(pins[i].name, "18_pi3");
					break;

				default:
					printf("Error during init GPIO\n");
				}

				fprintf(file, "%d", pins[i].id);
				fclose(file);

				/* Each GPIO is in output mode */
				sprintf(path, "/sys/class/gpio/gpio%s/direction", pins[i].name);
				file = fopen(path, "w");

				if (file != NULL) {
					fprintf(file, "out");
					fclose(file);
				} else
					printf("Error when openning %s", path);
			} else 
				printf("Error when openning /sys/class/gpio/export\n");
		}
	}
}

/* Apply state changes to each GPIO */
void changeStateGPIO(gpio pins[], unsigned char data, int nbPins)
{
	int i;

	if (pins != NULL) {
		for (i = 0; i < nbPins; i++)
			writeValueToGPIO(readBit(data, i), pins[i].name);
	}
}

unsigned char writeBit(unsigned char data, int value, int positionBit)
{
	if (value == 1)
		data |= 1 << positionBit;
	else 
		data &= ~(1 << positionBit);

	return data;
}

unsigned char readBit(unsigned char data, int position)
{
	unsigned char bit = data & (1 << position);

	if (bit != 0)
		bit = 1;

	return bit;
}

/* Validate data should be possible only with a pulsation */
void pulsePin(gpio pins[], int numPins, int duration)
{
	usleep(duration);
	writeValueToGPIO(1, pins[numPins].name);
	usleep(duration);
	writeValueToGPIO(0, pins[numPins].name);
	usleep(duration);
}
