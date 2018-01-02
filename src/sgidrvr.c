/**************************************************************************
 *
 * File:     sgidrvr.c
 *
 * Contains: Driver for the Logitech 3D mouse. This driver is based on
 *	     Logitech's "logidrvr.c" (from Jim Barnes), but does not have
 *	     full functionality. In particular, only 6D mouse functions in
 *	     Euler mode not supported (i.e,. no quaternions or 2D mouse
 * 	     modes).
 *
 * Author:   Terry Fong <terry@ptolemy.arc.nasa.gov>
 * History:  27-Mar-92  original version
 *
 ***************************************************************************/

#define _OLD_TERMIOS    /* this is for IRIX 6.5 */

#include <stdio.h>	/* for perror(3C) */
#include <sys/types.h>	/* for open(2) */
#include <sys/stat.h>	/* for open(2) */
#include <fcntl.h>	/* for open(2) */
#include <termios.h>	/* for tcsetattr(3T) */
#include <unistd.h>     /* for close(2) */

#include "sgidrvr.h"	/* function prototypes and data types */

/********************** local function prototypes ****************************/
static void euler_to_absolute (byte *record, MouseRecordPtr data);
static void print_bin (char a);

/*#define DEBUG*/

/******************************************************************************
 * Connect the mouse by opening a serial port (19200 baud, 8 data, 1 stop,
 * no parity). Reset the mouse once connected and verify diagnostics.
 *
 * Return: file descriptor to serial port or -1 if error opening port
******************************************************************************/
int logitech_open (char *port_name, char *reason)
{
  int fd;
  struct termios t;
  char data[DIAGNOSTIC_SIZE];	/* for diagnostics info */

  /* open a serial port, read/write */
  if ((fd = open (port_name, O_RDWR)) < 0) {
    sprintf(reason, "Could not open port: %s", port_name);
    perror ("logitech_open (open)");
    return -1;
  }

  /* get current termios structure */
  tcgetattr(fd, &t);

  /* disable all input mode processing */
  t.c_iflag = 0;

  /* disable all output mode processing */
  t.c_oflag = 0;

  /* hardware control flags: 19200 baud, 8 data bits, 1 stop bits, */
  /* no parity, enable receiver */
#ifdef _OLD_TERMIOS
  t.c_cflag = B19200 |  CS8 | CSTOPB | CREAD;
#else
  t.c_cflag = CS8 | CSTOPB | CREAD;
  t.c_ospeed = t.c_ispeed = B19200;
#endif

  /* disable local control processing (canonical, control sigs, etc) */
  t.c_lflag = 0;

  /* set timeout parameters */
  t.c_cc[VMIN] = 0; /* if no caracters recieved */
  t.c_cc[VTIME]= 20; /* read waits 2 seconds */

  /* control port immediately (TCSANOW) */
  if (tcsetattr(fd, TCSANOW, &t) < 0) {
    sprintf(reason, "Error controlling serial port: %s", port_name);
    perror ("logitech_open (tcsetattr)");
    close(fd);
    return -1;
  }

  /* reset the mouse */
  reset_control_unit (fd);

  /* do diagnostics */
  get_diagnostics (fd, data);

#ifdef DEBUG
  printf ("diag[0]: %2x=", data[0]);
  print_bin (data[0]);
  printf ("\n");
  printf ("diag[1]: %2x=", data[1]);
  print_bin (data[1]);
  printf ("\n");
#endif

  /* check diagnostic return */
  if ((data[0] != 0xbf) || (data[1] != 0x3f)) {
    sprintf(reason, "Mouse diagnostics failed (%s)", port_name);
    close(fd);
    return -1;
  }
  return (fd);
}


/***************************************************************************
 * Close the mouse by closing the serial port.
 *
 * Return 0 on success, -1 on failure.
 ***************************************************************************/
int logitech_close (int fd_mouse)
{
  if (close (fd_mouse) < 0) {
    perror ("error closing serial port");
    return (-1);
  } else return (0);
}


/**************************************************************************
* Command demand reporting
***************************************************************************/
void cu_demand_reporting (int fd)
{
#ifdef DEBUG
  printf ("demand reporting enabled\n");
#endif
  write (fd, "*D", 2);
}


/******************************************************************************
* Command control unit to Euler mode
******************************************************************************/
void cu_euler_mode (int fd)
{
#ifdef DEBUG
  printf ("euler data mode enabled\n");
#endif
  write (fd, "*G", 2);
}


/******************************************************************************
* Command control unit to head tracker mode
******************************************************************************/
void cu_headtracker_mode (int fd)
{
#ifdef DEBUG
  printf ("headtracking mode enabled\n");
#endif
  write (fd, "*H", 2);
}


