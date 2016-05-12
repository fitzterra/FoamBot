/**
 * Various utility functions.
 */

#ifndef _UTILS_H_
#define _UTILS_H_

// If SERIAL_SPEED is not defined in config.h, default it to 57600 here.
#ifndef SERIAL_SPEED
#define SERIAL_SPEED 57600
#endif // SERIAL_SPEED

void OpenSerial(long speed = SERIAL_SPEED);

#endif // _UTILS_H_
