/**
 * control.h
 * (c) Owen Danke 2026
 */

#include <TMCStepper.h>
#include <AccelStepper.h>

#include "pinDefinitions.h"
#include "stateStruct.h"

/* -- Machine Commands -- */
void handshake(stateStruct*, TMC2209Stepper&, AccelStepper&);

void disconnect(stateStruct*, TMC2209Stepper&, AccelStepper&);

void reportDriverStatus(TMC2209Stepper&);

void queryPosition(AccelStepper&);

void queryDistanceToGo(AccelStepper&);

void setMicrosteps(stateStruct*, TMC2209Stepper&);

void setSpeed(stateStruct*, AccelStepper&);

void setAccel(stateStruct*, AccelStepper&);

void setZeroPosition(stateStruct*, AccelStepper&);

void setCurrentPosition(stateStruct*, AccelStepper&);

/* -- Motion Commands -- */

void moveRelative(long, AccelStepper&);

void moveAbsolute(long, AccelStepper&);