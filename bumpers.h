/**
 * Task based Bumpers Sensors
 */

#ifndef _BUMPERS_H_
#define _BUMPERS_H_

#include <stdint.h>
#include "config.h"
#include "debug.h"
#include "utils.h"
#include "driveTrain.h"
#include <Task.h>

#ifdef DEBUG
#include "Streaming.h"
#endif // DEBUG


/**
 * Task to monitor bumpers
 */
class Bumpers : public Task {
    private:
        uint8_t _fl, _fr;	// Front left and right pins
		uint8_t _state;		// Bitwise bumpers state indicator
        DriveTrain *_driveTrain;    // Pointer to drive train object

    public:
        Bumpers(uint8_t pinFL, uint8_t pinFR, DriveTrain *driveTrain);
		virtual void run(uint32_t now);
		virtual bool canRun(uint32_t now);
};

#endif  //_BUMPERS_H_
