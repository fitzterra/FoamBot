/**
 * Objects for controlling drive train.
 **/


#ifndef _DRIVETRAIN_H_
#define _DRIVETRAIN_H_

#include <stdint.h>
#include <Arduino.h>
#include <Servo.h>
#include "Streaming.h"
#include "config.h"
#include "debug.h"

#define SPEED_STEP 5    // Increments for speed changes 
#define TURN_STEP 5     // Increments for turning left or right
#define MAX_LEFT -100   // Max value for the left direction
#define MAX_RIGHT 100   // Max value for the right direction
#define MAX_SPEED 100   // Max speed value
#define MIN_SPEED -100  // Min speed value

/**
 * Class for controlling a wheel.
 *
 * The wheel is assumed to a continues servo motor.
 **/
class Wheel {
    private:
        uint8_t _pin;       // The pin the servo is connected to
        uint8_t _side;      // Which side the wheel is located on. One of LEFT or RIGHT
        Servo _servo;       // The servo object

    public:
		Wheel();			// Default constructor
        Wheel(uint8_t pin, uint8_t side);
        void config(uint8_t pin, uint8_t side);
        void rotate(int8_t speed);
};

/**
 * Class that combines wheels into a single drive train control
 **/
class DriveTrain {
	private:
		Wheel _wheel[2];	// Left and Right wheels
		int8_t _speed;		// Current relative speed as percentage of full speed
		int8_t _dir;		// Direction of travel, -100 to 100. See MovementControl docs.
		uint8_t _sLeft, _sRight;	// Exact left/right wheel speed
		uint8_t _bumpers;	// Bitwise bumpers status indicator

        void _update();     // Updates the wheel rotation from speed and dir.

	public:
		DriveTrain(uint8_t pinLeft, uint8_t pinRight);
		void forward();
		void reverse();
		void stop();
        void left();
        void right();
        void direction(int16_t dir);
        void speedUp();
        void slowDown();
		void setSpeed(uint16_t speed);
		void bumpState(uint8_t bumpers); 
        int8_t getSpeed() {return _speed;};
        int8_t getDirection() {return _dir;};
		void info() {Serial << F("Hello for dt\n"); };
};

#endif // _DRIVETRAIN_H_
