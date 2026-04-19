/**
 * handlers.cpp
 * (c) Owen Danke 2026
 */

#include "handlers.h"

#include "control.h"

void handleMachineCmd(const MachineCmd& cmd, TMC2209Stepper& driver, AccelStepper& stepper) {
  switch (cmd.code) {
    case M1:  handshake(driver);                                      break;
    case M0:  disconnect(driver, stepper);                            break;
    case M4:  reportDriverStatus(driver);                             break;
    case M5:  queryPosition(stepper);                                 break;
    case M99: stepper.stop(); Serial.println("ok: stopped");          break;
    default:  Serial.println("err: unknown M code");                  break;
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

void handleCommand(const Command& cmd, TMC2209Stepper& driver, AccelStepper& stepper) {
  switch (cmd.word) {
    case WORD_M: handleMachineCmd(cmd.machine, driver, stepper); break;
    case WORD_G: handleMotionCmd(cmd.motion, stepper);   break;
    default:     Serial.println("err: unknown command");  break;
  }
}