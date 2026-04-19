/**
 * control.h
 * (c) Owen Danke 2026
 */

#include "pinDefinitions.h"

void handshake(TMC2209Stepper& driver) {

    const uint8_t MAX_RETRIES = 5;
    uint8_t attempt = 0;
    uint8_t result  = 0;

    // enable driver
    digitalWrite(EN_PIN, LOW);  // hardware

    driver.begin();                 //  SPI: Init CS pins and possible SW SPI pins
    driver.I_scale_analog(0);       // Set current source to UART over vref potentiometer
    driver.toff(5);                 // Enables driver in software
    driver.rms_current(750, 0.5);   // Set motor RMS current and holding current percentage
    driver.microsteps(16);          // Set microsteps to 1/16th

    driver.en_spreadCycle(false);   // Toggle spreadCycle on TMC2208/2209/2224
    driver.pwm_autoscale(true);     // Needed for stealthChop

    while (result != 0 && ++attempt < MAX_RETRIES) {
        result = driver.test_connection();
        if (result != 0) {
            delay(500);
        }
    }

    // let host know
    // 0 = OK, 1/2 = fault
    if (result != 0) {
        Serial.println("err: TMC2209 driver failed to connect");
    }
    else {
        Serial.println("ok: ArduFocuser_1.0");
    }
}

void disconnect(TMC2209Stepper& driver, AccelStepper& stepper) {
    stepper.stop();

    // disable driver
    digitalWrite(EN_PIN, HIGH); // hardware
    driver.toff(0); //software

    Serial.println("ok: disconnected");
}

void reportDriverStatus(TMC2209Stepper& driver) {
    Serial.println("ok: TMC2209 info - ");
    // Try reading version — should return 0x21 for TMC2209
    Serial.print("Driver version: 0x");
    Serial.println((driver.IOIN() >> 24) & 0xFF, HEX);  // expect 0x21

    // print microsteps
    Serial.print("Microsteps: ");
    Serial.println(driver.microsteps());

    // print current value
    Serial.print("RMS current (mA): ");
    Serial.println(driver.rms_current());  // Read back actual value

    // print current setting
    Serial.print("CS actual (current scale): ");
    Serial.println(driver.cs_actual());    // 0-31, higher = more current
}

void queryPosition(AccelStepper& stepper) {
    Serial.print("ok: position = ");
    Serial.println(stepper.currentPosition());
}