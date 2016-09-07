#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER 1024

/* 
 * Software to compute the sliding bitrate during the X previous seconds.
 * It uses "ffprobe --show_frames" output to generate this information.
 * Useful to check that a video or encoding parameters are not bigger than
 * a bitrate limit in case of video streaming for example.
 */

int addArray(int *array, int size)
{
	int i;
	int value = 0;

	for (i = 0; i < size; i++)
		value += array[i];

	return value;
}

void peak(FILE *file, int fps, int window_sec, int br_target)
{
	int size = fps * window_sec;
	int *bitrates = malloc(sizeof(int) * size);
	char buf[BUFFER];
	int i = 0;
	int pos, br, max_br = 0;
	float br_display;
	bool aboveTarget;

	if (!bitrates) {
		fprintf(stderr, "Can not allocate bitrates array\n");
		return;
	}

	memset(bitrates, 0, size * sizeof(int));

	while (!feof(file)) {
		fgets(buf, BUFFER, file);

		/* To count the current frame number */
		if (!strcmp(buf, "[FRAME]\n")) {
			i++;
		/* The current packet size value */
		} else if (!strncmp(buf, "pkt_size=", strlen("pkt_size="))) {
			/* For the firsts frames */
			if (i < size)
				pos = i;
			else
				pos = i % size;

			/* Save the packet size into the "window" bitrate array */
			br = atoi(buf + strlen("pkt_size="));
			bitrates[pos] = br;
			br = addArray(bitrates, size); /* Value in byte */

			/* Display correctly firsts bitrate values, in kbit/s */
			if (i < size)
				br_display = (float) br * size / i / 1024.0 / window_sec * 8;
			else
				br_display = (float) br / 1024.0 / window_sec * 8;

			if (max_br < br)
				max_br = br;

			if (br_display > br_target)
				aboveTarget = true;
			else
				aboveTarget = false;

				printf("Peak bitrate window for frame %8d: %8.1f kibit/s %s\n", i, br_display, aboveTarget ? "Above target" : "");
		}
	}

	printf("\nMaximum peak bitrate: %f kibit/s\n", (float) max_br / 1024.0 / window_sec * 8);

	free(bitrates);
}

int main(int argc, char *argv[])
{
	char filename[BUFFER];
	FILE *file;
	int fps;
	int window_seconds;
	int br_target;

	if (argc < 5) {
		fprintf(stderr, "Bad number of arguments: bitrate_peak filename fps window_seconds bitrate_target (kibit/s)\n");
		return EXIT_FAILURE;
	}

	strcpy(filename, argv[1]);
	fps = atoi(argv[2]);
	window_seconds = atoi(argv[3]);
	br_target = atoi(argv[4]);

	file = fopen(filename, "r");

	if (!file) {
		fprintf(stderr, "Can not open this file: %s\n", filename);
		return EXIT_FAILURE;
	} else
		peak(file, fps, window_seconds, br_target);

	return 0;
}
