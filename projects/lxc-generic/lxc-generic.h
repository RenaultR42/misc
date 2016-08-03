#ifndef LXC_GENERIC_H
#define LXC_GENERIC_H

#include <stdio.h>

/* Some definitions to improve 	readability  */
#define EQUAL		0
#define BUFFER_SIZE	4096
#define NB_OPTIONS	19
#define NB_NETWORK	4 /* real network interface ; virtual network interface ; IP address ; gateway address */
#define NB_NETWORK_DHCP	3 /* "dhcp" ; real network interface ; virtual network interface */

/* Options names to parse in definition files */
#define OPTIONS_NAME { "#CONTAINER_NAME", "#BINARIES_NAME", "#INIT", \
"#CAPACITIES_SYSTEM", "#NETWORK_CONFIG", "#SELINUX_RULE",	      \
"#MOUNT_POINTS", "#MOUNT_SOCKETS", "#DEVICE_FILES", "#INCLUDE_SHELL", \
"#CONFIG_FILES", "#DATA_FILES", "#CPU_SHARES", "#MEMORY_LIMIT", "#BLKIO_WEIGHT", \
"#BLKIO_READ", "#BLKIO_WRITE", "#NET_PRIO", "#PERSO_SCRIPT" }

/* Options constants (same order) */
enum {
	CONTAINER_NAME,
	BINARIES_NAME,
	INIT, 
	CAPACITIES_SYSTEM,
	NETWORK_CONFIG,
	SELINUX_RULE, 
	MOUNT_POINTS,
	MOUNT_SOCKETS,
	DEVICE_FILES,
	INCLUDE_SHELL, 
	CONFIG_FILES,
	DATA_FILES,
	CPU_SHARES,
	MEMORY_LIMIT,
	BLKIO_WEIGHT, 
	BLKIO_READ,
	BLKIO_WRITE,
	NET_PRIO,
	PERSO_SCRIPT
};

/* Start and end of files */
enum {
	START = 0,
	END = -1
};

/* Network mode selected */
enum {
	DHCP,
	IPV4,
	IPV6
};

/* Error codes */
enum {
	SUCCESS = 0,
	ERROR = -1,
	EMPTY = -2,
	INJECTION = -3,
	INVALID = -4,
	NOT_FOUND = -5,
	FILTERED = -6
};

void displayError(int error, char *string);
void help(void);
int protectInjection(char *string, int mode);
int saveFilesInserted(FILE **src, FILE **dst, char *nameSrc, char *nameDst);
int copyFiles(FILE *src, FILE *dst, long start, long end);
int findLineFile(char *string, FILE *file);
int copyStringSeparator(char *src, char *dst, int start, char separator, int size);
int replaceString(char *string, const char *start, const char *target, const char *substitute);
int containerName(char *nom, FILE *fScript, FILE *fScriptTmp);
int binariesName(char *noms, FILE *fScriptTmp);
int deviceFiles(char *deviceOptions, FILE *fScriptTmp);
int mountPoints(char *mountDir, FILE *fScriptTmp);
int configFiles(char *configArg, FILE *fScriptTmp);
int initConfig(char *initArg, FILE *fScript, FILE *fScriptTmp);
int networkConfig(char *networkOption, FILE *fScript, FILE *fScriptTmp, int mode);
int shellConfig(char *shellArg, FILE *fScript, FILE *fScriptTmp);
int dataFiles(char *dataConf, FILE *fScriptTmp);
int persoScript(char *scriptOption, FILE *fScriptTmp);
void parseOption(int numOption, char *valueOptions, FILE *fScript, FILE *fScriptTmp);
void parseConf(char *lineConf, FILE *fScript, FILE *fScriptTmp, int filterOptions[NB_OPTIONS]);
int configurationScript(FILE *fScript, FILE *fConf, char *pathFScript, int filterOptions[NB_OPTIONS]);

#endif
