#include "config.h"
#include "debug.h"
#include "control.h"
#include "lineFollow.h"
#include "utils.h"
#include <TaskScheduler.h>
#include "Streaming.h"
#include "lcd.h"
#include "commands.h"
#include "bumpers.h"
#include "driveTrain.h"

#ifdef DEBUG
#include "MemoryFree.h"
#endif // DEBUG


void setup() {
	OpenSerial();
	// Debug
	D(__FILE__<<":"<<__LINE__<<F("# ")<< F("Free memory:") << freeMemory() << endl);

	// Load the command maps from EEPROM
	loadCmdMaps();
}

void loop() {
    // Create the drive train
    DriveTrain driveTrain(SERVO_LEFT, SERVO_RIGHT);

    // Create the tasks.
	SerialIn serialInput;
	IrIn irInput(IR_PIN);
	InputDecoder decoder(&serialInput, &irInput);
	LineFollow lineFollow(LINEFOL_LEFT, LINEFOL_RIGHT, &driveTrain);
	CommandConsumer comCon(&decoder, &driveTrain, &lineFollow);
	Bumpers bumpers(BUMP_FL_PIN, BUMP_FR_PIN, &driveTrain);
    LCD lcd(LCD_RATE, &comCon, &driveTrain, &lineFollow);
    
    // Initialise the task list and scheduler.
    Task *tasks[] = {&lcd, &serialInput, &irInput, &decoder, &comCon,
					 &bumpers, &lineFollow};
    TaskScheduler sched(tasks, NUM_TASKS(tasks));

    // Run the scheduler - never returns.
    sched.run();
}
