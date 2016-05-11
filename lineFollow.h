/**
 * Task based Line Follwer Sensor
 */

#ifndef _LINEFOL_H_
#define _LINEFOL_H_

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
class LineFollow : public Task {
    private:
        uint8_t _lPin, _rPin;		// Left and right TCRT5000 pins
        int _lVal, _rVal;           // Left and right sensor values
        DriveTrain *_driveTrain;    // Pointer to drive train object
		bool _active;				// Indicates if LineFollower mode is active

    public:
        LineFollow(uint8_t pinL, uint8_t pinR, DriveTrain *driveTrain);
		virtual void run(uint32_t now);
		virtual bool canRun(uint32_t now);
		void activate() {_active = true;};
		void deactivate() {_active = false;};
		bool isActive() {return _active;};
        void senseVals(int *lVal, int *rVal);
};

#endif  //_LINEFOL_H_
