/**
 * Task based handlers for receiving input from various input devices.
 */

#ifndef _CONTROL_H_
#define _CONTROL_H_

#include <stdint.h>
#include <Task.h>
#include <IRremote.h>
#include "lineFollow.h"
#include "driveTrain.h"
#include "commands.h"

#ifdef _DEBUG
#include <Streaming.h>
#endif // _DEBUG

// Defines for each input type
#define INP_SERIAL 0
#define INP_IR 1

/**
 * Task to handle serial input.
 */
class SerialIn : public Task {
	private:
		char _in;			// The new input character received.
		uint8_t _repeat;	// Counter for repeats of the same character
		uint32_t _lastRx;	// Time the last char was received.
		bool _newInput;		// True if new input is ready.

	public:
		SerialIn();
		virtual void run(uint32_t now);
		virtual bool canRun(uint32_t now);
		bool newInput(char *c, uint8_t *rep);
};

/**
 * Task to handle IR input.
 */
class IrIn : public Task {
	private:
		uint8_t _pin;		// The pin to used for IR input
		decode_results _irRes; // The decoded IR result
		uint32_t _lastCode;	// The last IR code received.
		uint8_t _repeat;	// Counter for repeats of the same code
		uint32_t _lastRx;	// Time the last code was received.
		bool _newInput;		// True if new input is ready.
		IRrecv *_irRecv;	// Pointer to IR Receiver instance.

	public:
		IrIn(uint8_t pin);
		virtual void run(uint32_t now);
		virtual bool canRun(uint32_t now);
		bool newInput(uint32_t *c, uint8_t *rep);
};

/**
 * Task to check the input handlers for new input, and if any, attempt to
 * decode the new input to a known command.
 */
class InputDecoder : public Task {
	private:
		char _serIn;			// Stores serial input
		uint32_t _irCode;		// Stores IR input
		uint8_t _repeat;		// Receives input repeats count
		uint8_t _whatAvail;		// Indicates to the run() method what type of
								// input is available.
		SerialIn *_serialIn;	// Pointer to Serial input task handler object.
		IrIn *_irIn;			// Pointer to IR input task handler object.
		uint8_t _cmd;			// Holds the command code for valid input
		bool _newCmd;			// Indicates when a new command is available
		bool _learnMode;		// Will be set when in commands learning mode
		uint32_t _learnTimeout;	// Time when learn mode times out without input
		uint8_t _learnStep;		// The current step in learn mode

		// Private methods
		void _learn(uint32_t now);
	
	public:
		InputDecoder(SerialIn *si, IrIn *ii);
		virtual void run(uint32_t now);
		virtual bool canRun(uint32_t now);
		bool newCommand(uint8_t *c, uint8_t *rep);
};

/**
 * Simple command consumer class based on the Task class.
 */
class CommandConsumer : public Task {
	private:
		uint8_t _cmd;				// Holder for new commands
		uint8_t _repeat;			// Command repeat counter

		InputDecoder *_iDecoder;	// Pointer to the input decoder for commands
		DriveTrain *_device;		// Pointer to the device being controlled.
									// The DriveTrain in this case.
		LineFollow *_lineFol;		// Pointer to the line follower.

	public:
		CommandConsumer(InputDecoder *id, DriveTrain *dev, LineFollow *lf);
		virtual void run(uint32_t now);
		virtual bool canRun(uint32_t now);
		char *lastCommand();
};


#endif	// _CONTROL_H_
