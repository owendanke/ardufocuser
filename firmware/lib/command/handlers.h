/**
 * handlers.h
 * (c) Owen Danke 2026
 */

#ifndef HANDLERS_H
#define HANDLERS_H

#include <TMCStepper.h>
#include <AccelStepper.h>
#include "commands.h"

/**
 * Handlers take a reference to the stepper so they can control it
 * without it being a global. Declare your stepper in main and pass it in.
 */

void handleMachineCmd(const MachineCmd&, TMC2209Stepper&, AccelStepper&);
void handleMotionCmd(const MotionCmd&, TMC2209Stepper&, AccelStepper&);
void handleCommand(const Command&, TMC2209Stepper&, AccelStepper&);

#endif // HANDLERS_H