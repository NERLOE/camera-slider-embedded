#pragma once

#include <Arduino.h>
#include <Motor.h>

#define SLIDER_MOTOR_enabledPin 25
#define SLIDER_MOTOR_dirPin 26
#define SLIDER_MOTOR_stepPin 27

extern StepperMotor sliderStepper;

enum SliderState {
    SETUP = -1,
    IDLE,
    CALIBRATING,
    MOVING,
    ANIMATING,
};

struct Keyframe {
    int position;
    double duration;
    double stopTime;
};

enum TimelineState {
    STARTING,
    RUNNING,
    STOPPED,
};

struct Timeline {
    std::vector<Keyframe> keyframes;
    int keyframeCount = 0;
    TimelineState state = STARTING;
    int startPos;
    int endPos;
    int currentKeyframe = 0;
};

class SliderController {
   private:
    SliderState state = SETUP;
    int sliderLength = -1;

    Timeline currentTimeline;

   public:
    SliderController();

    void init();

    void run();

    void runNextKeyframe();

    void startCalibration();
    void stopCalibration();

    void updateSliderInfo();

    void runTimeline(Timeline timeline);

    void setSliderLength(int length);
    int getSliderLength();

    SliderState getState();
    void setState(SliderState state);
};

extern SliderController sliderController;