#pragma once

#include <Arduino.h>
#include <Motor.h>

#define SLIDER_MOTOR_enabledPin 25
#define SLIDER_MOTOR_dirPin 26
#define SLIDER_MOTOR_stepPin 27

extern StepperMotor sliderStepper;

enum State {
    IDLE,
    CALIBRATING,
    RUNNING
};

class SliderController {
   private:
    State state = IDLE;
    int sliderLength = -1;

   public:
    SliderController();

    void init();

    void run();

    void startCalibration();
    void stopCalibration();

    void updateSliderInfo();

    void setSliderLength(int length);
    int getSliderLength();

    State getState();
    void setState(State state);
};

extern SliderController sliderController;