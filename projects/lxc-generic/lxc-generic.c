#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "lxc-generic.h"

/* Display error messages */
void displayError(int error, char *string)
{
	switch (error) {
	case INJECTION:
		fprintf(stderr, "Injection shell detected in parameters, aborting.\nLine: %s\n", string);
		break;
	case EMPTY:
		fprintf(stderr, "Empty data, data ignored.\n");
		break;
	case ERROR:
		fprintf(stderr, "Error detected: %s.\n", string);
		perror("Error errno : ");
		break;
	case INVALID:
		fprintf(stderr, "Invalid data in function parameters.\n");
		break;
	case NOT_FOUND:
		fprintf(stderr, "Data not found: %s.\n", string);
		break;
	case FILTERED:
		fprintf(stderr, "Filtered option: %s.\n", string);
		break; 
	case SUCCESS:
		break;
	default:
		fprintf(stderr, "Error: %s.\n", string);
		perror("Error errno: ");
		break;
	}
}

/* Command line reminder */
void help(void)
{
	int error;

	error = printf("lxc-generic is a tool which takes lxc-minimal file with some variables and takes another one"
			" with avriable definitions to insert correct command in the final script to generate desired lxc container\n"
		 "Format: lxc-generic (options) [output_file] [lxc_minimal_file] [config_file]\n"
		 "OPTIONS: \n"
		 "\t-h: help\n"
		 "\t-i \"string\": read string to filter keywords in configuration file"
		 "Other keyworkds could be interpreted.\n");

	if (error < SUCCESS)
		displayError(ERROR, NULL);
}

/* Protect against injection shell code in users input.
 * Naive behaviour, characters are forbidden ` | & $ > < # and partially ; - */
int protectInjection(char *string, int mode)
{
	int i, space = 0;
	int size = strlen(string);

	if (string == NULL)
		return INVALID;
	/* If we allow personnal scripts, it was obvious that injections are possible, we should disable protection in this case. */
	else if (mode != PERSO_SCRIPT) {
		for (i = 0; i < size; i++) {
			if (string[i] == '`' || string[i] == '|' || string[i] == '&'
			  || string[i] == '$' || string[i] == '>'
			  || string[i] == '<' || string[i] == '#') {
				return INJECTION;
			} else if ((mode != INIT && mode != DEVICE_FILES 
				    && mode != PERSO_SCRIPT && mode != NETWORK_CONFIG && mode != CONFIG_FILES && mode != DATA_FILES ) 
				    && (string[i] == ';' || string[i] == '-')) {
				return INJECTION;
			} else if (string[i] == ' ' || string[i] == '\t') {
				space++;
			}
		}

		/* If string contained only spaces or tabs */
		if (i+1 == space)
			return EMPTY;
		else
			return SUCCESS;
	}

	return SUCCESS;
}

/* Close two files including temporary file which contains inserted data before to replace original file and reopen them to do others inserts */
int saveFilesInserted(FILE **src, FILE **dst, char *nameSrc, char *nameDst)
{
	int errMV, errRM, errSrc, errDst;

	if(src == NULL || dst == NULL || nameSrc == NULL || nameDst == NULL)
		return INVALID;
	else {
		errSrc = fclose(*src);
		errDst = fclose(*dst);

		if (errSrc != SUCCESS || errDst != SUCCESS)
			return ERROR;

		errRM = remove(nameSrc);
		errMV = rename(nameDst, nameSrc);

		/* To avoid errors, original is opened in read only mode */
		*src = fopen(nameSrc, "r");
		*dst = fopen(nameDst, "w+");

		if (errMV != SUCCESS || errRM != SUCCESS || *src == NULL || *dst == NULL)
			return ERROR;
		else
			return SUCCESS;
	}
}

/* Copy between two positions from source file characters to destination file 
 * If end == -1 (END) the copy should be for the rest of the file */
