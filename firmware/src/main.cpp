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

// Match to your drive
// SilentStepStick series use 0.11
#define R_SENSE 0.11f 

#define BUFFER_LENGTH 64

// Set stepper driver type
TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS);       // Hardware Serial
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

char inputBuffer[BUFFER_LENGTH];
uint8_t bufferIndex = 0;

bool dir = false;
uint32_t stepsToMove;
uint32_t currentStep = 0;

static stateStruct focuserDataObj;
static stateStruct* const focuserData = &focuserDataObj;

void setup() {
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  // start with driver hardware disabled
  digitalWrite(EN_PIN, HIGH);

  // Enable Serial for usb and Serial1
  // SPI.begin();
  Serial.begin(9600);             // usb serial
  SERIAL_PORT.begin(115200);      // HW UART drivers  

  // Flush any garbage in the buffer before handshake
  while(!Serial);
  delay(100);  // give USB stack time to settle
}

void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    if ((c == '\r') && bufferIndex < (BUFFER_LENGTH - 1)) bufferIndex++;  // ignore carriage return
    if (c == '\n') {
      if (bufferIndex > 0) {
        inputBuffer[bufferIndex] = '\0';  // terminate string

        focuserData->cmd = parseCommand(inputBuffer);   // parse command

        if (!validateCommand(focuserData->cmd)) {
          bufferIndex = 0;
          return;  // error already printed by validateCommand
        }

        // always accept machine commands
        if (focuserData->cmd.word == WORD_M) {
          handleCommand(focuserData, driver, stepper);
        }
        
        // only accept motion if focuser is connected
        else if (focuserData->isConnected && focuserData->cmd.word == WORD_G) {
          handleCommand(focuserData, driver, stepper);
        }
        else {
          Serial.println(F("err: focuser not connected, send M1"));  
        }

        bufferIndex = 0;
      }
    } else if (bufferIndex < (BUFFER_LENGTH - 1)) {
      inputBuffer[bufferIndex++] = c;
    }
  }

  stepper.run();
}