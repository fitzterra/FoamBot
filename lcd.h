/**
 * Task based LCD display
 *
 * The Nokia 5110/PCD8544 LCD uses the "Fast PCD8544 Library" by TheCoolest.
 *   see: http://forum.arduino.cc/index.php?topic=176794.0
 * 
 * The control pins used by default is:
 * PIN_DC    D8 
 * PIN_RESET D9 
 * PIN_SCE   D10 
 * PIN_SDIN  D11 
 * PIN_SCLK  D13 
 *
 * See the library for more details.
 */

#ifndef _LCD_H_
#define _LCD_H_

#include <stdint.h>
#include "config.h"
#include "debug.h"
#include "utils.h"
#include "control.h"
#include "driveTrain.h"
#include "lineFollow.h"
#include <SPI.h>
#include "PCD8544_SPI.h"
#include <Task.h>

#ifdef _DEBUG
#include <Streaming.h>
#endif // _DEBUG


/**
 * Task to update LCD display
 */
class LCD : public TimedTask {
    private:
        PCD8544_SPI _lcd;           // LCD instance. NOT framebuffer for now.
        CommandConsumer *_comCon;   // Pointer to command consumer task
        DriveTrain *_driveTrain;    // Pointer to drive train object
        LineFollow *_lineFol;     // Pointer to line follower task
        uint32_t _updateRate;       // Rate at which to update the display in millis

    public:
		LCD(uint32_t rate);
        LCD(uint32_t rate, CommandConsumer *cc, DriveTrain *dt, LineFollow *lf);
		virtual void run(uint32_t now);
};

#endif  //_LCD_H_
