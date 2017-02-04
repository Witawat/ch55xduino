/*
 * Stepper.c - Stepper library for Wiring/Arduino - Version 1.1.0
 *
 * Original library        (0.1)   by Tom Igoe.
 * Two-wire modifications  (0.2)   by Sebastian Gassner
 * Combination version     (0.3)   by Tom Igoe and David Mellis
 * Bug fix for four-wire   (0.4)   by Tom Igoe, bug fix from Noah Shibley
 * High-speed stepping mod         by Eugene Kozlenko
 * Timer rollover fix              by Eugene Kozlenko
 * Five phase five wire    (1.1.0) by Ryan Orendorff
 * Ported to C syntax      (1.1.0) by Michael Mayer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Drives a unipolar, bipolar, or five phase stepper motor.
 *
 * When wiring multiple stepper motors to a microcontroller, you quickly run
 * out of output pins, with each motor requiring 4 connections.
 *
 * By making use of the fact that at any time two of the four motor coils are
 * the inverse of the other two, the number of control connections can be
 * reduced from 4 to 2 for the unipolar and bipolar motors.
 *
 * A slightly modified circuit around a Darlington transistor array or an
 * L293 H-bridge connects to only 2 microcontroler pins, inverts the signals
 * received, and delivers the 4 (2 plus 2 inverted ones) output signals
 * required for driving a stepper motor. Similarly the Arduino motor shields
 * 2 direction pins may be used.
 *
 * The sequence of control signals for 5 phase, 5 control wires is as follows:
 *
 * Step C0 C1 C2 C3 C4
 *    1  0  1  1  0  1
 *    2  0  1  0  0  1
 *    3  0  1  0  1  1
 *    4  0  1  0  1  0
 *    5  1  1  0  1  0
 *    6  1  0  0  1  0
 *    7  1  0  1  1  0
 *    8  1  0  1  0  0
 *    9  1  0  1  0  1
 *   10  0  0  1  0  1
 *
 * The sequence of control signals for 4 control wires is as follows:
 *
 * Step C0 C1 C2 C3
 *    1  1  0  1  0
 *    2  0  1  1  0
 *    3  0  1  0  1
 *    4  1  0  0  1
 *
 * The sequence of controls signals for 2 control wires is as follows
 * (columns C1 and C2 from above):
 *
 * Step C0 C1
 *    1  0  1
 *    2  1  1
 *    3  1  0
 *    4  0  0
 *
 * The circuits can be found at
 *
 * http://www.arduino.cc/en/Reference/Stepper
 */

#include "Arduino.h"
#include "Stepper.h"

#if 0
/*
 * two-wire constructor.
 * Sets which wires should control the motor.
 */
Stepper::Stepper(int number_of_steps, int motor_pin_1, int motor_pin_2)
{
  this->step_number = 0;    // which step the motor is on
  this->direction = 0;      // motor direction
  this->last_step_time = 0; // time stamp in us of the last step taken
  this->number_of_steps = number_of_steps; // total number of steps for this motor

  // Arduino pins for the motor control connection:
  this->motor_pin_1 = motor_pin_1;
  this->motor_pin_2 = motor_pin_2;

  // setup the pins on the microcontroller:
  pinMode(this->motor_pin_1, OUTPUT);
  pinMode(this->motor_pin_2, OUTPUT);

  // When there are only 2 pins, set the others to 0:
  this->motor_pin_3 = 0;
  this->motor_pin_4 = 0;
  this->motor_pin_5 = 0;

  // pin_count is used by the stepMotor() method:
  this->pin_count = 2;
}
#endif
#if 0
/*
 *   constructor for four-pin version
 *   Sets which wires should control the motor.
 */
Stepper::Stepper(int number_of_steps, int motor_pin_1, int motor_pin_2,
                                      int motor_pin_3, int motor_pin_4)
{
  this->step_number = 0;    // which step the motor is on
  this->direction = 0;      // motor direction
  this->last_step_time = 0; // time stamp in us of the last step taken
  this->number_of_steps = number_of_steps; // total number of steps for this motor

  // Arduino pins for the motor control connection:
  this->motor_pin_1 = motor_pin_1;
  this->motor_pin_2 = motor_pin_2;
  this->motor_pin_3 = motor_pin_3;
  this->motor_pin_4 = motor_pin_4;

  // setup the pins on the microcontroller:
  pinMode(this->motor_pin_1, OUTPUT);
  pinMode(this->motor_pin_2, OUTPUT);
  pinMode(this->motor_pin_3, OUTPUT);
  pinMode(this->motor_pin_4, OUTPUT);

  // When there are 4 pins, set the others to 0:
  this->motor_pin_5 = 0;

  // pin_count is used by the stepMotor() method:
  this->pin_count = 4;
}
#endif

