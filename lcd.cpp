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
	// Update the current mode
    _lcd.gotoXY(0,0);
    _lcd.print("Testing LCD...\n");
	_lcd.print(now);


    // Run again in the required number of milliseconds.
    incRunTime(_updateRate);
}


