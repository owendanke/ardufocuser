/**
 * main.cpp
 * (c) Owen Danke 2026
 * 
 */
#include <Arduino.h>
#include <TMCStepper.h>
#include <AccelStepper.h>

#include "commands.h"
#include "handlers.h"
#include "parseCommand.h"

#include "pinDefinitions.h"

#define DRIVER_ADDRESS 0b00 // TMC2209 Driver address according to MS1 and MS2
#define SERIAL_PORT Serial1 // UART serial for TMC2209

#define R_SENSE 0.11f // Match to your driver
                      // SilentStepStick series use 0.11

// Set stepper driver type
TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS);       // Hardware Serial
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

/**
 * How many steps needed to make one revolution
 * 1.8 degrees/step * 1 revlotion/360 degrees = 200 steps/revolution
 * This must be multiplied by microsteps before being used
 */
uint32_t STEPS_PER_REV = 200;

String inputBuffer = "";

bool dir = false;
uint32_t stepsToMove;
uint32_t currentStep = 0;

void setup() {
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  // start with driver hardware disabled
  digitalWrite(EN_PIN, HIGH);

  // Enable Serial for usb and Serial1
  Serial.begin(9600);             // usb serial
  SERIAL_PORT.begin(115200);      // HW UART drivers  

  // handshake loop
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        inputBuffer.trim();

        Command cmd = parseCommand(inputBuffer);
        handleCommand(cmd, driver, stepper);

        inputBuffer = "";
      }
    } else {
      inputBuffer += c;
    }
  }

  // Check UART comms are working
  uint8_t result = driver.test_connection();
  if (result != 0) {
    Serial.print("UART connection failed, error: ");
    Serial.println(result);  // 0 = OK, 1/2 = fault
  } else {
    Serial.println("Driver connected OK");

    // adjust how many steps needed for a single revolution based on configured microsteps
    uint16_t ms = driver.microsteps();
    if (ms == 0) ms = 256;  // 0 means 256-step interpolation
    STEPS_PER_REV = 200 * ms;

    // configure speed and acceleration
    stepper.setMaxSpeed(STEPS_PER_REV * 5);           // 5 rev/sec
    stepper.setAcceleration(STEPS_PER_REV * 5);       // 2 rev/sec/sec

    Serial.println("Ready. Commands: M<steps> P<pos> S<speed> A<accel> H ?");
  }
}

void loop() {
  // Non-blocking serial read
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        inputBuffer.trim();
        Command cmd = parseCommand(inputBuffer);
        handleCommand(cmd, driver, stepper);
        inputBuffer = "";
      }
    } else {
      inputBuffer += c;
    }
  }

  // Must be called every loop — non-blocking
  if (stepper.distanceToGo() != 0) {
    stepper.run();
  }
}

/*
void loop() {
  stepper.moveTo(STEPS_PER_REV);       // 1 revolution forward
  while (stepper.distanceToGo() != 0) stepper.run();
  delay(500);

  stepper.moveTo(0);                   // back to start
  while (stepper.distanceToGo() != 0) stepper.run();
  delay(500);
}
*/