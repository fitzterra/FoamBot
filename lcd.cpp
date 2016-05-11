/**
 * Task based LCD display
 */

#include "lcd.h"

#define INVERT false	// If display should be inverted or not
#define CONTRAST 0xB4	// Set VOP value: 00h-7Fh. RTFM!
#define TEMPCOEF 0x04	// Temperature coeficient. Leave at default. RTFM!
#define BIAS 0x012		// Leave at default. RTFM!

// ####################### LCD class definitions ######################

/**
 * Constructor.
 */
LCD::LCD(uint32_t rate, CommandConsumer *cc, DriveTrain *dt, LineFollow *lf)
: TimedTask(millis()) {
    // Set locals
    _comCon = cc;
    _driveTrain = dt;
    _lineFol = lf;
    _updateRate = rate;

	// Initialize the LCD
    _lcd.begin(INVERT, CONTRAST, TEMPCOEF, BIAS);
}

LCD::LCD(uint32_t rate)
: TimedTask(millis()) {
    // Set locals
    _comCon = 0;
    _driveTrain = 0;
    _lineFol = 0;
    _updateRate = rate;

	// Initialize the LCD
    _lcd.begin(INVERT, CONTRAST, TEMPCOEF, BIAS);
}
/**
 * Update the LCD
 *
 * @param now The current millis() counter.
 */
void LCD::run(uint32_t now) {
	String s;
    int lfLeft, lfRight;

	// Update the current mode
    _lcd.gotoXY(0,0);
    _lcd.print(_lineFol->isActive() ? F("Line Follow") : F("Normal     "));

    // Update the last command by fetching it from the command names array,
	// padding it with 14 spaces to clear the full line, and then only printing
	// a substring of 14 chars (lenght of a line) to the LCD.
	s = String(_comCon->lastCommand()) + "              ";
    _lcd.gotoXY(0,1);
	_lcd.print(s.substring(0, 14));

    // Update the speed and direction
    _lcd.gotoXY(0, 2);
    s = String(_driveTrain->getSpeed()) + "       ";
    s = s.substring(0, 7) + String(_driveTrain->getDirection()) + "       ";
    _lcd.print(s.substring(0, 14));

    // Update the line follower sensor values if we're in line follower mode
    //if (_lineFol->isActive()) {
        // Get the linefollower sensor values
        _lineFol->senseVals(&lfLeft, &lfRight);
        // Set up the output string
        s = String(lfLeft, DEC) + " : " + String(lfRight, DEC) + "          ";
        s = s.substring(0, 14);
    //} else {
        // Clear the line
    //    s = String("              ");
    //}
    _lcd.gotoXY(0, 3);
    _lcd.print(s);



	_lcd.gotoXY(0, 5);
	_lcd.print(now);

    // Run again in the required number of milliseconds.
    incRunTime(_updateRate);
}


