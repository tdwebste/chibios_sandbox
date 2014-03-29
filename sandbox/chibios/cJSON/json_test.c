/**
 * json_test.c
 *
 *  Created on: 2013-06-04
 *      Author: jeromeg
 */

/*
 * This is a quick-and-dirty command-line utility to test the JSON interface
 * over RS485. This is NOT production code.
 */
/* ----------------------- Standard includes --------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define CMD_NONE    0
#define CMD_GET     1
#define CMD_SET     2
#define CODE_STX    0x02
#define CODE_ETX    0x03
#define BUFFER_SIZE 1024
/* ----------------------- Modbus includes ----------------------------------*/
/* ----------------------- Defines ------------------------------------------*/
#define PROG            "json_test"

/* ----------------------- Static variables ---------------------------------*/

static uint8_t    ucPort;
static char inbuffer[BUFFER_SIZE];
static char outbuffer[BUFFER_SIZE];
/* ----------------------- Static functions ---------------------------------*/

uint8_t debugflags = 0;
int fd;
struct termios xOldTIO;

/* ----------------------- Start implementation -----------------------------*/

void send_command(char *cmd)
{
    sprintf(inbuffer, "\x02%s\x03", cmd);
    write(fd,inbuffer, strlen(inbuffer));
}

void get_response(void)
{
    char *ptr;
    size_t  len;
    ptr = &outbuffer[0];
    do
    {
        len = read(fd, ptr, BUFFER_SIZE);
        ptr += len;
    } while (*(ptr - 1) != CODE_ETX);
    *ptr = 0;
    printf("%d: %s", len, &outbuffer[1]);
}
void print_usage(void)
{
    printf("Usage:\n\njson_test <options>\n\n");
    printf("-s <infile>                             - send database file\n");
    printf("-g                                      - get database\n");
    printf("-p <port>                               - use serial port <port>\n");
}

int serial_init( uint8_t ucPort, uint32_t ulBaudRate)
{
    char            szDevice[16];
    int             bStatus = 1;

    struct termios  xNewTIO;
    speed_t         xNewSpeed;

    snprintf( szDevice, 16, "/dev/ttyS%d", ucPort );

    if( ( fd = open( szDevice, O_RDWR | O_NOCTTY ) ) < 0 )
    {
        printf("Can't open serial port %s: %s\n", szDevice,
                    strerror( errno ) );
    }
    else if( tcgetattr( fd, &xOldTIO ) != 0 )
    {
        printf("Can't get settings from port %s: %s\n", szDevice,
                    strerror( errno ) );
    }
    else
    {
        bzero( &xNewTIO, sizeof( struct termios ) );

        xNewTIO.c_iflag |= IGNBRK | INPCK;
        xNewTIO.c_cflag |= CREAD | CLOCAL | CS8;

        switch ( ulBaudRate )
        {
        case 9600:
            xNewSpeed = B9600;
            break;
        case 19200:
            xNewSpeed = B19200;
            break;
        case 38400:
            xNewSpeed = B38400;
            break;
        case 57600:
            xNewSpeed = B57600;
            break;
        case 115200:
            xNewSpeed = B115200;
            break;
        default:
            bStatus = 0;
        }
        if( bStatus )
        {
            if( cfsetispeed( &xNewTIO, xNewSpeed ) != 0 )
            {
                printf("SER-INIT", "Can't set baud rate %ld for port %s: %s\n",
                            ulBaudRate, strerror( errno ) );
            }
            else if( cfsetospeed( &xNewTIO, xNewSpeed ) != 0 )
            {
                printf("SER-INIT", "Can't set baud rate %ld for port %s: %s\n",
                            ulBaudRate, szDevice, strerror( errno ) );
            }
            else if( tcsetattr( fd, TCSANOW, &xNewTIO ) != 0 )
            {
                printf("SER-INIT", "Can't set settings for port %s: %s\n",
                            szDevice, strerror( errno ) );
            }
        }
    }
    return bStatus;
}
int main( int argc, char *argv[] )
{
    int     iExitCode = EXIT_SUCCESS;
    int     index;
    int     c;
    int     timeout;
    int     cmd = CMD_NONE;
    char   *infile = NULL;
    opterr = 0;
    ucPort = 3; /* Default to first USB serial dongle */
   // char *ucPort = "ttyUSB0"; /* Default to first USB serial dongle */
    /*
     * Process command line options
     */
    while ((c = getopt(argc, argv, "gp:s")) != -1)
    {
        switch (c)
        {
        case 'g':
            cmd = CMD_GET;
            break;
        case 'p':
            ucPort = strtoul(optarg, NULL, 0);
            break;
        case 's':
            cmd = CMD_SET;
            break;
        default:
            abort();
            break;
        }
    }
    if (optind < argc)
    {
        infile = argv[optind++];
    }
    serial_init(ucPort, 115200);

    if (argc < 2)
    {
        print_usage();
    }
    switch(cmd)
    {
    case CMD_NONE:
        printf("NONE\n");
        break;
    case CMD_SET:
        printf("SET\n");
        send_command("SET {\"PulsesPerRotation\":50}");
        get_response();
        break;
    case CMD_GET:
        printf("GET\n");
        send_command("GET");
        get_response();
        break;
    }
    close(fd);
    return iExitCode;
}

