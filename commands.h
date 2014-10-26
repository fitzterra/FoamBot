#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "config.h"
#include <eeprom_access.h>
#include <Streaming.h>

/*** All possible Commands ****/
#define CMD_FWD 0	// Forward
#define CMD_REV 1	// Reverse
#define CMD_LFT 2	// Left
#define CMD_RGT 3	// Right
#define CMD_SUP 4	// Speed up
#define CMD_SDN 5	// Slow down
#define CMD_BRK 6	// Brake
//#define CMD_FRW 5	// Freewheel
//#define CMD_SPL 8	// Spin left
//#define CMD_SPR 9	// Spin right
#define CMD_INF 7	// Info
#define CMD_DMO 8	// Demo      
#define CMD_LRN 9	// Learn command maps
//#define CMD_TLK 13	// Talk?
#define CMD_ZZZ 10	// End indicator

/*** Any other defines ****/
#define ESC_KEY 0x1B		// Escape key code
#define CR_KEY 0x0D			// Carriage return
#define LF_KEY 0x0A			// Line feed

/**** Command names map ***/
extern char *cmdName[CMD_ZZZ];

/**** Map of IR codes to commands ****/
extern unsigned long cmdIR[CMD_ZZZ];

/**** Map of serial input character codes to commands ****/
extern char cmdSerial[CMD_ZZZ];

// Should be changed everytime the commands or command structures change.
extern long eepromSignature;

// The EEPROM data structure definition for saving/retrieving command maps
struct __eeprom_data {
  long sig; 						// The config signature.
  int numCmds; 						// Number of commands
  char cmdSerial[CMD_ZZZ];			// The serial input char commands mapping
  unsigned long cmdIR[CMD_ZZZ];		// The IR code commands mapping
};

void saveCmdMaps();
void loadCmdMaps();

#endif // _COMMANDS_H_

