/**
 * Objects for controlling drive train.
 **/

#include "driveTrain.h"

// ####################### Wheel class definitions ######################

/**
 * Default contstructor
 **/
Wheel::Wheel() {
    // Preset pins and side to uninitialized
    _pinServo = _pinDir = _pinPWM = -1;
    _side = -1;
}

/**
 * Contstructor for creating a servo controlled wheel
 *
 * See config() method
 **/
Wheel::Wheel(uint8_t pinServo, uint8_t side) {
	// Call config
	config(pinServo, side);
}

/**
 * Contstructor for creating a HBridge/DC Motor controlled wheel
 *
 * See config() method
 **/
Wheel::Wheel(uint8_t pinDir, uint8_t pinPWM, uint8_t side) {
	// Call config
	config(pinDir, pinPWM, side);
}

/**
 * Configure method for servo controlled wheel.
 *
 * Set the pin the wheel is connected as well as the side of the robot it is
 * mounted on. Will set the wheel rotation to 0.
 *
 * @param pinServo The pin the servo is connected to
 * @param side The side (LEFT or RIGHT) the wheel is mounted
 **/
void Wheel::config(uint8_t pinServo, uint8_t side) {
    // Save pin and side
    _pinServo = pinServo;
    _side = side;

    // Attach the servo to the pin
    _servo.attach(_pinServo);
    // Set to stationary
    rotate(0);
}

/**
 * Configure method HBridge/DC Motor controlled wheel.
 *
 * Set the pins controling the wheel rotation as well as the side of the robot
 * it is mounted on. Will set the wheel rotation to 0.
 *
 * @param pinDir The pin for direction control on the HBridge
 * @param pinPWM The pin for speed control on the HBridge
 * @param side The side (LEFT or RIGHT) the wheel is mounted
 **/
void Wheel::config(uint8_t pinDir, uint8_t pinPWM, uint8_t side) {
    // Save pin and side
    _pinDir = pinDir;
    _pinPWM = pinPWM;
    _side = side;

    // Set up the pins
    pinMode(_pinDir, OUTPUT);
    pinMode(_pinPWM, OUTPUT); 
    // Set to stationary
    rotate(0);
}

/**
 * Makes the wheel rotate.
 *
 * The speed value determines the speed and direction to rotate the wheel in.
 * The speed is a value between 0 and 100 to indicate the percentage of full
 * speed. 0 is stop and 100 is full speed.
 *
 * To rotate forward, speed must be a positive value. To rotate backwards, speed
 * must be a negative value.
 *
 * Note that the rotation direction will be auto *mirrored* for LEFT and RIGHT
 * wheels. This allows the hardware connections to be made identical (not
 * mirrored) and we do the mirroring in software.
 *
 * @param speed The rotation speed: 0%-100% with a positive value rotates forward
 *        and a negative value rotates backward.
 */
void Wheel::rotate(int8_t speed) {
    // Validate the speed
    if (speed<-100 || speed>100) return;

    // Are we driving a servo?
    if(_pinServo!=1) {
        uint8_t angle;

        // Map the speed value between -100 (fullspeed reverse) and 100 (fullspeed
        // ahead) to an angle between 0 (fullspeed rotate left) to 180 (fullspeed
        // rotate right) - This is for the right motor... the left motor mirrors
        // the angle.
        if (_side==RIGHT) {
            angle = map(speed, -100, 100, 0, 180);
        } else {
            angle = map(speed, 100, -100, 0, 180);
        }

        // Set the speed and direction
        _servo.write(angle);
    } else {
        // We're driving an HBridge.
        uint8_t dir, pwm;

        // We model the direction and speed control based on an L9110 HBridge
        // driver using the following truth table:
        //
        //   Input ||  Output || 
        // IA | IB || OA | OB || Description
        // ---+----++----+----++------------
        // L  | L  || L  | L  || Motor off
        // L  | H  || L  | H  || Forward
        // H  | L  || H  | L  || Reverse
        // H  | H  || H  | H  || Motor Off
        //
        // We will assume the A input to be the direction control which we will
        // set to LOW or HIGH for reverse and forward respectively using
        // digitalWrite.  This will set the one side of the motor to GND or VCC
        // and then allows the other pin to do PWM which will result in a PWM
        // signal on the other side of the motor connection. Keep in mind the
        // low current PWM on the B *input* gets *switched* by the HBridge to a
        // high wattage PWM *output* to the motor.
        
        // Determine the direction: Negative speed is reverse, positive is
        // forward for the RIGHT wheel, and the oposite for the LEFT wheel.
        dir = speed<0 ? (_side==RIGHT ? HIGH : LOW) : (_side==RIGHT ? LOW : HIGH);

        // Determine the PWM duty cycle. When rotating forward (dir/IA is LOW),
        // the speed pin, IB, needs a *high* duty cycle PWM for higher speeds,
        // and a *low* duty cycle PWM signal for slower speeds.
        // When rotating in reverse (dir/IA is HIGH), the opposite is true, and
        // the PWM duty cycle should be *low* for higher speeds, and *high* for
        // lower speeds.
        // Map the duty cycle based on the rotation direction
        if (dir==LOW) {
            pwm = map(abs(speed), 0, 100, 0, 255);
        } else {
            pwm = map(abs(speed), 0, 100, 255, 0);
        }
        // Set the speed and direction
        digitalWrite(_pinDir, dir);
        analogWrite(_pinPWM, pwm);
    }
}