/******************************************************************************
* Command control unit to mouse mode
******************************************************************************/
void cu_mouse_mode (int fd)
{
#ifdef DEBUG
  printf ("mouse mode enabled\n");
#endif
  write (fd, "*h", 2);
}


/******************************************************************************
* Command control unit to perform diagnostics
******************************************************************************/
void cu_request_diagnostics (int fd)
{
#ifdef DEBUG
  printf ("performing diagnostics\n");
#endif
  write (fd, "*\05", 2);
}


/******************************************************************************
* Demand a single report
******************************************************************************/
void cu_request_report (int fd)
{
#ifdef DEBUG
  printf ("asking for a single report\n");
#endif
  write (fd, "*d", 2);
}


/******************************************************************************
* Command a reset
******************************************************************************/
void cu_reset_control_unit (int fd)
{
#ifdef DEBUG
  printf ("resetting control unit\n");
#endif
  write (fd, "*R", 2);
}


/******************************************************************************
* Retrieve diagnostics report
******************************************************************************/
void get_diagnostics (int fd, char data[])
{
  cu_request_diagnostics (fd);	/* command diagnostics */
  read (fd, data, DIAGNOSTIC_SIZE);
}


/******************************************************************************
* Retrieve a single record. This routine will spin until a valid record
* (i.e., 16 bytes and bit 7, byte 0 is on) is received.
******************************************************************************/
int get_record (int fd, MouseRecordPtr data)
{
  int num_read;
  byte record[EULER_RECORD_SIZE];

  cu_request_report (fd);
  if((num_read = read (fd, record, EULER_RECORD_SIZE)) == -1)
    return -1;

  /* if didn't get a complete record or if invalid record, then try
     to get a good one */
  while ((num_read<EULER_RECORD_SIZE)|| !(record[0]&logitech_FLAGBIT)){
    
    /*     printf ("get_record: only got %d bytes\n", num_read); */

    /* flush the buffer */
#ifdef TCFLUSH 
    ioctl (fd, TCFLSH, 0);
#else /* Used on Linux systems */
    ioctl (fd, TCIFLUSH);
#endif

    cu_request_report (fd);
    if((num_read = read (fd, record, EULER_RECORD_SIZE)) == -1)
      return -1;    
  }

#ifdef DEBUG
  printf ("%d bytes read...", num_read);
#endif

  euler_to_absolute (record, data);

  return 0;
}


/******************************************************************************
* Set control unit into demand reporting, send reset command, and wait for 
* the reset.
******************************************************************************/
void reset_control_unit (int fd)
{
  cu_demand_reporting (fd);	/* make sure control unit is processing */
  usleep (100000);		/* wait 100 ms */
  cu_reset_control_unit (fd);	/* command a reset */
  sleep (1); 		        /* wait 1 second */
}


/******************************************************************************
* convert from raw Euler data record to absolute data
******************************************************************************/
static void euler_to_absolute (byte record[], MouseRecordPtr data)
{
  long ax, ay, az, arx, ary, arz;

  data->buttons = (byte) record[0];

  ax = (record[1] & 0x40) ? 0xFFE00000 : 0;
  ax |= (long)(record[1] & 0x7f) << 14;
  ax |= (long)(record[2] & 0x7f) << 7;
  ax |= (record[3] & 0x7f);

  ay = (record[4] & 0x40) ? 0xFFE00000 : 0;
  ay |= (long)(record[4] & 0x7f) << 14;
  ay |= (long)(record[5] & 0x7f) << 7;
  ay |= (record[6] & 0x7f);
  
  az = (record[7] & 0x40) ? 0xFFE00000 : 0;
  az |= (long)(record[7] & 0x7f) << 14;
  az |= (long)(record[8] & 0x7f) << 7;
  az |= (record[9] & 0x7f);

  data->x = ((float) ax) / 1000.0;
  data->y = ((float) ay) / 1000.0;
  data->z = ((float) az) / 1000.0;

  arx  = (record[10] & 0x7f) << 7;
  arx += (record[11] & 0x7f);
  
  ary  = (record[12] & 0x7f) << 7;
  ary += (record[13] & 0x7f);
   
  arz  = (record[14] & 0x7f) << 7;
  arz += (record[15] & 0x7f);

  data->pitch = ((float) arx) / 40.0;
  data->yaw = ((float) ary) / 40.0;
  data->roll = ((float) arz) / 40.0;
}


/******************************************************************************
* print an 8-bit binary string
******************************************************************************/
static void print_bin (char a)
{
  int i;

  for (i=7; i>=0; i--)
    printf ("%c", (a&(1<<i)) ? '1' : '0');
}

