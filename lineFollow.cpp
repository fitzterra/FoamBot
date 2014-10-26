/**
 * Task based LineFollower
 */

#include "lineFollow.h"

// ####################### LIne follower class definitions ######################

/**
 * Constructor.
 */
LineFollow::LineFollow(uint8_t pinL, uint8_t pinR, DriveTrain *driveTrain) : Task() {
    // Save pins and drive tain
    _lPin = pinL;
    _rPin = pinR;
    _driveTrain = driveTrain;
	// The line follower mode starts off not being active
	_active = false;

	// Open the serial port with default speed.
	OpenSerial();

	// DEBUG
    D(F("Starting LineFollow task...\n"));
}

/**
 * Checks if line follower mode is active
 */
bool LineFollow::canRun(uint32_t now) {
	static bool lastState = false;

	// State changed?
	if (lastState != _active) {
		// Are we in line follow mode now?
		if (_active) {
			// Yes, set the driveTrain to 70% speed forward
			//_driveTrain->stop();
			_driveTrain->direction(0);
			_driveTrain->setSpeed(70);
		} else {
			// We exited line follow mode. Stop the bot
			_driveTrain->stop();
		}
		// Update local state tracker
		lastState = _active;
	}
	return _active;
}

/**
 * Reads the sensors and corrects drive direction.
 *
 * @param now The current millis() counter.
 */
void LineFollow::run(uint32_t now) {
	// Read the sensors
	int l = analogRead(_lPin);
	int r = analogRead(_rPin);
	int16_t correction = 0;

	D(F("Line Follower ") << "- l: " << l << "  ,r: " << r << "      \n");

	// If both sensors are below our min level, we have lost the line
	if (l<LINEFOL_MIN && r<LINEFOL_MIN) {
		D(F("Line Follower ") << F("lost line. Stopping.\n"));
		// Deactive line follower mode
		_active = false;
		// Stop the bot
		_driveTrain->stop();
		return;
	}
	// With one sensor less than min level (on 'white') and the other above min
	// level (on 'black'), we issue a correction
	if ((l<LINEFOL_MIN && r>=LINEFOL_MIN) || (r<LINEFOL_MIN && l>=LINEFOL_MIN)) {
		// The correction is the difference between right and left, mapped from
		// (-max to +max) to (-100 to 100). The correction value is the turn
		// direction (sign included) to street to get back on the line again.
		//correction = map(r-l, -LINEFOL_MAX, LINEFOL_MAX, -100, 100);
		correction = map(l-r, -LINEFOL_MAX, LINEFOL_MAX, -100, 100);
		D(F("Line Follower ") << F("correction: ") << correction << "               " << endl);

		// Send the correction to the drive train
		_driveTrain->direction(correction);
		return;
	}
	// If either sensor is now above max level, it means that at least one of
	// them are not on the track anymore, but probably both, so we stop
	if (l>LINEFOL_MAX || r>LINEFOL_MAX) {
		D(F("Line Follower ") << F("lost track. Stopping.\n"));
		// Deactive line follower mode
		_active = false;
		// Stop the bot
		_driveTrain->stop();
		return;
	}

	// If we get here, all is good and the bot is on the line.
}