int copyFiles(FILE *src, FILE *dst, long start, long end)
{
	int c;
	int error;

	if (src == NULL || dst == NULL || (start > end && end != EOF) || (start < 0 && start != EOF))
		return INVALID;
	else {
		error = fseek(src, start, SEEK_SET);

		while (!feof(src) && error != ERROR) {
			c = fgetc(src);

			if (c != EOF && c > SUCCESS)
				fputc(c, dst);
			else if (c < SUCCESS && c != EOF)
				return ERROR;

			/* Stop copy when the limit is reached or end of the file (if needed) */
			if (end != END && (error = ftell(src)) >= end)
				break;
		}

		if(error != ERROR)
			return SUCCESS;
		else
			return ERROR;
	}
}

int findLineFile(char *string, FILE *file)
{
	char line[BUFFER_SIZE];
	char *endLine;
	char *error;

	if (string == NULL || file == NULL)
		return INVALID;
	else {
		rewind(file);

		while (!feof(file)) {
			error = fgets(line, BUFFER_SIZE-1, file);

			if (error == NULL && !feof(file))
				return ERROR;

			endLine = strchr(line, '\n');

			if (endLine != NULL)
				*endLine = '\0';

			if (strcmp(string, line) == EQUAL)
				return SUCCESS;
		}

		return NOT_FOUND;
	}
}

/* Extract source substring from start position to next field delimited by separator.
 * Returns stop position to allow another iteration. (if needed) */
int copyStringSeparator(char *src, char *dst, int start, char separator, int size)
{
	int i;
	int sizeString = strlen(src);

	if (src == NULL || dst == NULL || start >= size || start >= sizeString || start < 0)
		return INVALID;
	else {
		for (i = start ; i < size && i <= sizeString; i++) {

			/* Stop in case of end of line, end of string or defined separator */
			if (src[i] == separator || src[i] == '\n' || src[i] == '\0') {
				dst[i - start] = '\0';
				return i+1;
			/* Else, copy */
			} else {
				dst[i - start] = src[i];
				}
			}
		return NOT_FOUND;
	}
}

/* Replace target (substring of string) by substitute. Start is the target position in the string */
int replaceString(char *string, const char *start, const char *target, const char *substitute)
{
	int i, j;
	char stringTmp[BUFFER_SIZE];
	int delta = start - string;

	if (string == NULL || start == NULL || target == NULL || substitute == NULL 
	    || start < string || start > (string + strlen(string)))
		return INVALID;
	else {
		memcpy(stringTmp, string, BUFFER_SIZE-1);

		/* Replace target */
		for (i = 0; i < strlen(substitute); i++)
			string[i + delta] = substitute[i];

		/* Copy after the target into origin */
		for (j = delta + strlen(target); j < strlen(stringTmp); j++, i++)
			string[i + delta] = stringTmp[j];

		string[i + delta] = '\0';
		return SUCCESS;
	}
}

/* Replace container names by user choice */
int containerName(char *nom, FILE *fScript, FILE *fScriptTmp)
{
	char line[BUFFER_SIZE];
	char *extract;
	long position;
	char *error;
	int ret = SUCCESS;
	int err;

	if (nom == NULL || fScriptTmp == NULL || fScriptTmp == NULL)
		return INVALID;
	else {
		rewind(fScript);
		rewind(fScriptTmp);

		/* If we find "CONTAINER_NAME", replace container name by this associated name */
		while (!feof(fScript) && ret == SUCCESS) {
			position = ftell(fScript);
			error = fgets(line, BUFFER_SIZE-1, fScript);

			extract = strstr(line, "CONTAINER_NAME");

			if (extract != NULL) {
				ret = replaceString(line, extract, "CONTAINER_NAME", nom);
				err = fprintf(fScriptTmp, "%s", line);
			} else if (position == ERROR || (error == NULL && !feof(fScript)) && err < SUCCESS)
				return ERROR;
			else
				ret = copyFiles(fScript, fScriptTmp, position, ftell(fScript));
		}

		return ret;
	}
}

