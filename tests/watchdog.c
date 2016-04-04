/* Application to test watchdog device on board */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

#include <linux/watchdog.h>

#define WATCHDOG "/dev/watchdog"
static const char *const short_options = "hd:i:";
static const struct option long_options[] = {
    {"help", 0, NULL, 'h'},
    {"dev", 1, NULL, 'd'},
    {"interval", 1, NULL, 'i'},
    {NULL, 0, NULL, 0},
};

static void print_usage(FILE *stream, char *app_name, int exit_code)
{
   fprintf(stream, "Usage: %s [options]\n", app_name);
   fprintf(stream,
       " -h  --help                Display this usage information.\n"
       " -d  --dev <device_file>   Use <device_file> as watchdog device file.\n"
       "                           The default value is '/dev/watchdog'\n"
       " -i  --interval <interval> Change the watchdog interval time\n");

   exit(exit_code);
}

static void close_watchdog(int fd)
{
    /* The 'V' value shall be written into watchdog device file before
       to close/stop the watchdog.
     */
    write(fd, "V", 1);
    close(fd);
}

int main(int argc, char **argv)
{
    int fd;
    int interval = 0; /* Watchdog timeout interval (in secs) */
    int bootstatus;
    char *dev = WATCHDOG;
    int option;
    int next_option;
    char kick_watchdog = 0;

    /* Parse options if any */
    do {
        next_option = getopt_long(argc, argv, short_options, long_options, NULL);

       switch (next_option) {
       case 'h':
           print_usage(stdout, argv[0], EXIT_SUCCESS);
       case 'd':
           dev = optarg;
           break;
       case 'i':
           interval = atoi(optarg);
          break;
       case '?':   /* Invalid options */
           print_usage(stderr, argv[0], EXIT_FAILURE);
       case -1:   /* Done with options */
           break;
       default:   /* Unexpected stuffs */
           abort();
       }
    } while (next_option != -1);

    /* Open watchdog device */
    fd = open(dev, O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Set the watchdog interval */
    if (interval != 0) {
        if (ioctl(fd, WDIOC_SETTIMEOUT, &interval) != 0) {
           fprintf(stderr, "Error: set watchdog interval failed\n");
           close_watchdog(fd);
           exit(EXIT_FAILURE);
        }
    }

    /* Display current watchdog interval */
    if (ioctl(fd, WDIOC_GETTIMEOUT, &interval) == 0) {
        fprintf(stdout, "Current watchdog interval is %d\n", interval);
    } else {
        fprintf(stderr, "Error: cannot read watchdog interval\n");
        close_watchdog(fd);
        exit(EXIT_FAILURE);
    }

    /* Check if last boot is caused by watchdog */
    if (ioctl(fd, WDIOC_GETBOOTSTATUS, &bootstatus) == 0) {
        fprintf(stdout, "Last boot is caused by: %s\n",
           (bootstatus != 0) ? "Watchdog" : "Power-On-Reset");
    } else {
        fprintf(stderr, "Error: cannot read watchdog status\n");
        close_watchdog(fd);
        exit(EXIT_FAILURE);
    }

    /* There are two ways to reset the watchdog:
        - by writing any value into watchdog device file, or
        - by using IOCTL WDIOC_KEEPALIVE
    */
    fprintf(stdout,
            "Use:\n"
            " <d> to reset through writing over device file\n"
            " <i> to reset through IOCTL\n" 
            " <q> to exit the program\n");
    do {
        kick_watchdog = getchar();
        switch (kick_watchdog) {
        case 'd':
            write(fd, "d", 1);
            fprintf(stdout,
                    "Reset watchdog through writing over device file\n");
            break;
        case 'i':
            ioctl(fd, WDIOC_KEEPALIVE, NULL);
            fprintf(stdout, "Reset watchdog through IOCTL\n");
            break;
        case 'q':
            fprintf(stdout, "Goodbye !\n");
            break;
        default:
            fprintf(stdout, "Unknown command\n");
            break;
        }
    } while (kick_watchdog != 'x');

    close_watchdog(fd);
    return EXIT_SUCCESS;
}
