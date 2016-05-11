/**
 * Task based handler for receiving input from various devices.
 */

#include <Arduino.h>
#include "config.h"
#include "utils.h"
#include "control.h"

// ####################### SerialIn class definitions ######################

/**
 * Constructor.
 */
SerialIn::SerialIn() : Task() {
	_repeat = _in = _lastRx = 0;	// Initialise all vars.
	_newInput = false;

	// Open the serial port with default speed.
	OpenSerial();

	#ifdef DEBUG
    Serial << F("Starting SerialIn task...\n");
	#endif // DEBUG
}

/**
 * Tests if we have any input to process.
 */
bool SerialIn::canRun(uint32_t now) {
	return Serial.available();
}

/**
 * Processes any new input received.
 *
 * This method will only be called if serial input is available (the canRun()
 * method ensures this).
 *
 * @param now THe current millis() counter.
 */
void SerialIn::run(uint32_t now) {
	// Read the input;
	char c = (char)Serial.read();
	// Calculate the time since the last input was received
	uint32_t rxInterval = now - _lastRx;

	// Is the interval between the last received char and this one less than
	// the min delay allowed between input chars?
	if(rxInterval < SI_MIN_DELAY) {
		// Too quick. Ignore it
		#ifdef DEBUG
		Serial << "Serial min delay exceeded. Ignoring input...\n";
		#endif	//DEBUG
		return;
	}
	
	// Update the last received timer
	_lastRx = now;

	// Is it a repeat of the previous input and are we still within the allowed
	// repeat time?
	if (c==_in && rxInterval<=SI_REPEAT_MAX) {
		_repeat++;
	} else {
		_repeat = 0;
		_in = c;
	}

	_newInput = true;
}

/**
 * Checks if there is new input available and returns this input and any possible
 * repeat counts (via pointer args).
 *
 * NOTE: After calling this method, the input is reset and will not be available
 *       again on a successive call unless new input has arrived. The caller is
 *       responsible for processing the input after this call.
 *
 * @param c A pointer to a char type that will be set to the new char received
 *        if there is anything new.
 * @param rep A pointer to a uint_8 that will be set to the repeat count for how
 *        many times this input char was repeated within the SI_REPEAT_MAX time
 *        between repeats.
 *
 * @return True if new input is available, or False otherwise.
 */
bool SerialIn::newInput(char *c, uint8_t *rep) {
	if (!_newInput) 
		return false;

	*c = _in;
	*rep = _repeat;
	_newInput = false;

	return true;
}


// ####################### IrIn class definitions ######################

/**
 * Constructor.
 */
IrIn::IrIn(uint8_t pin) : Task(),
  _pin(pin) {
	_repeat = _lastRx = _lastCode = 0;	// Initialise all vars.
	_newInput = false;

	// Create the IR receiver instance
	_irRecv = new IRrecv(_pin);
	_irRecv->enableIRIn();

	#ifdef DEBUG
	OpenSerial();
    Serial << F("Starting IrIn task...\n");
	#endif // DEBUG
}

/**
 * Tests if we have any input to process.
 */
bool IrIn::canRun(uint32_t now) {
	// Try to decode any input we may have
	if (_irRecv->decode(&_irRes)) {
		// Get ready to receive the next input
		_irRecv->resume();
		return true;
	}
	return false;
}

/**
 * Processes any new input received.
 *
 * This method will only be called if serial input is available (the canRun()
 * method ensures this).
 *
 * @param now The current millis() counter.
 */
void IrIn::run(uint32_t now) {
	// We already have the new decoded input in _irRes.
	// Calculate the time since the last input was received
	uint32_t rxInterval = now - _lastRx;

	// Is the interval between the last received code and this one less than
	// the min delay allowed between input?
	if(rxInterval < IR_MIN_DELAY) {
		// Too quick. Ignore it
		#ifdef DEBUG
		Serial << "IR min delay exceeded. Ignoring input...\n";
		#endif	//DEBUG
		return;
	}
	
	// Update the last received timer
	_lastRx = now;

	// Is it a repeat of the previous input and are we still within the allowed
	// repeat time?
	if (_irRes.value==REPEAT && rxInterval<=IR_REPEAT_MAX) {
		_repeat++;
	} else {
		_repeat = 0;
		_lastCode = _irRes.value;
	}

	_newInput = true;
}

