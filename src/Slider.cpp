#include <Bluetooth.h>
#include <Slider.h>

SliderController sliderController;
StepperMotor sliderStepper(SLIDER_MOTOR_dirPin, SLIDER_MOTOR_stepPin, SLIDER_MOTOR_enabledPin);

SliderController::SliderController() {
}

int getTimelineStart(Timeline timeline) {
    return timeline.keyframes.at(0).position;
}

int getKeyframeCount(Timeline timeline) {
    return timeline.keyframes.size();
}

int getTimelineDuration(Timeline timeline) {
    int time = 0;

    for (int i = 0; i < getKeyframeCount(timeline); i++) {
        time += timeline.keyframes.at(i).duration + timeline.keyframes.at(i).stopTime;
    }

    return time;
}

void SliderController::init() {
    this->sliderLength = preferences.getInt("sliderLength", -1);
    // this->state = (SliderState)preferences.getInt("sliderState", SETUP);

    sliderStepper.motorPosition = preferences.getInt("sliderPosition", 0);

    Serial.println("Initialized slider controller");
    Serial.println("Slider length: " + String(this->sliderLength));

    if (this->sliderLength != -1) {
        this->setState(IDLE);
    }

    /*     if (this->sliderLength == -1) {
            this->startCalibration();
        } else {
            this->setState(IDLE);

            Timeline timeline;

            Keyframe keyframe1;
            keyframe1.duration = 2000;
            keyframe1.position = 10000;
            keyframe1.stopTime = 0;

            timeline.keyframes.push_back(keyframe1);

            Keyframe keyframe2;
            keyframe2.duration = 10000;
            keyframe2.position = 100000;
            keyframe2.stopTime = 2000;

            timeline.keyframes.push_back(keyframe2);

            Keyframe keyframe3;
            keyframe3.duration = 5000;
            keyframe3.position = 50000;
            keyframe3.stopTime = 10000;

            timeline.keyframes.push_back(keyframe3);

            Keyframe keyframe4;
            keyframe4.duration = 300000;
            keyframe4.position = 80000;
            keyframe4.stopTime = 0;

            timeline.keyframes.push_back(keyframe4);

            timeline.state = STARTING;
            timeline.currentKeyframe = 0,

            this->runTimeline(timeline);
        }; */
}

long sliderStopTime = 0;
void SliderController::runNextKeyframe() {
    this->currentTimeline.currentKeyframe++;
    Keyframe nextKeyframe = this->currentTimeline.keyframes.at(this->currentTimeline.currentKeyframe);
    Serial.println("Next keyframe: " + String(nextKeyframe.position) + " in " + String(nextKeyframe.duration) + "ms");
    sliderStepper.moveTo(nextKeyframe.position, nextKeyframe.duration);
}

long lastSliderInfoUpdate = 0;
void SliderController::run() {
    long mics = micros();

    if (this->state == ANIMATING) {
        if (sliderStopTime != 0 && sliderStopTime > mics) {
            return;
        }

        Keyframe currentKeyframe = this->currentTimeline.keyframes.at(this->currentTimeline.currentKeyframe);
        if (this->currentTimeline.state == STARTING) {
            if (sliderStepper.motorPosition == getTimelineStart(this->currentTimeline)) {
                Serial.println("Motor reached start position, starting timeline");

                this->currentTimeline.state = RUNNING;
                this->runNextKeyframe();
            }
        } else if (this->currentTimeline.state == RUNNING && sliderStepper.motorPosition == currentKeyframe.position) {
            if (this->currentTimeline.currentKeyframe + 1 >= getKeyframeCount(this->currentTimeline)) {
                Serial.println("Reached end of timeline, stopping motor");
                this->currentTimeline.state = ENDED;

                this->setState(IDLE);
                sliderStepper.setEnabled(false);
            } else {
                if (currentKeyframe.stopTime > 0 && this->currentTimeline.state == RUNNING) {
                    Serial.println("Keyframe has stoptime, stopping motor for " + String(currentKeyframe.stopTime) + "ms");
                    sliderStopTime = micros() + (currentKeyframe.stopTime * 1000);
                    this->currentTimeline.state = STOP_TIME;
                    return;
                }

                Serial.println("Motor reached keyframe position, starting next keyframe");
                this->runNextKeyframe();
            }
        } else if (this->currentTimeline.state == STOP_TIME && sliderStepper.motorPosition == currentKeyframe.position) {
            Serial.println("Stoptime ended, moving to next keyframe");
            this->currentTimeline.state = RUNNING;
            this->runNextKeyframe();
        }
    }

    if (this->state != IDLE && this->state != SETUP) {
        if (this->sliderLength != -1 && ((sliderStepper.motorPosition >= this->sliderLength && sliderStepper.getDirection() == FORWARD) || (sliderStepper.motorPosition <= 0 && sliderStepper.getDirection() == BACKWARD))) {
            // Dont move the motor if it has reached the end of the slider
            Serial.println("Stopping motor, reached end of slider");
        } else {
            sliderStepper.run();
        }

        if (mics - lastSliderInfoUpdate >= 1000000) {
            lastSliderInfoUpdate = mics;

            this->updateSliderInfo();
        }
    }
}

void SliderController::startCalibration() {
    if (this->state != IDLE && this->state != SETUP) {
        Serial.println("Cannot start calibration while not idle or in setup mode");
        return;
    }

    Serial.println("Starting calibration...");
    sliderStepper.motorPosition = 0;
    sliderStepper.setEnabled(true);
    sliderStepper.moveTo(-1000000000);

    this->setSliderLength(-1);
    this->setState(CALIBRATING);

    this->updateSliderInfo();
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

void SliderController::runTimeline(Timeline timeline) {
    if (this->state != IDLE) {
        Serial.println("Cannot run timeline while not idle");
        return;
    }

    Serial.println("Running timeline with " + String(getKeyframeCount(timeline)) + " keyframes, taking about " + String(getTimelineDuration(timeline)) + "ms");

    this->currentTimeline = timeline;
    this->setState(ANIMATING);
    sliderStepper.setEnabled(true);
    sliderStepper.moveTo(getTimelineStart(timeline));
}

void SliderController::setSliderLength(int length) {
    this->sliderLength = length;
    this->updateSliderInfo();
    preferences.putInt("sliderLength", length);
}

int SliderController::getSliderLength() {
    return this->sliderLength;
}

SliderState SliderController::getState() {
    return this->state;
}

void SliderController::setState(SliderState state) {
    this->state = state;
    this->updateSliderInfo();
    preferences.putInt("sliderState", state);
}