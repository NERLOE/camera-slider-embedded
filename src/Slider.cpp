#include <Bluetooth.h>
#include <Slider.h>

SliderController sliderController;
StepperMotor sliderStepper(SLIDER_MOTOR_dirPin, SLIDER_MOTOR_stepPin, SLIDER_MOTOR_enabledPin);

SliderController::SliderController() {
}

void SliderController::init() {
    this->startCalibration();
}

long lastSliderInfoUpdate = 0;
void SliderController::run() {
    if (this->state != IDLE) {
        sliderStepper.run();

        long mics = micros();
        if (mics - lastSliderInfoUpdate >= 1000000) {
            lastSliderInfoUpdate = mics;

            this->updateSliderInfo();
        }
    }
}

void SliderController::startCalibration() {
    if (this->state != IDLE) return;

    Serial.println("Starting calibration...");
    sliderStepper.motorPosition = 0;
    sliderStepper.setEnabled(true);
    sliderStepper.setTargetPosition(-1000000000);

    sliderController.setState(CALIBRATING);
}

void SliderController::stopCalibration() {
    if (!this->state == CALIBRATING) return;

    this->setSliderLength(abs(sliderStepper.motorPosition));

    sliderStepper.motorPosition = 0;
    sliderStepper.setEnabled(false);

    this->setState(IDLE);

    this->updateSliderInfo();

    Serial.println("Slider calibrated - Steps length: " + String(this->getSliderLength()));
}

void SliderController::updateSliderInfo() {
    String sliderInfo = "L:" + String(this->getSliderLength()) + ";S:" + String(this->getState()) + ";P:" + String(sliderStepper.motorPosition);
    sliderInfoCharacteristic->setValue(sliderInfo.c_str());

    sliderInfoCharacteristic->notify();
};

void SliderController::setSliderLength(int length) {
    this->sliderLength = length;
}

int SliderController::getSliderLength() {
    return this->sliderLength;
}

State SliderController::getState() {
    return this->state;
}

void SliderController::setState(State state) {
    this->state = state;
}