/* Write necessary steps to copy needed binaries into container. The first one should be used as init if init was not enabled. */
int binariesName(char *noms, FILE *fScriptTmp)
{
	int error;
	int position = 0;
	char arg[BUFFER_SIZE];
	static int i = 0; /* static to keep in memory this pass and avoid to create another symslinks to init in case of multiple calls */
	int size = strlen(noms);

	if (noms == NULL || fScriptTmp == NULL)
		return INVALID;
	else {
		while (position >= SUCCESS && position < size) {
			position = copyStringSeparator(noms, arg, position, ' ', BUFFER_SIZE-1);

			error = fprintf(fScriptTmp, "which %s >/dev/null 2>&1\n\n"
				       "if [ $\? -ne 0 ]; then\n"
				       "\techo \"%s executable is not accessible\"\n"
				       "\treturn 1\n"
				       "fi\n\n"
				       "cp $(which %s) $rootfs$(which %s)\n"
				       "if [ $\? -ne 0 ]; then\n"
				       "\techo \"failed to copy %s in the rootfs\"\n"
				       "\treturn 1\n"
				       "fi\n\n"
				       "for LIB in $(ldd $(which %s) | cut -d ' ' -f 3) ; do\n"
				       "\tcp $LIB $rootfs$LIB\n"
				       "done\n\n", arg, arg, arg, arg, arg, arg);

			/* The first binary is the init process */
			if(i < 1 && error > SUCCESS)
				error = fprintf(fScriptTmp, "\tln -s $rootfs/$(which %s) $rootfs/sbin/init\n\n", arg);

			if (error < SUCCESS)
				return ERROR;

			i++;
		}

		if (position < SUCCESS && position != NOT_FOUND)
			return position;
		else
			return SUCCESS;
	}
}

/* Allow to add or create device files needed by the user */
int deviceFiles(char *deviceOptions, FILE *fScriptTmp)
{
	int error = SUCCESS;
	int position = 0;
	char arg[BUFFER_SIZE];
	int size = strlen(deviceOptions);

	if (deviceOptions == NULL || fScriptTmp == NULL)
		return INVALID;
	else {
		while (position >= SUCCESS && position < size && error >= SUCCESS) {
			position = copyStringSeparator(deviceOptions, arg, position, ';', BUFFER_SIZE-1);

			error = fprintf(fScriptTmp, "lxc.cgroup.devices.allow = %s\n", arg);
		}

		if (position < SUCCESS && position != NOT_FOUND)
			return position;
		else if (error < SUCCESS)
			return ERROR;
		else
			return SUCCESS;
	}
}

/* Mount in read only via bind (to improve security and save space disk)
 * common directories between host and the container. */
int mountPoints(char *mountDir, FILE *fScriptTmp)
{
	int error = SUCCESS;
	int position = 0;
	char arg[BUFFER_SIZE];
	int size = strlen(mountDir);

	if (mountDir == NULL || fScriptTmp == NULL)
		return INVALID;
	else {
		while (position >= SUCCESS && position < size && error >= SUCCESS) {
			position = copyStringSeparator(mountDir, arg, position, ' ', BUFFER_SIZE-1);
			error = fprintf(fScriptTmp, "lxc.mount.entry = %s %s none ro,bind 0 0\n", arg, arg+1);
		}

		if (position < SUCCESS && position != NOT_FOUND)
			return position;
		else if (error < SUCCESS)
			return ERROR;
		else
			return SUCCESS;
	}
}

/* Copy directories with useful configuration files. */
int configFiles(char *configArg, FILE *fScriptTmp)
{
	int error = SUCCESS;
	int position = 0;
	char arg[BUFFER_SIZE];
	int size = strlen(configArg);

	if (configArg == NULL || fScriptTmp == NULL)
		return INVALID;
	else {
		while (position >= SUCCESS && position < size && error >= SUCCESS) {
			position = copyStringSeparator(configArg, arg, position, ' ', BUFFER_SIZE-1);

			error = fprintf(fScriptTmp, "mkdir -p $rootfs$(dirname %s)\n"
				       "cp -R %s $rootfs$(dirname %s)\n", arg, arg, arg);
		}

		if (position < SUCCESS && position != NOT_FOUND)
			return position;
		else if (error < SUCCESS)
			return ERROR;
		else
			return SUCCESS;
	}
}