// ####################### DriveTrain class definitions ######################

/**
 * Contstructor
 **/
DriveTrain::DriveTrain(uint8_t pinLeft, uint8_t pinRight) {
	// Default speed to 0 and reset bumpers
	_speed = 0;
	_bumpers = 0;
	// Configure the wheels
	_wheel[LEFT].config(pinLeft, LEFT);
	_wheel[RIGHT].config(pinRight, RIGHT);
};

/**
 * Update the wheel rotation based on the current direction and speed.
 **/
void DriveTrain::_update() {
    // Left and right relative speeds
    int8_t leftRel, rightRel;

	// If any bumper is set, do nothing here
	if (_bumpers) return;

    // See the MovementControl docs for more info.
    // For a positive direction (forward or turning right), the left wheel
    // relative speed is at 100%, while the right wheel needs to be mapped
    // between 100 and -100 %
    if (_dir>=0) {
        leftRel = 100;
        rightRel = map(_dir, 0, 100, 100, -100);
    } else {
        // For turning to the left, the right wheel rel speed is 100% while
        // the left wheel needs to be mapped between 100 and -100 %
        rightRel = 100;
        leftRel = map(_dir, 0, -100, 100, -100);
    }

    // The left and right wheel speeds are now the relative percentages of the
    // current speed setting.
    _sLeft = ((int16_t)_speed*leftRel)/100;
    _sRight = ((int16_t)_speed*rightRel)/100;

    // Update the wheels
	_wheel[LEFT].rotate(_sLeft);
	_wheel[RIGHT].rotate(_sRight);
};

/**
 * Sets full speed forward
 **/
void DriveTrain::forward() {
    // Set speed to 100% and direction to 0
    _speed = 100;
    _dir = 0;
    // Update
    _update();
}

/**
 * Sets full speed reverse
 **/
void DriveTrain::reverse() {
    // Set speed to -100% and direction to 0
    _speed = -100;
    _dir = 0;
    // Update
    _update();
}

/**
 * Stops motion by setting speed to 0
 **/
void DriveTrain::stop() {
    // Set speed to 0, but leave current direction
    _speed = 0;
    // Update
    _update();
}

/**
 * Adjust direction by TURN_STEPs to the left
 **/
void DriveTrain::left() {
    // Adjust direction
    _dir -= TURN_STEP;
    // Stick to limits
    if (_dir < MAX_LEFT) _dir = MAX_LEFT;
    // Update
    _update();
}

/**
 * Adjust direction by TURN_STEPs to the right
 **/
void DriveTrain::right() {
    // Adjust direction
    _dir += TURN_STEP;
    // Stick to limits
    if (_dir > MAX_RIGHT) _dir = MAX_RIGHT;
    // Update
    _update();
}

/**
 * Adjust direction by the supplied direction.
 *
 * @param dir A direction value between MAX_LEFT (full turn left) and MAX_RIGHT
 *        (full turn right), with 0 being going straight forward. See diagrams
 *        in docs.
 **/
void DriveTrain::direction(int16_t dir) {
    // Ignore an invalid direction
	if(dir<MAX_LEFT || dir>MAX_RIGHT) return;
	// Set the new direction.
    _dir = dir;
    // Update
    _update();
}

/**
 * Speed up by SPEED_STEPs
 **/
void DriveTrain::speedUp() {
	// TODO: Speed should be between 0 and 100%, not MIN and MAX_SPEED
    // Adjust speed
    _speed += SPEED_STEP;
    // Stick to limits
    if (_speed > MAX_SPEED) _speed = MAX_SPEED;
    // Update
    _update();
}

/**
 * Slow down by SPEED_STEPs
 **/
void DriveTrain::slowDown() {
	// TODO: Speed should be between 0 and 100%, not MIN and MAX_SPEED
    // Adjust speed
    _speed -= SPEED_STEP;
    // Stick to limits
    if (_speed < MIN_SPEED) _speed = MIN_SPEED;
    // Update
    _update();
}

/**
 * Set the speed.
 *
 * @param speed A speed value as percentage of full speed.
 **/
void DriveTrain::setSpeed(uint16_t speed) {
	// Set the speed
    _speed = speed;
    // Update
    _update();
}

/**
 * Interface to update the local bumpers state indicator.
 *
 * @param bumpers An 8 bit integer where each bumper state is represented by a
 *        specific bit. The BUMP_FL, BUMP_FR, etc. values defines the bit in
 *        this value that represents each bumper. If the bit is set, the bumper
 *        is active (bumped), if reset, then the bumper is clear of obstacles.
 **/
void DriveTrain::bumpState(uint8_t bumpers) {
    // Update bumper states
    _bumpers = bumpers;
    // Stop the wheels without changing current speed or direction if a bumper
	// is active, and resume travel if not.
	if(_bumpers) {
		_wheel[LEFT].rotate(0);
		_wheel[RIGHT].rotate(0);
	} else {
		_update();
	}
}

