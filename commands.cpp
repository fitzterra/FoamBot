/**
 * Library to manage commands and command mappings.
 */

#include "commands.h"

/****** EEPROM Handling *****/
// Should be changed everytime the commands or command structures change.
long eepromSignature = 0xAFBAABFB;

/**** Command names map ***/
char *cmdName[] = {
	"Forward",			// CMD_FWD 0	// Forward
	"Reverse",      	// CMD_REV 1	// Reverse
	"Left",         	// CMD_LFT 2	// Left
	"Right",        	// CMD_RGT 3	// Right
	"Speed up",     	// CMD_SUP 4	// Speed up
	"Slow down",    	// CMD_SDN 5	// Slow down
	"Brake",        	// CMD_BRK 6	// Brake
//	"Freewheel",    	// CMD_FRW 5	// Freewheel
//	"Spin Left",    	// CMD_SPL 8	// Spin left
//	"Spin Right",   	// CMD_SPR 9	// Spin right
	"Info",         	// CMD_INF 7	// Info
	"Demo",         	// CMD_TST 8	// Test      
	"Learn",         	// CMD_LRN 9	// Learn command maps
//	"Talk",         	// CMD_TLK 13	// Talk??
};

/**** Map of IR codes to commands ****/
unsigned long cmdIR[] = {
	0x00,		// CMD_FWD 0	// Forward
	0x00,   	// CMD_REV 1	// Reverse
	0x00,   	// CMD_LFT 2	// Left
	0x00,   	// CMD_RGT 3	// Right
	0x00,   	// CMD_SUP 4	// Speed up
	0x00,   	// CMD_SDN 5	// Slow down
	0x00,   	// CMD_BRK 6	// Brake
//	0x00,      	// CMD_FRW 5	// Freewheel
//	0x00,      	// CMD_SPL 8	// Spin left
//	0x00,      	// CMD_SPR 9	// Spin right
	0x00,   	// CMD_INF 7	// Info
	0x00,      	// CMD_DMO 8	// Demo      
	0x00,      	// CMD_LRN 9	// Learn command maps
//	0x00,      	// CMD_TLK 13	// Talk?
};

/**** Map of character codes to commands ****/
char cmdSerial[] = {
	0,	// CMD_FWD 0	// Forward
	0,  // CMD_REV 1	// Reverse
	0,  // CMD_LFT 2	// Left
	0,  // CMD_RGT 3	// Right
	0,  // CMD_SUP 4	// Speed up
	0,  // CMD_SDN 5	// Slow down
	0,  // CMD_BRK 6	// Brake
//	0,	// CMD_FRW 5	// Freewheel
//	0,	// CMD_SPL 8	// Spin left
//	0,	// CMD_SPR 9	// Spin right
	0,  // CMD_INF 7	// Info
	0,	// CMD_DMO 8	// Demo      
	'l',// CMD_LRN 9	// Learn command maps
//	0,  // CMD_TLK 13	// Talk?
};

/**
 * Loads the command maps from EEPROM if the correct signature is found in
 * the EEPROM.
 */
void loadCmdMaps() {
	long sig;
	int cmdCount;
	// Read the signature and command counts from EEPROM	
	eeprom_read(sig, sig);
	eeprom_read(cmdCount, numCmds);

	if(sig==eepromSignature && cmdCount==CMD_ZZZ) {
		// Signature and cound is good, read the maps
		eeprom_read_to(cmdSerial, cmdSerial, sizeof(cmdSerial));
		eeprom_read_to(cmdIR, cmdIR, sizeof(cmdIR));
	#ifdef DEBUG
		Serial << F("Command maps read from EEPROM.\n");
	} else {
		Serial << F("Command maps signature not found in EEPROM.\n");
	#endif //DEBUG
	}
}

/**
 * Saves the command maps to EEPROM.
 */
void saveCmdMaps() {
	//Write the signature and command count
	eeprom_write(eepromSignature, sig);
	eeprom_write(CMD_ZZZ, numCmds);

	// Now write the maps
	eeprom_write_from(cmdSerial, cmdSerial, sizeof(cmdSerial));
	eeprom_write_from(cmdIR, cmdIR, sizeof(cmdIR));
	#ifdef DEBUG
	Serial << F("Command maps written to EEPROM.\n");
	#endif //DEBUG
}