/* Write necessary steps to init process. initArg has user's options  */
int initConfig(char *initArg, FILE *fScript, FILE *fScriptTmp)
{
	char arg[BUFFER_SIZE], line[BUFFER_SIZE];
	char *extract, *ret;
	int position = 0;
	int error;
	long positionFile;
	int size = strlen(initArg);

	if (initArg == NULL || fScript == NULL || fScriptTmp == NULL)
		return INVALID;
	else {
		if(strcmp(initArg, "no") != SUCCESS) {
			/* Starting file, with user's options and binaries */
			error = fprintf(fScriptTmp, "\tcat <<EOF >> $rootfs/etc/init.d/rcS\n"
				       "#!/bin/sh\n"
				       "/bin/mount -a\n"
				       "udhcpc\n");

			while(position >= SUCCESS && position < size && error >= SUCCESS) {
				position = copyStringSeparator(initArg, arg, position, ';', BUFFER_SIZE-1);
				error = fprintf(fScriptTmp, "%s\n", arg);
			}

			error = fprintf(fScriptTmp, "EOF\n\n");

			if (error <= SUCCESS)
				return ERROR;
			else if (position < SUCCESS && position != NOT_FOUND)
				return position;

			/* Une autre séquence de lancement */
			error = fprintf(fScriptTmp, "\tcat <<EOF >> $rootfs/etc/inittab\n"
				       "::sysinit:/etc/init.d/rcS\n"
				       "tty1::respawn:/bin/getty -L tty1 115200 vt100\n"
				       "console::askfirst:/bin/sh\n"
				       "EOF\n\n"
				       "\tchmod 644 $rootfs/etc/inittab || return 1\n\n"
				       "\tln -s /bin/yes $rootfs/sbin/init\n\n");

			if (error <= SUCCESS)
				return ERROR;

			error = configFiles("/etc/rc.common /lib/functions.sh /lib/functions /etc/services /etc/uci-defaults /lib/libuci.so /lib/config /usr/share/libubox", fScriptTmp);

			if (error < SUCCESS)
				return error;

			error = binariesName("uci", fScriptTmp);

			/* Comment following lines with "ln" which are added by BINARIES_NAME flag */
			while (!feof(fScript) && error >= SUCCESS) {
				positionFile = ftell(fScript);
				ret = fgets(line, BUFFER_SIZE-1, fScript);
				extract = strstr(line, "ln");

				if (extract != NULL) {
					replaceString(line, extract, " ln ", "##");
					fprintf(fScriptTmp, "%s", line);
				} else if (positionFile == ERROR || (ret == NULL && !feof(fScript)))
					return ERROR;
				else
					error = copyFiles(fScript, fScriptTmp, positionFile, ftell(fScript));
			}
		}

		if (error < SUCCESS)
			return error;
		else
			return SUCCESS;
	}
}

