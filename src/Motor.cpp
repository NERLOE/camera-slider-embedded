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
    this->timePerStep = (this->timeToTarget / (double)this->targetPosition) * 1000;
};

void StepperMotor::run() {
    if (!this->isEnabled) return;

    long mics = micros();
    if (mics - this->prevStepMicros >= _max(FASTEST_STEP_TIME, this->timePerStep)) {
        int distanceToTarget = this->getDistanceToTarget();

        if (this->targetPosition < this->motorPosition && this->direction == 1) {
            this->setDirection(BACKWARD);
        } else if (this->targetPosition > this->motorPosition && this->direction == 0) {
            this->setDirection(FORWARD);
        }

        if (distanceToTarget > 0) {
            this->prevStepMicros = mics;
            this->singleStep();
        } else if (this->targetPosition) {
            Serial.println("Distance to target: " + String(distanceToTarget) + " - targetPosition: " + String(this->targetPosition) + " - motorPosition: " + String(this->motorPosition));
            // this->setEnabled(false);
            delay(1000);
        }
    }
};

void StepperMotor::moveTo(int targetPosition, double timeToTarget) {
    this->targetPosition = targetPosition;
    this->timeToTarget = timeToTarget;

    int distanceToTarget = this->getDistanceToTarget();
    this->timePerStep = timeToTarget == -1 ? 100 : _max(FASTEST_STEP_TIME, (this->timeToTarget / (double)distanceToTarget) * 1000);

    if (timeToTarget) {
        Serial.println("Moving motor from " + String(this->motorPosition) + " to " + String(this->targetPosition) + " (" + String(distanceToTarget) + " steps) in " + String((this->timePerStep * distanceToTarget) / 1000) + "ms - " + String(this->timePerStep / 1000) + "ms per step");
    }

    if (!this->isEnabled) {
        this->setEnabled(true);
    }
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

void StepperMotor::setDirection(Direction dir) {
    this->direction = dir;
    digitalWriteFast(this->dirPin, dir == 1 ? HIGH : LOW);
};

void StepperMotor::singleStep() {
    digitalWriteFast(this->stepPin, HIGH);
    digitalWriteFast(this->stepPin, LOW);

    this->motorPosition += this->direction == 1 ? 1 : -1;
}

Direction StepperMotor::getDirection() {
    return this->direction;
}