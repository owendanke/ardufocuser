/**
 * control.cpp
 * (c) Owen Danke 2026
 */

#include "control.h"

#include "microsteps.h"

void handshake(stateStruct* data, TMC2209Stepper& driver, AccelStepper& stepper) {

    const uint8_t MAX_RETRIES = 5;
    uint8_t attempt = 0;
    uint8_t result  = 0;

    /**
     * How many steps needed to make one revolution
     * 1.8 degrees/step * 1 revlotion/360 degrees = 200 steps/revolution
     * This must be multiplied by microsteps before being used
     */
    uint32_t STEPS_PER_REV = 200;

    // enable driver
    digitalWrite(EN_PIN, LOW);

    // init driver
    driver.begin();                 // SPI: Init CS pins and possible SW SPI pins
    driver.I_scale_analog(0);       // Set current source to UART over vref potentiometer
    driver.toff(5);                 // Enables driver in software
    driver.rms_current(750, 0.5);   // Set motor RMS current and holding current percentage
    driver.microsteps(16);          // Set microsteps to 1/16th
    driver.en_spreadCycle(false);   // Toggle spreadCycle on TMC2208/2209/2224
    driver.pwm_autoscale(true);     // Needed for stealthChop
    //driver.SGTHRS(0);               // set stall threshold to 0 — disables stall protection

    do {
        result = driver.test_connection();
        if (result != 0) {
            delay(500);
        }
    } while (result != 0 && ++attempt < MAX_RETRIES);

    // let host know
    // 0 = OK, 1/2 = fault
    if (result != 0) {
        Serial.println(F("err: TMC2209 driver failed to connect"));
        return;
    }

    // adjust how many steps needed for a single revolution based on configured microsteps
    uint16_t ms = driver.microsteps();
    if (ms == 0) ms = 256;  // 0 means 256-step interpolation
    STEPS_PER_REV = 200 * ms;

    // configure speed and acceleration
    stepper.setMaxSpeed(data->maxSpeed);    // 3200
    stepper.setAcceleration(data->accel);   // 6400

    // update connected status
    data->isConnected = true;
    Serial.println(F("ok: ArduFocuser_1.0"));
}

void disconnect(stateStruct* data, TMC2209Stepper& driver, AccelStepper& stepper) {
    stepper.stop();

    // disable driver
    digitalWrite(EN_PIN, HIGH); // hardware
    driver.toff(0); //software

    data->isConnected = false;

    Serial.println(F("ok: disconnected"));
}

void reportDriverStatus(TMC2209Stepper& driver) {
    Serial.println(F("ok: TMC2209 info - "));
    // Try reading version — should return 0x21 for TMC2209
    Serial.print(F("Driver version: 0x"));
    Serial.println((driver.IOIN() >> 24) & 0xFF, HEX);  // expect 0x21

    // print microsteps
    Serial.print(F("Microsteps: "));
    Serial.println(driver.microsteps());

    // print current value
    Serial.print(F("RMS current (mA): "));
    Serial.println(driver.rms_current());  // Read back actual value

    // print current setting
    Serial.print(F("CS actual (current scale): "));
    Serial.println(driver.cs_actual());    // 0-31, higher = more current

    Serial.print(F("Stall Guard: "));
    Serial.println(driver.SGTHRS());
}

void queryDistanceToGo(AccelStepper& stepper) {
    long distanceToGo = stepper.distanceToGo();

    if (stepper.isRunning()) {
        Serial.print(F("ok: moving = "));
        Serial.println(distanceToGo);
    }
    else {
        Serial.print(F("ok: done. position = "));
        Serial.println(stepper.currentPosition());
    }
}


void setMicrosteps(stateStruct* data, TMC2209Stepper& driver) {

    const Microsteps Levels[] = { EIGHTH, SIXTEENTH, THIRTYSECONDTH, SIXTYFOURTH };
    const int numLevels = sizeof(Levels) / sizeof(Levels[0]);

    bool valid = false; // Flag to check if the value is valid

    for (int i = 0; i < numLevels; i++) {
        Microsteps current = Levels[i];

        if (data->cmd.machine.I == current) {
            driver.microsteps(data->cmd.machine.I);
            Serial.print(F("ok: microsteps = "));
            Serial.println(data->cmd.machine.I);
            valid = true;
            break;
        }
    }

    if (!valid) {
        // If no valid match was found, print an error message
        Serial.print(F("err: Invalid microsteps value "));
        Serial.println(data->cmd.machine.I);
    }
}

void setSpeed(stateStruct* data, AccelStepper& stepper) {
    if (data->cmd.machine.I < 1) {
        Serial.print(F("err: Invalid speed value "));
        Serial.println(data->cmd.machine.I);
    }
    else {
        stepper.setMaxSpeed(data->cmd.machine.I);
        data->maxSpeed = data->cmd.machine.I;
        Serial.print(F("ok: max speed = "));
        Serial.println(data->cmd.machine.I);
    }
}

void setAccel(stateStruct* data, AccelStepper& stepper) {
    if (data->cmd.machine.I < 1) {
        Serial.print(F("err: Invalid accel value "));
        Serial.println(data->cmd.machine.I);
    }
    else {
        stepper.setAcceleration(data->cmd.machine.I);
        data->accel = data->cmd.machine.I;
        Serial.print(F("ok: accel = "));
        Serial.println(data->cmd.machine.I);
    }
}

void setZeroPosition(stateStruct* data, AccelStepper& stepper) {
    if (stepper.distanceToGo() != 0) {
        Serial.println(F("err: cannot reset position while moving"));
        return;
    }
    stepper.setCurrentPosition(0);
    stepper.setMaxSpeed(data->maxSpeed);
    stepper.setAcceleration(data->accel);
    Serial.println(F("ok: reset zero position"));
}

void setCurrentPosition(stateStruct* data, AccelStepper& stepper) {
    if (stepper.distanceToGo() != 0) {
        Serial.println(F("err: cannot set position while moving"));
        return;
    }
    stepper.setCurrentPosition(data->cmd.machine.I);
    stepper.setMaxSpeed(data->maxSpeed);
    stepper.setAcceleration(data->accel);
    Serial.print(F("ok: set position "));
    Serial.println(data->cmd.machine.I);
}