/* To save information related to network */
int networkConfig(char *networkOption, FILE *fScript, FILE *fScriptTmp, int mode)
{
	long position;
	int error;
	int i = 0, start = 0;
	char arg[NB_NETWORK][BUFFER_SIZE];
	int size = strlen(networkOption);

	if (networkOption == NULL || fScript == NULL || fScriptTmp == NULL)
		return INVALID;
	else {
		/* Configure DHCP */
		if (mode == DHCP) {
			error = fprintf(fScriptTmp, "\tcat <<EOF >> $rootfs/usr/share/udhcpc/default.script\n"
				       "#!/bin/sh\n"
				       "case \"\\$1\" in\n"
				       "\tdeconfig)\n"
				       "\t\tip addr flush dev \\$interface\n"
				       "\t\t;;\n\n"
				       "\trenew|bound)\n"
				       "\t\tif [ -n \"\\$router\" ]; then\n"
				       "\t\t\tip route del default 2> /dev/null\n"
				       "\t\tfi\n\n"
				       "\t\tif [ -n \"\\$broadcast\" ]; then\n"
				       "\t\t\tbroadcast=\"broadcast \\$broadcast\"\n"
				       "\t\tfi\n\n"
				       "\t\tip addr add \\$ip/\\$mask \\$broadcast dev \\$interface\n\n"
				       "\t\tif [ -n \"\\$router\" ]; then\n"
				       "\t\t\tip route add default via \\$router dev \\$interface\n"
				       "\t\tfi\n\n"
				       "\t\t[ -n \"\\$domain\" ] && echo search \\$domain > /etc/resolv.conf\n"
				       "\t\tfor i in \\$dns ; do\n"
				       "\t\t\techo nameserver \\$i >> /etc/resolv.conf\n"
				       "\t\tdone\n"
				       "\t\t;;\n"
				       "esac\n"
				       "exit 0\n"
				       "EOF\n\n"
				       "\tchmod 744 $rootfs/usr/share/udhcpc/default.script\n\n");

			if (error < SUCCESS)
				return ERROR;
		}

		position = ftell(fScript);
		error = findLineFile("#NETWORK_CONFIG_LXC", fScript);

		if (position < SUCCESS)
			return ERROR;
		else if (error < SUCCESS)
			return error;

		error = copyFiles(fScript, fScriptTmp, position, ftell(fScript));
		if (error < SUCCESS)
			return error;

		if (mode == IPV4) {
			/* We get name of network, IP address and gateway address in this order */
			while (start >= SUCCESS && start < size && i < NB_NETWORK) {
				start = copyStringSeparator(networkOption, arg[i], start, ' ', BUFFER_SIZE-1);
				i++;
			}

			if (position < SUCCESS)
				return position;

			error = fprintf(fScriptTmp, "lxc.network.type = veth\n"
				       "lxc.network.flags = up\n"
				       "lxc.network.link = %s\n"
				       "lxc.network.name = %s\n"
				       "lxc.network.ipv4 = %s\n"
				       "lxc.network.ipv4.gateway = %s\n", arg[0], arg[1], arg[2], arg[3]);
		} else if (mode == DHCP) {
			/* We get "dhcp", hostname and virtual address in this order */
			while (start >= SUCCESS && start < size && i < NB_NETWORK_DHCP) {
				start = copyStringSeparator(networkOption, arg[i], start, ' ', BUFFER_SIZE-1);
				i++;
			}

			if (position < SUCCESS)
				return position;

			error = fprintf(fScriptTmp, "lxc.network.type = veth\n"
				       "lxc.network.flags = up\n"
				       "lxc.network.link = %s\n"
				       "lxc.network.name = %s\n", arg[1], arg[2]);
		}

		if (error < SUCCESS)
			return ERROR;
		else
			return SUCCESS;
	}
}

/* Necessary configuration to have a functionnal shell in the container */
int shellConfig(char *shellArg, FILE *fScript, FILE *fScriptTmp)
{
	long position;
	int error;

	if (shellArg == NULL || fScript == NULL || fScriptTmp == NULL)
		return INVALID;
	else {
		position = ftell(fScript);
		/* Copy busybox */
		error = fprintf(fScriptTmp, "which busybox >/dev/null 2>&1\n\n"
			     "if [ $\? -ne 0 ]; then\n"
			     "\techo \"busybox executable is not accessible\"\n"
			     "\treturn 1\n"
			     "fi\n\n"
			     "cp $(which busybox) $rootfs$(which busybox)\n"
			     "if [ $\? -ne 0 ]; then\n"
			     "\techo \"failed to copy busybox in the rootfs\"\n"
			     "\treturn 1\n"
			     "fi\n\n");

		if (error < SUCCESS || (position < SUCCESS && position != NOT_FOUND))
			return ERROR;

		/* Import Busybox commands alias */
		error = fprintf(fScriptTmp, "\tcd $rootfs/bin\n"
			     "\t./busybox --help | grep \'Currently defined functions:\' -A300 | \\\n"
			     "\tgrep -v \'Currently defined functions:\' | tr , \'\\n\' | \\\n"
			     "\txargs -n1 ln -s busybox\n"
			     "\tcd $rootfs\n\n");

		if (error < SUCCESS)
			return ERROR;

		/* Some device files needed by Busybox */
		error = fprintf(fScriptTmp, "\tcd $rootfs/dev || return 1\n\n"
			     "\t\tfor dev in tty console tty0 tty1 tty5 ram0 null random urandom; do\n"
			     "\t\t\ttouch $rootfs/dev/$dev\n"
			     "\t\tdone\n"
			     "\tcd /\n\n");

		if (error < SUCCESS)
			return ERROR;

		/* We allow these device files */
		error = findLineFile("#DEVICE_FILES", fScript);

		if (error < SUCCESS)
			return error;

		error = copyFiles(fScript, fScriptTmp, position, ftell(fScript));

		if (error < SUCCESS)
			return error;

		error = deviceFiles("c 5:0 rw ; c 5:1 rw ; c 4:0 rw ; b 1:0 rw ; c 1:3 rw ; c 1:5 rw ; c 1:8 rw ; c 1:9 rw", 
			fScriptTmp);

		if (error < SUCCESS && error != NOT_FOUND)
			return error;
		else
			return SUCCESS;
	}
}

