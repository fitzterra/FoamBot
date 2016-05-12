/**
 * Application compile time config settings.
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

// Define this to enable debugging
#define _DEBUG

// ############### General utility definitions #################
// NOTE!!! DO NOT change the order of these defs - LEFT and RIGHT are used as
// array indexes and should always be values 0 and 1
enum { LEFT, RIGHT, FORWARD, BACKWARD, STOP };

// ############### Digital Pin definitions #################
#define IR_PIN			4	// IR Receiver
#define SERVO_LEFT		5	// Pin for left servo
#define SERVO_RIGHT		6	// Pin for right servo
#define BUMP_FL_PIN		2	// Front left bumper pin
#define BUMP_FR_PIN		3	// Front right bumper pin
// These are hardcoded in the LCD lib, but we define them here as a reminder
#define LCD_DC          8 
#define LCD_RESET       9 
#define LCD_SCE         10 
#define LCD_SDIN        11 
#define LCD_SCLK        13 

// ############### Analog Pin definitions #################
#define LINEFOL_LEFT	4	// Line Follower Left Sensor Analog pin
#define LINEFOL_RIGHT	5	// Line Follower Right Sensor Analog pin

// ############### Bumper state definitions #################
#define BUMPED			0	// The pin state when the sensor IS bumped
// Bit positions for bumper bits in a bumper bit storage int
#define BUMP_FL			0
#define BUMP_FR			1
#define BUMP_RL			2
#define BUMP_RR			3
// A digital output pin that will be set high if any bumper is active.
// Can be used to light an LED or something.
#define BUMP_LED_PIN    7

// ############### Line Follower definitions #################
#define LINEFOL_MIN		400  // Min 'black' reading. Lower indicates off line
#define LINEFOL_MAX		1000  // Max 'black' reading. Higher means error.

// ############### LCD definitions #################
#define LCD_RATE        100  // LCD update rate in milliseconds

// ############### Serial Input config #################
// This is the minumum delay required between successive serial input characters.
// If succesive input is received within this delay period, the input will be
// read from the serial line, but it will be discarded. Note that keystrokes that
// deliver multiple characters like CRLF for Enter will only ever show the first
// character.
// The reason forthis delay is to prevent overenthusiastic pressing of keys to
// send the same command to a robot. Many robot commands takes a short while
// before the results can be seen.
#define SI_MIN_DELAY 100
// Some commands will behave differently depending on wether the command is being
// repeated or just sent in quick succession. This value determines the max time
// interval between successive receipt of the same character which would count as
// a repeat of this input.
#define SI_REPEAT_MAX 250

// ############### IR Input config #################
// As for SI_MIN_DELAY, but only for IR
#define IR_MIN_DELAY 100
// As for SI_REPEAT_MAX, but only for IR
#define IR_REPEAT_MAX 250

#endif  //_CONFIG_H_