/**
 * Checks if there is new input available and returns this input and any possible
 * repeat counts (via pointer args).
 *
 * NOTE: After calling this method, the input is reset and will not be available
 *       again on a successive call unless new input has arrived. The caller is
 *       responsible for processing the input after this call.
 *
 * @param c A pointer to a uint32_t type that will be set to the new code received
 *        if there is anything new.
 * @param rep A pointer to a uint8_t that will be set to the repeat count for how
 *        many times this input code was repeated within the IR_REPEAT_MAX time
 *        between repeats.
 *
 * @return True if new input is available, or False otherwise.
 */
bool IrIn::newInput(uint32_t *c, uint8_t *rep) {
	if (!_newInput) 
		return false;

	// Always use _lastCode because _irRes.value may be the REPEAT value
	*c = _lastCode;
	*rep = _repeat;
	_newInput = false;

	return true;
}


// ####################### InputDecoder class definitions ######################

/**
 * Constructor.
 */
InputDecoder::InputDecoder(SerialIn *si, IrIn *ii) : Task(),
_serialIn(si), _irIn(ii) {
	// Open the serial port if we have not done so already.
	OpenSerial();

	// Preset local variables
	_learnMode = false;
	_learnStep = 0;
	_newCmd = false;
	_repeat = 0;
}

/**
 * Method used to learn which input commands to associate with which commands.
 *
 * This method will be called once as soon as a "learn command" was received
 * (CMD_LRN) by the run() method. The method will then set the internal object
 * state to "learn mode" and intercept any new input until all commands have
 * been learned or until the user decides to quit.
 *
 * @param now The time value we receive from the task scheduler vi the run()
 *        method.
 */
