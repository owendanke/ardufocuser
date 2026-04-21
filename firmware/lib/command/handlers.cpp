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
      queryPosition(stepper);
      break;
    case M6:
      queryDistanceToGo(stepper);
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
    case M99:
      stepper.stop();
      Serial.println("ok: stopped");
      break;
    default:
      Serial.println("err: unknown M code");
      break;
  }
}

void handleMotionCmd(const MotionCmd& cmd, AccelStepper& stepper) {
  switch (cmd.code) {
    case G0:
      stepper.move(cmd.P);
      Serial.print("ok: moving ");
      Serial.println(cmd.P);
      break;
    default:
      Serial.println("err: unknown G code");
      break;
  }
}

void handleCommand(stateStruct* data, TMC2209Stepper& driver, AccelStepper& stepper) {
  switch (data->cmd.word) {
    case WORD_M: handleMachineCmd(data, driver, stepper); break;
    case WORD_G: handleMotionCmd(data->cmd.motion, stepper);   break;
    default:     Serial.println("err: unknown command");  break;
  }
}