/* Copy data directories into container. */
int dataFiles(char *dataConf, FILE *fScriptTmp)
{
	return configFiles(dataConf, fScriptTmp);
}

/* Copy personnal command lines if allowed */
int persoScript(char *scriptOption, FILE *fScriptTmp)
{
	int error = SUCCESS;
	int position = 0;
	char arg[BUFFER_SIZE];
	int size = strlen(scriptOption);

	if (scriptOption == NULL || fScriptTmp == NULL)
		return INVALID;
	else {
		while (position >= SUCCESS && position < size && error >= SUCCESS) {
			position = copyStringSeparator(scriptOption, arg, position, ';', BUFFER_SIZE-1);
			error = fprintf(fScriptTmp, "%s\n", arg);
		}

		if (position < SUCCESS && position != NOT_FOUND)
			return position;
		else if (error < SUCCESS)
			return ERROR;
		else
			return SUCCESS;
	}
}


/* Multiplex function */
void parseOption(int numOption, char *valueOptions, FILE *fScript, FILE *fScriptTmp)
{
	char *nameOptions[NB_OPTIONS] = OPTIONS_NAME;
	int error = SUCCESS, errorLine = SUCCESS;
	long position = SUCCESS;

	if (valueOptions != NULL && fScript != NULL && fScriptTmp != NULL) {
		/* CONTAINER_NAME is special because this value is not one line in skeleton file  */
		if (numOption != CONTAINER_NAME) {
			/* Search #OPTION in script file in one line */
			position = ftell(fScript);
			errorLine = findLineFile(nameOptions[numOption], fScript);
			error = copyFiles(fScript, fScriptTmp, position, ftell(fScript));
		}

		/* Multiplex */
		if (error == SUCCESS && errorLine == SUCCESS && position != ERROR) {
			switch (numOption) {
			case CONTAINER_NAME:
				error = containerName(valueOptions, fScript, fScriptTmp);
				break;
			case BINARIES_NAME:
				error = binariesName(valueOptions, fScriptTmp);
				break;
			case INIT:
				error = initConfig(valueOptions, fScript, fScriptTmp);
				break;
			case CAPACITIES_SYSTEM:
				error = fprintf(fScriptTmp, "lxc.cap.drop = %s\n", valueOptions);
				break;
			case NETWORK_CONFIG:
				if (strstr(valueOptions, "dhcp") != NULL)
					error = networkConfig(valueOptions, fScript, fScriptTmp, DHCP);
				else if (strcmp(valueOptions, "no") == SUCCESS)
					error = SUCCESS;
				else
					error = networkConfig(valueOptions, fScript, fScriptTmp, IPV4);
				break;
			case SELINUX_RULE:
				error = fprintf(fScriptTmp, "lxc.se_context = %s\n", valueOptions);
				break;
			case MOUNT_POINTS:
				error = mountPoints(valueOptions, fScriptTmp);
				break;
			case MOUNT_SOCKETS:
				if (strcmp(valueOptions, "yes") == SUCCESS) {
					error = fprintf(fScriptTmp, "lxc.mount.entry = /run run none bind 0 0\n"
						        "lxc.mount.entry = /var/run var/run none bind 0 0\n");
				}
				break;
			case DEVICE_FILES:
				error = deviceFiles(valueOptions, fScriptTmp);
				break;
			case INCLUDE_SHELL:
				if (strcmp(valueOptions, "yes") == SUCCESS)
					error = shellConfig(valueOptions, fScript, fScriptTmp);
				break;
			case CONFIG_FILES:
				error = configFiles(valueOptions, fScriptTmp);
				break;
			case DATA_FILES:
				error = dataFiles(valueOptions, fScriptTmp);
				break;
			case CPU_SHARES:
				error = fprintf(fScriptTmp, "lxc.cgroup.cpu.shares = %s\n", valueOptions);
				break;
			case MEMORY_LIMIT:
				error = fprintf(fScriptTmp, "lxc.cgroup.memory.limit_in_bytes = %s\n", valueOptions);
				break;
			case BLKIO_WEIGHT:
				error = fprintf(fScriptTmp, "lxc.cgroup.blkio.weight = %s\n", valueOptions);
				break;
			case BLKIO_READ:
				error = fprintf(fScriptTmp, "lxc.cgroup.blkio.throttle.read_bps_device = %s\n", valueOptions);
				break;
			case BLKIO_WRITE:
				error = fprintf(fScriptTmp, "lxc.cgroup.blkio.throttle.write_bps_device = %s\n", valueOptions);
				break;
			case NET_PRIO:
				error = fprintf(fScriptTmp, "lxc.cgroup.net_prio = %s\n", valueOptions);
				break;
			case PERSO_SCRIPT:
				error = persoScript(valueOptions, fScriptTmp);
				break;
			default:
				error = INVALID;
			}

			if (error < SUCCESS)
				goto output;
		}
		/* Copy end of file into inserted file */
		error = copyFiles(fScript, fScriptTmp, ftell(fScript), END);
	}

/* Errors handler */
output:
	if (errorLine < SUCCESS || error < SUCCESS || position == ERROR) {
		if (error < SUCCESS)
			displayError(error, "multiplex");
		else if (errorLine < SUCCESS)
			displayError(errorLine, "multiplex: search keywords");
		else
			displayError(position, "input-output");

		printf("Option value %d: %s = %s\n", numOption, nameOptions[numOption], valueOptions);
		fclose(fScript); 
		fclose(fScriptTmp);
		exit(error);
	}
}

