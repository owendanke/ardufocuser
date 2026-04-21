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
#include "stateStruct.h"

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

//singleton& focuserData = singleton::getInstance();
stateStruct* focuserData = NULL;

void setup() {
  focuserData = (stateStruct*) malloc(sizeof(stateStruct));

  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  // start with driver hardware disabled
  digitalWrite(EN_PIN, HIGH);

  // Enable Serial for usb and Serial1
  Serial.begin(9600);             // usb serial
  SERIAL_PORT.begin(115200);      // HW UART drivers  

  // // Flush any garbage in the buffer before handshake
  // delay(100);  // give USB stack time to settle
  // while (Serial.available()) Serial.read();

  /* -- handshake loop -- */
  // blocking loop
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') {
        // if there is 
        if (inputBuffer.length() > 0) {
          inputBuffer.trim();

          focuserData->cmd = parseCommand(inputBuffer);
          inputBuffer = "";

          // check if command is handshake
          if (focuserData->cmd.machine.code == M1) {
            handleCommand(focuserData, driver, stepper);
            break;
          }
          else {
            Serial.println("err: invalid handshake, send M1");  
          }
        }
      } else {
        inputBuffer += c;
      }
    }
  }
  


}

void loop() {
  // Non-blocking serial read
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        inputBuffer.trim();

        focuserData->cmd = parseCommand(inputBuffer);
        inputBuffer = "";

        // always accept machine commands
        if (focuserData->cmd.word == WORD_M) {
          handleCommand(focuserData, driver, stepper);
        }

        // only accept motion if focuser is connected
        else if (focuserData->isConnected && focuserData->cmd.word == WORD_G) {
          handleCommand(focuserData, driver, stepper);
        }
        else {
          Serial.println("err: focuser not connected, send M1");  
        }
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