/*
 *   constructor for five phase motor with five wires
 *   Sets which wires should control the motor.
 */
Stepper Stepper_5phase(
        unsigned int number_of_steps,
        unsigned char motor_pin_1,
        unsigned char motor_pin_2,
        unsigned char motor_pin_3,
        unsigned char motor_pin_4,
        unsigned char motor_pin_5)
{
  Stepper this;	// pointer to struct Stepper_s

  // reserve memory for motor data, init all to zero:
  this = calloc(1, sizeof(struct Stepper_s));

//  this->step_number = 0;    // which step the motor is on
//  this->direction = 0;      // motor direction
//  this->last_step_time = 0; // time stamp in us of the last step taken
  this->number_of_steps = number_of_steps; // total number of steps for this motor

  // set Arduino pins for the first two phases:
  this->motor_pin_1 = motor_pin_1;
  this->motor_pin_2 = motor_pin_2;

  pinMode(this->motor_pin_1, OUTPUT);
  pinMode(this->motor_pin_2, OUTPUT);

  // check if there are more phases:
  if (motor_pin_3 == 0) {
    // no, just two phases
    // pin_count is used by the stepMotor() method:
    this->pin_count = 2;
  } else {
    // we have at least 4 phases, maybe even 5.
    // start by setting Arduino pins for phases 3 and 4:
    this->motor_pin_3 = motor_pin_3;
    this->motor_pin_4 = motor_pin_4;
    pinMode(this->motor_pin_3, OUTPUT);
    pinMode(this->motor_pin_4, OUTPUT);
    // check for 5 phases:
    if (motor_pin_5 == 0) {
      // pin_count is used by the stepMotor() method:
      this->pin_count = 4;
    } else {
      // set Arduino pin for phase 5:
      this->motor_pin_5 = motor_pin_5;
      pinMode(this->motor_pin_5, OUTPUT);
      // pin_count is used by the stepMotor() method:
      this->pin_count = 5;
    }
  }

  return this;
}

/*
 * Sets the speed in revs per minute
 */
void Stepper_setSpeed(Stepper this, long whatSpeed)
{
  this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / whatSpeed;
}

/*
 * Moves the motor steps_to_move steps.  If the number is negative,
 * the motor moves in the reverse direction.
 */
void Stepper_step(Stepper this, int steps_to_move)
{
  int steps_left = abs(steps_to_move);  // how many steps to take

  // determine direction based on whether steps_to_mode is + or -:
  if (steps_to_move > 0) { this->direction = 1; }
  if (steps_to_move < 0) { this->direction = 0; }


  // decrement the number of steps, moving one step each time:
  while (steps_left > 0)
  {
    unsigned long now = micros();
    // move only if the appropriate delay has passed:
    if (now - this->last_step_time >= this->step_delay)
    {
      // get the timeStamp of when you stepped:
      this->last_step_time = now;
      // increment or decrement the step number,
      // depending on direction:
      if (this->direction == 1)
      {
        this->step_number++;
        if (this->step_number == this->number_of_steps) {
          this->step_number = 0;
        }
      }
      else
      {
        if (this->step_number == 0) {
          this->step_number = this->number_of_steps;
        }
        this->step_number--;
      }
      // decrement the steps left:
      steps_left--;
      // step the motor to step number 0, 1, ..., {3 or 10}
      if (this->pin_count == 5)
        Stepper_stepMotor(this, this->step_number % 10);
      else
        Stepper_stepMotor(this, this->step_number % 4);
    }
  }
}

/*
 * Moves the motor forward or backwards.
 */
