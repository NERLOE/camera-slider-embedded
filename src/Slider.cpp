#include <Bluetooth.h>
#include <Slider.h>

SliderController sliderController;
StepperMotor sliderStepper(SLIDER_MOTOR_dirPin, SLIDER_MOTOR_stepPin, SLIDER_MOTOR_enabledPin);

SliderController::SliderController() {
}

void SliderController::init() {
    this->sliderLength = preferences.getInt("sliderLength", -1);
    // this->state = (SliderState)preferences.getInt("sliderState", SETUP);

    sliderStepper.motorPosition = preferences.getInt("sliderPosition", 0);

    Serial.println("Initialized slider controller");
    Serial.println("Slider length: " + String(this->sliderLength));

    if (this->sliderLength == -1) {
        this->startCalibration();
    } else {
        this->setState(IDLE);

        Timeline timeline;

        Keyframe keyframe1;
        keyframe1.duration = 2000;
        keyframe1.position = 80000;
        keyframe1.stopTime = 0;

        timeline.keyframes.push_back(keyframe1);

        Keyframe keyframe2;
        keyframe2.duration = 10000;
        keyframe2.position = 100000;
        keyframe2.stopTime = 0;

        timeline.keyframes.push_back(keyframe2);

        timeline.keyframeCount = 2;
        timeline.state = STARTING;
        timeline.startPos = 10000;
        timeline.endPos = 80000;
        timeline.currentKeyframe = 0,

        this->runTimeline(timeline);
    };
}

void SliderController::runNextKeyframe() {
    Keyframe nextKeyframe = this->currentTimeline.keyframes.at(this->currentTimeline.currentKeyframe);
    Serial.println("Next keyframe: " + String(nextKeyframe.position) + " in " + String(nextKeyframe.duration) + "ms");
    sliderStepper.moveTo(nextKeyframe.position, nextKeyframe.duration);
}

long lastSliderInfoUpdate = 0;
void SliderController::run() {
    if (this->state == ANIMATING) {
        if (this->currentTimeline.state == STARTING && sliderStepper.motorPosition == this->currentTimeline.startPos) {
            Serial.println("Motor reached start position, starting timeline");
            this->currentTimeline.state = RUNNING;

            this->runNextKeyframe();
        } else if (this->currentTimeline.state == RUNNING && sliderStepper.motorPosition == this->currentTimeline.keyframes.at(this->currentTimeline.currentKeyframe).position) {
            this->currentTimeline.currentKeyframe++;
            if (this->currentTimeline.currentKeyframe >= this->currentTimeline.keyframeCount) {
                Serial.println("Reached end of timeline, stopping motor");
                this->currentTimeline.state = STOPPED;

                this->setState(IDLE);
                sliderStepper.setEnabled(false);
            } else {
                Serial.println("Motor reached keyframe position, starting next keyframe");
                this->runNextKeyframe();
            }
        }
    }

    if (this->state != IDLE && this->state != SETUP) {
        if (this->sliderLength != -1 && ((sliderStepper.motorPosition >= this->sliderLength && sliderStepper.getDirection() == FORWARD) || (sliderStepper.motorPosition <= 0 && sliderStepper.getDirection() == BACKWARD))) {
            // Dont move the motor if it has reached the end of the slider
            Serial.println("Stopping motor, reached end of slider");
        } else {
            sliderStepper.run();
        }

        long mics = micros();
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

    Serial.println("Running timeline...");

    this->currentTimeline = timeline;
    this->setState(ANIMATING);
    sliderStepper.setEnabled(true);
    sliderStepper.moveTo(timeline.startPos);
}

void SliderController::setSliderLength(int length) {
    this->sliderLength = length;
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
    preferences.putInt("sliderState", state);
}