void InputDecoder::_learn(uint32_t now) {
	static uint8_t learnCmd = 0;	// Command number currently learning
	static uint8_t learnInput = 0;	// Type of input being learned: INP_SERIAL|INP_IR
	int i;

	// Set next timeout - 30 secs. The canRun() method will handle timeouts.
	_learnTimeout = now + 30000;

	// If we are at step 0, it is a new call for learning.
STEP0:
	if (_learnStep==0) {
		// First set learn mode on.
		_learnMode = true;
		// Reset the learn command tracker
		learnCmd = 0;
		// Ask what input to learn
		Serial << F("Train key or IR codes (k/i/q) ? ");
		// Get ready for next step
		_learnStep++;
		// Return and wait for next input
		return;
	}

	if (_learnStep==1) {
		// Here we only want serial input
		if (_whatAvail!=INP_SERIAL) {
			Serial << F("\nOnly key (serial) input allowed. Try again...\n");
			_learnStep = 0;
			goto STEP0;
		}
		// What input did we get?
		switch (_serIn) {
			case 'k':
				Serial << F("\nLearning key codes.");
				learnInput = INP_SERIAL;
				break;
			case 'i':
				Serial << F("\nLearning IR codes.");
				learnInput = INP_IR;
				break;
			case 'q':
			case ESC_KEY:
				Serial << F("Quiting...\n");
				_learnMode = false;
				_learnStep = 0;
				return;
				break;
			default:
				Serial << F("\nNot a valid answer. Please try again.\n");
				_learnStep = 0;
				goto STEP0;
			break;
		}
		// Some more messages
		Serial << F(" Press key for each command, escape to abort.\n");
		// We want to skip to step 3 to ask the command to learn
		_learnStep=3;
	}

	if (_learnStep==2) {
		// In this step we check the input supplied for a command
		// First check for escape or enter keys
		if (_whatAvail==INP_SERIAL) {
			switch (_serIn) {
				case ESC_KEY:
					Serial << F(" Aborting...\n");
					_learnMode = false;
					_learnStep = 0;
					return;
				case CR_KEY:
				case LF_KEY:
					// Do not change the current assignment. Go on to next
					_learnStep=3;
					learnCmd++;
					Serial << F("Not changed.\n");
					goto STEP3;
					break;
			}
		}
		// If we get here, the type of input received should be the type we
		// are learning
		if(_whatAvail!=learnInput) {
			if(learnInput==INP_SERIAL) {
				Serial << F("\nPlease use IR remote.");
			} else {
				Serial << F("\nPlease use keyboard (serial input).");
			}
			Serial << F(" Try again...\n");
			goto STEP3;
		}
		// Now we need to make sure that we do not already have this code assigned
		// to a previous command.
		for (i=0; i<learnCmd; i++) {
			if (learnInput==INP_SERIAL) {
				if (cmdSerial[i]!=0x00 && cmdSerial[i]==_serIn)
					break;
			} else {
				if (cmdIR[i]!=0x00 && cmdIR[i]==_irCode)
					break;
			}
		}
		if(i<learnCmd) {
			Serial << F("Already assigned to: ") << cmdName[i] << F(". Try again...\n");
			_learnStep=3;
			goto STEP3;
		}
		// Now we can assign the input to the command
		if (learnInput==INP_SERIAL) {
			cmdSerial[learnCmd] = _serIn;
			Serial << cmdSerial[learnCmd] << "  (0x" << _HEX(cmdSerial[learnCmd]) << ")" << endl;
		} else {
			cmdIR[learnCmd] = _irCode;
			Serial << F(" IR code 0x") << _HEX(cmdIR[learnCmd]) << endl;
		}
		// Next command
		learnCmd++;
		// Next Step
		_learnStep=3;
	}
STEP3:
	if (_learnStep==3) {
		// In this step we present the next command to learn.
		// Do not let the learn key be changed
		if (learnCmd==CMD_LRN) learnCmd++;
		// If we are not at the end of the commands yet
		if (learnCmd!=CMD_ZZZ) {
			// Prompt
			Serial << F("New key for ") << cmdName[learnCmd] << " [";
			// Add current value
			if (learnInput==INP_SERIAL) 
				Serial << cmdSerial[learnCmd];
			else
				Serial << F("0x") << _HEX(cmdIR[learnCmd]);
			Serial << "]? : ";
			// Next time round, get the answer
			_learnStep=2;
			return;
		}
		// Else we have done all commands. skip to step 4
		_learnStep=4;
	}

STEP4:
	if (_learnStep==4) {
		// In this step we ask if we should write the command maps to EEPROM
		Serial << F("Write new map(s) to EEPROM (y/n)? ");
		// Next step
		_learnStep = 5;
		return;
	}

	if (_learnStep==5) {
		// Here we only want serial input
		if (_whatAvail!=INP_SERIAL) {
			Serial << F("\nOnly key (serial) input allowed. Try again...\n");
			_learnStep = 4;
			goto STEP4;
		}
		// What input did we get?
		switch (_serIn) {
			case 'y':
				Serial << F("\nWriting to EEPROM. Please wait....");
				saveCmdMaps();
				Serial << F("   Done\n");
				break;
			case 'n':
			case ESC_KEY:
				Serial << F("\nNot written to EEPROM.\n");
				break;
			default:
				Serial << F("\nNot a valid answer. Please try again.\n");
				_learnStep = 4;
				goto STEP4;
			break;
		}
	}
	
	// All done, reset
	_learnMode = false;
	_learnStep = 0;
}

/**
 * Tests if we have any input to decode.
 */
bool InputDecoder::canRun(uint32_t now) {
	// If we have a SerialIn task, and it has any new input, fetch it and the
	// repeat count
	if (_serialIn!=NULL && _serialIn->newInput(&_serIn, &_repeat)) {
		// Indicate the type of input that is available.
		_whatAvail = INP_SERIAL;
		return true;
	// If we have a SerialIn task, and it has any new input, fetch it and the
	// repeat count
	} else if (_irIn!=NULL && _irIn->newInput(&_irCode, &_repeat)) {
		// Indicate the type of input that is available.
		_whatAvail = INP_IR;
		return true;
	}

	// Check for learn mode timeout
	if (_learnMode && now>=_learnTimeout) {
		// Reset learn mode and learn step
		_learnMode = false;
		_learnStep = 0;
		#ifdef DEBUG
		Serial << F("\nTimeout waiting for input. Aborting learn mode...\n");
		#endif //DEBUG
	}
	
	// Nothing available
	return false;
}

/**
 * Decodes any new input received.
 *
 * @param now The current millis() counter.
 */
