#pragma once
#include <Arduino.h>
#include <Settings.h>
#include <digitalWriteFast.h>

// The fastest step time in microseconds (1/1000 of a millisecond)
#define FASTEST_STEP_TIME 34

// Constants
const int STEPS_PER_REVOLUTION = 200;
const int MICRO_STEPS = 16;

enum Direction {
    FORWARD = 1,
    BACKWARD = 0,
};

class StepperMotor {
   private:
    int dirPin;
    int stepPin;
    int enabledPin;

    // Variables
    long prevStepMicros = 0;
    Direction direction = FORWARD;

    bool isCalibrating = false;

    // Calculate the time in microseconds to move one step
    double timePerStep;

    void setDirection(Direction dir);
    void singleStep();

   public:
    // Variables
    int motorPosition = 0;
    // Target position (in steps)
    int targetPosition = 0;

    // Time to reach target position (in milliseconds)
    double timeToTarget = 100;

    // Whether the motor is enabled
    int isEnabled = false;

    StepperMotor(int directionPin, int stepPin, int enabledPin);

    void run();

    double getTimeLeft();

    int getDistanceToTarget();

    void setEnabled(bool enabled);

    void moveTo(int targetPosition, double timeToTarget = -1);

    Direction getDirection();
};