/* Parse configuration line OPTION = VALUE which are two strings, 
 * the first one doesn't have spaces, the second one finished on end of line */
void parseConf(char *lineConf, FILE *fScript, FILE *fScriptTmp, int filterOptions[NB_OPTIONS])
{
	int i;
	char nameOptions[BUFFER_SIZE], valueOptions[BUFFER_SIZE];
	char *options[NB_OPTIONS] = OPTIONS_NAME;
	int ret;

	if (lineConf != NULL && fScript != NULL && fScriptTmp != NULL && filterOptions != NULL) {
		ret = sscanf(lineConf, "%2047s = %2047[^\n]\n", nameOptions, valueOptions);

		if (ret != 2) {
			displayError(ERROR, "parse configuration");
			fclose(fScript);
			fclose(fScriptTmp);
			exit(EXIT_FAILURE);
		}

		for (i = 0; i < NB_OPTIONS && ret >= SUCCESS; i++) {
			/* +1 is to avoid the # character in the comparison.
			* (# is important to comment script file and to protect variable) */
			if(strcmp(nameOptions, options[i] + 1) == EQUAL) {
				ret = protectInjection(valueOptions, i);

				if (ret == SUCCESS && filterOptions[i] != FILTERED)
					parseOption(i, valueOptions, fScript, fScriptTmp);
				else if (filterOptions[i] == FILTERED) {
					displayError(FILTERED, options[i]);
					ret = copyFiles(fScript, fScriptTmp, START, END);
				}

				break;
			}
		}

		/* If option is unknown, copy entirely the file without changes */
		if (i >= NB_OPTIONS) {
			displayError(NOT_FOUND, nameOptions);
			ret = copyFiles(fScript, fScriptTmp, START, END);
		} else if (ret == EMPTY) {
			displayError(EMPTY, nameOptions);
			ret = copyFiles(fScript, fScriptTmp, START, END);
		}

		if(ret != SUCCESS) {
			displayError(ret, valueOptions);
			fclose(fScript);
			fclose(fScriptTmp);
			exit(EXIT_FAILURE);
		}
	}
}

