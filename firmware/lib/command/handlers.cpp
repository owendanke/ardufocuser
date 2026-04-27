/**
 * handlers.cpp
 * (c) Owen Danke 2026
 */

#include "handlers.h"
#include "control.h"
#include "stateStruct.h"

void handleMachineCmd(stateStruct* data, TMC2209Stepper& driver, AccelStepper& stepper) {
  switch (data->cmd.machine.code) {
    case M1:
      handshake(data, driver, stepper);
      break;
    case M0:
      disconnect(data, driver, stepper);
      break;
    case M4:
      reportDriverStatus(driver);
      break;
    case M5:
      Serial.print(F("ok: position = "));
      Serial.println(stepper.currentPosition());
      break;
    case M6:
      queryDistanceToGo(stepper);
      break;
    case M7:
      Serial.print(F("ok: maximum speed: "));
      Serial.println(data->maxSpeed);
      break;
    case M8:
    Serial.print(F("ok: acceleration: "));
      Serial.println(data->accel);
      break;
    case M9:
      Serial.print(F("ok: maximum steps: "));
      Serial.println(data->maxPos);
      break;
    case M10:
      setMicrosteps(data, driver);
      break;
    case M11:
      setSpeed(data, stepper);
      break;
    case M12:
      setAccel(data, stepper);
      break;
    case M13:
      setZeroPosition(data, stepper);
      break;
    case M14:
      setCurrentPosition(data, stepper);
      break;
    case M15:
      if (data->cmd.machine.I < 1) {
        Serial.println(F("err: steps must be positive"));
        break;
      }
      
      data->maxPos = data->cmd.machine.I;
      Serial.print(F("ok: set maximum steps: "));
      Serial.println(data->maxPos);
      break;
    case M99:
      stepper.stop();
      Serial.println(F("ok: stopped"));
      break;
    case M_UNKNOWN:
      Serial.println(F("err: unknown M code"));
      break;
    default:
      Serial.println(F("err: unknown M code"));
      break;
  }
}

void handleMotionCmd(stateStruct* data, AccelStepper& stepper) {
  const MotionCmd& cmd = data->cmd.motion;
  switch (cmd.code) {
    case G0:
      if((stepper.currentPosition() + cmd.P) > data->maxPos) {
        Serial.println(F("err: movement exceeds max position"));
        break;
      }
      stepper.move(cmd.P);
      Serial.print(F("ok: moving "));
      Serial.println(cmd.P);
      break;
    case G1:
      if(cmd.P > data->maxPos) {
        Serial.println(F("err: movement exceeds max position"));
        break;
      }
      stepper.moveTo(cmd.P);
      Serial.print(F("ok: moving to "));
      Serial.println(cmd.P);
      break;
    case G_UNKNOWN:
      Serial.println(F("err: unknown G code"));
      break;
    default:
      Serial.println(F("err: unknown G code"));
      break;
  }
}

void handleCommand(stateStruct* data, TMC2209Stepper& driver, AccelStepper& stepper) {
  switch (data->cmd.word) {
    case WORD_M: 
      handleMachineCmd(data, driver, stepper);
      break;
    case WORD_G: 
      handleMotionCmd(data, stepper);
      break;
    case WORD_UNKNOWN:
      Serial.println(F("err: unknown command"));
      break;
    default:
      Serial.println(F("err: unknown command"));
      break;
  }
}