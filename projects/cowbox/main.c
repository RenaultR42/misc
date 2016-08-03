/*
# WP2 - System - 2013/2014
# COWBOX
# Author: Charles-Antoine Couret
# Mail  : charles-antoine.couret@isen.fr
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gpio.h"
#include "lcd.h"

#define PAUSE 20

int main(int argc, char *argv[])
{
	gpio pins[NB_PINS];
	FILE *toolsFile, *systemFile;
	char batteryStatus[BUFFER], networkName[BUFFER], toolsData[BUFFER];
	char batterieData[BUFFER], ressourceData[BUFFER], networkData[BUFFER], charge[BUFFER];
	int ram, nbConnections;
	float cpu;

	initLCD(pins, NB_PINS);
	clearScreen(pins, NB_PINS);

	if (argc == 2 && strcmp(argv[1], "restore") == 0) {
		writeLine(0, "[ INSERT MICRO-SD  ]", pins, NB_PINS);
		writeLine(1, "Before restoration, ", pins, NB_PINS);
		writeLine(2, "save your data ,  or", pins, NB_PINS);
		writeLine(3, "they will be lost !!", pins, NB_PINS);
	} else if (argc == 2 && strcmp(argv[1], "update") == 0) {
		writeLine(0, "====================", pins, NB_PINS);
		writeLine(1, "    SYSTEM UPDATE   ", pins, NB_PINS);
		writeLine(2, "====================", pins, NB_PINS);
		writeLine(3, "DO NOT DISCONNECT...", pins, NB_PINS);
	} else if (argc == 2 && strcmp(argv[1], "restore-wait") == 0) {
		writeLine(0, "====================", pins, NB_PINS);
		writeLine(1, " RESTORE.     WAIT. ", pins, NB_PINS);
		writeLine(2, "====================", pins, NB_PINS);
		writeLine(3, " FORMATING SD CARD  ", pins, NB_PINS);
	} else if (argc == 2 && strcmp(argv[1], "shutdown") == 0) {
		writeLine(0, "====================", pins, NB_PINS);
		writeLine(1, "  NO MICROSD CARD  ", pins, NB_PINS);
		writeLine(2, "====================", pins, NB_PINS);
		writeLine(3, "SYSTEM WILL SHUTDOWN", pins, NB_PINS);
	} else if (argc == 2 && strcmp(argv[1], "reboot") == 0) {
		writeLine(0, "====================", pins, NB_PINS);
		writeLine(1, " SYTEM WILL REBOOT  ", pins, NB_PINS);
		writeLine(2, "====================", pins, NB_PINS);
		writeLine(3, "DO NOT DISCONNECT...", pins, NB_PINS);
	} else if (argc == 2 && strcmp(argv[1], "boot") == 0) {
		writeLine(0, "====================", pins, NB_PINS);
		writeLine(1, "       COWBOX       ", pins, NB_PINS);
		writeLine(2, "====================", pins, NB_PINS);
		writeLine(3, "SYSTEM IS BOOTING...", pins, NB_PINS);
	} else if (argc >= 3) {
		while (1) {
			toolsFile = fopen(argv[1], "r");
			systemFile = fopen(argv[2], "r");

			if (systemFile != NULL && toolsFile != NULL) {
				backToBegin(pins, NB_PINS, DURATION);

				fscanf(systemFile, "battery_status=%255s\n", batteryStatus);
				fscanf(systemFile, "battery_level=%255s\n", charge);
				fscanf(systemFile, "network=%255s\n", networkName);
				fscanf(systemFile, "nb_connected=%255d\n", &nbConnections);
				fscanf(systemFile, "cpu_used=%255f\n", &cpu);
				fscanf(systemFile, "ram_used=%255d\n", &ram);
				fscanf(toolsFile, "%255s\n", toolsData);
				fclose(systemFile);
				fclose(toolsFile);

				sprintf(batterieData,  "%3s%% %15s", charge, batteryStatus);
				sprintf(networkData,   "%20s", networkName);
				sprintf(ressourceData, "CPU %03d%%    RAM %03d%%", (int)(100 - cpu), (int)((811 - ram/1000.0)/100.0));
				sprintf(toolsData,     "Connected: %9d" , nbConnections);

				writeLine(0, batterieData, pins, NB_PINS);
				writeLine(1, networkData, pins, NB_PINS);
				writeLine(2, ressourceData, pins, NB_PINS);
				writeLine(3, toolsData, pins, NB_PINS);

				shutdownLCD(pins, NB_PINS);
				sleep(PAUSE);
				initLCD(pins, NB_PINS);
				clearScreen(pins, NB_PINS);
			}
		}
	}

	shutdownLCD(pins, NB_PINS);
	return EXIT_SUCCESS;
}