void Stepper_stepMotor(Stepper this, int thisStep)
{
  if (this->pin_count == 2) {
    switch (thisStep) {
      case 0:  // 01
        digitalWrite(this->motor_pin_1, LOW);
        digitalWrite(this->motor_pin_2, HIGH);
      break;
      case 1:  // 11
        digitalWrite(this->motor_pin_1, HIGH);
        digitalWrite(this->motor_pin_2, HIGH);
      break;
      case 2:  // 10
        digitalWrite(this->motor_pin_1, HIGH);
        digitalWrite(this->motor_pin_2, LOW);
      break;
      case 3:  // 00
        digitalWrite(this->motor_pin_1, LOW);
        digitalWrite(this->motor_pin_2, LOW);
      break;
    }
  }
  if (this->pin_count == 4) {
    switch (thisStep) {
      case 0:  // 1010
        digitalWrite(this->motor_pin_1, HIGH);
        digitalWrite(this->motor_pin_2, LOW);
        digitalWrite(this->motor_pin_3, HIGH);
        digitalWrite(this->motor_pin_4, LOW);
      break;
      case 1:  // 0110
        digitalWrite(this->motor_pin_1, LOW);
        digitalWrite(this->motor_pin_2, HIGH);
        digitalWrite(this->motor_pin_3, HIGH);
        digitalWrite(this->motor_pin_4, LOW);
      break;
      case 2:  //0101
        digitalWrite(this->motor_pin_1, LOW);
        digitalWrite(this->motor_pin_2, HIGH);
        digitalWrite(this->motor_pin_3, LOW);
        digitalWrite(this->motor_pin_4, HIGH);
      break;
      case 3:  //1001
        digitalWrite(this->motor_pin_1, HIGH);
        digitalWrite(this->motor_pin_2, LOW);
        digitalWrite(this->motor_pin_3, LOW);
        digitalWrite(this->motor_pin_4, HIGH);
      break;
    }
  }

  if (this->pin_count == 5) {
    switch (thisStep) {
      case 0:  // 01101
        digitalWrite(this->motor_pin_1, LOW);
        digitalWrite(this->motor_pin_2, HIGH);
        digitalWrite(this->motor_pin_3, HIGH);
        digitalWrite(this->motor_pin_4, LOW);
        digitalWrite(this->motor_pin_5, HIGH);
        break;
      case 1:  // 01001
        digitalWrite(this->motor_pin_1, LOW);
        digitalWrite(this->motor_pin_2, HIGH);
        digitalWrite(this->motor_pin_3, LOW);
        digitalWrite(this->motor_pin_4, LOW);
        digitalWrite(this->motor_pin_5, HIGH);
        break;
      case 2:  // 01011
        digitalWrite(this->motor_pin_1, LOW);
        digitalWrite(this->motor_pin_2, HIGH);
        digitalWrite(this->motor_pin_3, LOW);
        digitalWrite(this->motor_pin_4, HIGH);
        digitalWrite(this->motor_pin_5, HIGH);
        break;
      case 3:  // 01010
        digitalWrite(this->motor_pin_1, LOW);
        digitalWrite(this->motor_pin_2, HIGH);
        digitalWrite(this->motor_pin_3, LOW);
        digitalWrite(this->motor_pin_4, HIGH);
        digitalWrite(this->motor_pin_5, LOW);
        break;
      case 4:  // 11010
        digitalWrite(this->motor_pin_1, HIGH);
        digitalWrite(this->motor_pin_2, HIGH);
        digitalWrite(this->motor_pin_3, LOW);
        digitalWrite(this->motor_pin_4, HIGH);
        digitalWrite(this->motor_pin_5, LOW);
        break;
      case 5:  // 10010
        digitalWrite(this->motor_pin_1, HIGH);
        digitalWrite(this->motor_pin_2, LOW);
        digitalWrite(this->motor_pin_3, LOW);
        digitalWrite(this->motor_pin_4, HIGH);
        digitalWrite(this->motor_pin_5, LOW);
        break;
      case 6:  // 10110
        digitalWrite(this->motor_pin_1, HIGH);
        digitalWrite(this->motor_pin_2, LOW);
        digitalWrite(this->motor_pin_3, HIGH);
        digitalWrite(this->motor_pin_4, HIGH);
        digitalWrite(this->motor_pin_5, LOW);
        break;
      case 7:  // 10100
        digitalWrite(this->motor_pin_1, HIGH);
        digitalWrite(this->motor_pin_2, LOW);
        digitalWrite(this->motor_pin_3, HIGH);
        digitalWrite(this->motor_pin_4, LOW);
        digitalWrite(this->motor_pin_5, LOW);
        break;
      case 8:  // 10101
        digitalWrite(this->motor_pin_1, HIGH);
        digitalWrite(this->motor_pin_2, LOW);
        digitalWrite(this->motor_pin_3, HIGH);
        digitalWrite(this->motor_pin_4, LOW);
        digitalWrite(this->motor_pin_5, HIGH);
        break;
      case 9:  // 00101
        digitalWrite(this->motor_pin_1, LOW);
        digitalWrite(this->motor_pin_2, LOW);
        digitalWrite(this->motor_pin_3, HIGH);
        digitalWrite(this->motor_pin_4, LOW);
        digitalWrite(this->motor_pin_5, HIGH);
        break;
    }
  }
}

/*
  version() returns the version of the library:
*/
/* replaced with a simple #define
int Stepper_version(void)
{
  return 5;
}
*/