void InputDecoder::run(uint32_t now) {
	int n;

	// Preset that no new command is available and clear the command code
	_newCmd = false;
	_cmd = CMD_ZZZ;

	// If we are in learn mode, go straight there.
	if(_learnMode) {
		_learn(now);
		return;
	}

	// Test the input received agains all possible commands.
	for (n=0; n<CMD_ZZZ; n++) {
		if(_whatAvail==INP_SERIAL && _serIn==cmdSerial[n]) {
			#ifdef DEBUG
			Serial << "Received serial input: " << _serIn \
				   << "  Repeat: " << _repeat << endl;
			#endif // DEBUG
			break;
		} else if(_whatAvail==INP_IR && _irCode==cmdIR[n]) {
			#ifdef DEBUG
			Serial << "Received IR input: " << _HEX(_irCode) \
				   << "  Repeat: " << _repeat << endl;
			#endif // DEBUG
			break;
		}
	}

	// Find a valid command?
	if(n==CMD_ZZZ) {
		#ifdef DEBUG
		if(_whatAvail==INP_SERIAL) {
			Serial << "Invalid serial input: " << _serIn << endl;
		} else if(_whatAvail==INP_IR) {
			Serial << "Invalid IR input: " << _HEX(_irCode) << endl;
		}
		#endif
		return;
	}

	// If we received the learn command, we go into learning mode
	if (n==CMD_LRN) {
		_learn(now);
		return;
	}

	// A valid command was found. Set the command and indicator
	_cmd = n;
	_newCmd = true;
}

/**
 * Checks if there is a new comand available and returns this command and any
 * possible repeat counts (via pointer args).
 *
 * NOTE: After calling this method, the command is reset and will not be available
 *       again on a successive call unless a new command has arrived. The caller
 *       is responsible for processing the command after this call.
 *
 * @param c A pointer to a uint8_t type that will be set to the new command ID
 *        received if there is anything new. This will correspond to one of the
 *        CMD_nnn defines in commands.h.
 * @param rep A pointer to a uint8_t that will be set to the repeat count for how
 *        many times this command  was repeated within the max allowed repeat
 *        time.
 *
 * @return True if a new command is available, or False otherwise.
 */
bool InputDecoder::newCommand(uint8_t *c, uint8_t *rep) {
	if (!_newCmd) 
		return false;

	// Return the command and repeats via the pointers
	*c = _cmd;
	*rep = _repeat;
	// Reset new command indicator.
	_newCmd = false;

	return true;
}


// ####################### CommandConsumer class definitions ######################

/**
 * Constructor.
 */
CommandConsumer::CommandConsumer(InputDecoder *id, DriveTrain *dev,
		LineFollow *lf) : Task(), _iDecoder(id), _device(dev), _lineFol(lf) {
	  
	// Open the serial port if we have not done so already.
	OpenSerial();
}

/**
 * Tests if we have any new commands to handle

 * @param now The current millis() counter.
 */
bool CommandConsumer::canRun(uint32_t now) {
	// Any new command?
	return _iDecoder->newCommand(&_cmd, &_repeat);
}

/**
 * Executes any new command received.
 *
 * @param now The current millis() counter.
 */
void CommandConsumer::run(uint32_t now) {
	// Debug
	D(F("Received command: ") << cmdName[_cmd] << F("  Repeat count: ") << _repeat << endl);

	// Dispatch command
	switch(_cmd) {
		case CMD_FWD:
			// Forward
			_device->forward();
			break;
		case CMD_REV:
			// Reverse
			_device->reverse();
			break;
		case CMD_BRK:
			// Brake. Deactive line follow mode in case it was active.
			_lineFol->deactivate();
			_device->stop();
			break;
		case CMD_LFT:
			// Left
			_device->left();
			break;
		case CMD_RGT:
			// Right
			_device->right();
			break;
		case CMD_SUP:
			// Speed up
			_device->speedUp();
			break;
		case CMD_SDN:
			// Slow down
			_device->slowDown();
			break;
		case CMD_INF:
			// Info
			_device->info();
			break;
		case CMD_DMO:
			// For now we use the demo command to go into line follower mode if not
			// doing so already.
			if (!_lineFol->isActive()) {
				// Stop the bot
				_device->stop();
				// Activate the line follower
				_lineFol->activate();
			}
			break;
		default:
			// Debug
			D(__FILE__<<":"<<__LINE__<<F("# ")<< F("Command not supported now.\n"));
	}
}

/**
 * Returns a pointer into the cmdName array for the string name of the last
 * command issued.
 */
char *CommandConsumer::lastCommand() {
	return cmdName[_cmd];
}
