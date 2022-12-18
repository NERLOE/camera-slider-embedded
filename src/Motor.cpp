#include <Motor.h>

// Constructor for StepperMotor class
StepperMotor::StepperMotor(int directionPin, int stepPin, int enabledPin) {
    this->dirPin = directionPin;
    this->stepPin = stepPin;
    this->enabledPin = enabledPin;

    // Set pin modes
    pinMode(this->dirPin, OUTPUT);
    pinMode(this->stepPin, OUTPUT);
    pinMode(this->enabledPin, OUTPUT);

    // Set initial direction and disable motor
    this->setDirection(this->direction);
    this->setEnabled(false);

    // Calculate initial time per step based on timeToTarget and targetPosition
    this->timePerStep = ((double)this->timeToTarget / (double)this->targetPosition) * 1000;
};

void StepperMotor::run() {
    if (!this->isEnabled) return;

    long mics = micros();
    if (mics - this->prevStepMicros >= _max(FASTEST_STEP_TIME, this->timePerStep)) {
        int distanceToTarget = this->getDistanceToTarget();

        if (this->targetPosition < this->motorPosition && this->direction == 1) {
            this->setDirection(0);
        } else if (this->targetPosition > this->motorPosition && this->direction == 0) {
            this->setDirection(1);
        }

        if (distanceToTarget > 0) {
            this->prevStepMicros = mics;
            this->singleStep();
        } else if (this->targetPosition) {
            Serial.println("Distance to target: " + String(distanceToTarget) + " - targetPosition: " + String(this->targetPosition) + " - motorPosition: " + String(this->motorPosition));
            // this->setDirection(this->direction == 1 ? 0 : 1);
            //  this->setTargetPosition(this->targetPosition == 0 ? STEPS_PER_REVOLUTION * MICRO_STEPS * 10 : 0);
            delay(1000);
        }
    }
};

void StepperMotor::setTargetPosition(int targetPosition, int timeToTarget) {
    this->targetPosition = targetPosition;
    this->timeToTarget = timeToTarget;
    this->timePerStep = timeToTarget == -1 ? 100 : ((double)this->timeToTarget / (double)this->targetPosition) * 1000;
};

int StepperMotor::getDistanceToTarget() {
    return this->direction == 1 ? this->targetPosition - this->motorPosition : this->motorPosition - this->targetPosition;
};

double StepperMotor::getTimeLeft() {
    // Calculate the time left to reach the target position in milliseconds
    return (double)this->timeToTarget - ((double)this->motorPosition / (double)this->targetPosition) * (double)this->timeToTarget;
};

void StepperMotor::setEnabled(bool enabled) {
    this->isEnabled = enabled;
    digitalWrite(this->enabledPin, enabled ? LOW : HIGH);
};

void StepperMotor::setDirection(int dir) {
    this->direction = dir;
    digitalWriteFast(this->dirPin, dir == 1 ? HIGH : LOW);
};

void StepperMotor::singleStep() {
    digitalWriteFast(this->stepPin, HIGH);
    digitalWriteFast(this->stepPin, LOW);

    this->motorPosition += this->direction == 1 ? 1 : -1;
}

void distanceBetweenSteps() {
}