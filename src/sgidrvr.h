/*****************************************************************************
*
* File:     sgidrvr.h
*
* Contains: function prototypes and data types for logidrvr.c
*	    (based on Logitech's "logidrvr.h")
*
* Author:   Terry Fong <terry@ptolemy.arc.nasa.gov>
* History:  27-Mar-92  original version
*
* Copyright (C) 1992, National Aeronautics and Space Administration
* NASA Ames Research Center, Moffett Field, CA 94035
*
******************************************************************************/

#ifndef TRUE
#define TRUE			1
#endif

#ifndef FALSE
#define FALSE			0
#endif

#define DIAGNOSTIC_SIZE		2
#define EULER_RECORD_SIZE 	16

#define logitech_FLAGBIT        0x80
#define logitech_FRINGEBIT      0x40
#define logitech_OUTOFRANGEBIT  0x20
#define logitech_RESERVED       0x10
#define logitech_SUSPENDBUTTON  0x08
#define logitech_LEFTBUTTON     0x04
#define logitech_MIDDLEBUTTON   0x02
#define logitech_RIGHTBUTTON    0x01

/************************* logidrvr data types *******************************/
typedef unsigned char byte;

typedef struct {
  byte buttons;
  float x;
  float y;
  float z;
  float pitch;
  float yaw;
  float roll;
} MouseRecordType, *MouseRecordPtr;


/************************* function prototypes *******************************/
int  logitech_open (char *port_name, char *reason);
int  logitech_close (int fd_mouse);

void cu_demand_reporting (int fd);
void cu_euler_mode (int fd);
void cu_headtracker_mode (int fd);
void cu_mouse_mode (int fd);
void cu_request_diagnostics (int fd);
void cu_request_report (int fd);
void cu_reset_control_unit (int fd);

void get_diagnostics (int fd, char data[]);
int  get_record (int fd, MouseRecordPtr data);
void reset_control_unit (int fd);