/* Read each line of configuration file to launch associated instructions and save state between each action */
int configurationScript(FILE *fScript, FILE *fConf, char *pathFScript, int filterOptions[NB_OPTIONS])
{
	char lineConf[BUFFER_SIZE];
	FILE *fScriptTmp = fopen("lxcTmp","w+");
	int error = SUCCESS, errorFer;
	char *ret;

	if (fScript == NULL || fConf == NULL || fScriptTmp == NULL || pathFScript == NULL || filterOptions == NULL)
		return EXIT_FAILURE;
	else {
		while (!feof(fConf) && error == SUCCESS) {
			ret = fgets(lineConf, BUFFER_SIZE-1, fConf);

			/* Take into account not commented lines */
			if (lineConf[0] != '#' && ret != NULL) {
				parseConf(lineConf, fScript, fScriptTmp, filterOptions);
				/* Close both files, replace original by temporary working file 
				 * then reopen to insert another data */
				error = saveFilesInserted(&fScript, &fScriptTmp, pathFScript, "lxcTmp");
			}
		}

		errorFer = fclose(fScriptTmp);

		if (errorFer != SUCCESS || error != SUCCESS || (ret == NULL && !feof(fConf))) {
			displayError(ERROR, "lecture configuration");
			return EXIT_FAILURE;
		} else 
			return EXIT_SUCCESS;
	}
}

int main(int argc, char *argv[])
{
	int i, j, position = SUCCESS;
	int error = EXIT_FAILURE;
	int errorFer1, errorFer2;
	FILE *fScript = NULL, *fConf = NULL, *fOutput = NULL;
	char extract[BUFFER_SIZE];
	int filterOptions[NB_OPTIONS] = { 0 };
	char *options[NB_OPTIONS] = OPTIONS_NAME;

	if (argc < 2)
		help();
	else {
		for (i = 1; i < argc; i++) {
			/* Display help */
			if (strcmp("-h", argv[i]) == EQUAL) {
				help();
				return EXIT_SUCCESS;
				/* If filter string */
			} else if (strcmp("-i", argv[i]) == EQUAL && i+1 < argc) {
				while (position >= SUCCESS) {
					position = copyStringSeparator(argv[i+1], extract, position, ' ', BUFFER_SIZE-1);

					for (j = 0; j < NB_OPTIONS; j++) {
						/* +1 is to avoid the # character in the comparison.
						* (# is important to comment script file and to protect variable) */
						if (strcmp(extract, options[j]+1) == EQUAL) {
							filterOptions[j] = FILTERED;
							break;
						}
					}
				}

				/* Avoid i+1 which is input string... */
				i++;
			} else if (i+2 < argc) {
				/* Read only by security */
				fOutput = fopen(argv[i], "w+");
				fScript = fopen(argv[i+1], "r");
				fConf = fopen(argv[i+2], "r");

				if (fScript == NULL || fConf == NULL || fOutput == NULL)
					displayError(ERROR, "openning files main");
				else
					copyFiles(fScript, fOutput, START, END);

				errorFer1 = fclose(fOutput);
				errorFer2 = fclose(fScript);
				fOutput = fopen(argv[i], "r");

				if (errorFer1 != SUCCESS || errorFer2 != SUCCESS || fOutput == NULL)
					displayError(ERROR, "closing/openning main");
				else
					error = configurationScript(fOutput, fConf, argv[i], filterOptions);

				errorFer1 = fclose(fConf);
				errorFer2 = fclose(fOutput);

				if (errorFer1 != SUCCESS || errorFer2 != SUCCESS) {
					displayError(ERROR, "closing main");
					exit(ERROR);
				}

				return error;
			} else {
				help();
				return error;
			}
		}
	}

	return error;
}
