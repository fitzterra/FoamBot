/**
 * Task based Bumpers Sensors
 */

#include "bumpers.h"

// ####################### Bumpers class definitions ######################

/**
 * Constructor.
 */
Bumpers::Bumpers(uint8_t pinFL, uint8_t pinFR, DriveTrain *driveTrain) : Task() {
    // Save pins and drive tain
    _fl = pinFL;
    _fr = pinFR;
    _driveTrain = driveTrain;

	// Preset state bits to not bumped
	_state = 0;

	// Open the serial port with default speed.
	OpenSerial();

	// Set the bumper pins to use internal pull ups
	pinMode(_fl, INPUT_PULLUP);
	pinMode(_fr, INPUT_PULLUP);
    // Set the bumper LED pin as output and LOW
    // TODO: Should probably pass this pin in as an optional indicator pin
    //       and if not a valid pin number, to ignore the indicator
    //       functionality??
    pinMode(BUMP_LED_PIN, OUTPUT);
    digitalWrite(BUMP_LED_PIN, LOW);

	// DEBUG
    D(F("Starting bumpers task...\n"));
}

/**
 * Read the bumpers and see if the state has change
 */
bool Bumpers::canRun(uint32_t now) {
	uint8_t state;
	bool changed;

	// Read the bumpers. See the wiki doc for info on how
    // we use XNOR here to ensure that the state is always
    // HIGH for a bumped (actived), and low for a non-bumped
    // sensor.
	state = 0 | \
            (!(digitalRead(_fl) ^ BUMPED))<<BUMP_FL | \
            (!(digitalRead(_fr) ^ BUMPED))<<BUMP_FR;

	// Determine if there was any change in state
	changed = state!=_state;

	// Save current state
	_state = state;

	// Indicate whether we should run or not
	return changed;
}

/**
 * Update the robot movement
 *
 * @param now THe current millis() counter.
 */
void Bumpers::run(uint32_t now) {
	
    // Update the driveTrain's bumper indicators.
    _driveTrain->bumpState(_state);
    // Update the indicator LED pin
    if(_state) {
        digitalWrite(BUMP_LED_PIN, HIGH);
    } else {
        digitalWrite(BUMP_LED_PIN, LOW);
    }

    D(F("Bumper state changed - Left:") << _DEC(_state&BUMP_FL) << \
	  F(" ,  Right: ") << _DEC(_state&BUMP_FR) << endl);
}


