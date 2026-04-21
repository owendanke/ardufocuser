/**
 * control.h
 * (c) Owen Danke 2026
 */

#include <TMCStepper.h>
#include <AccelStepper.h>

#include "pinDefinitions.h"
#include "stateStruct.h"


void handshake(stateStruct*, TMC2209Stepper&, AccelStepper&);

void disconnect(stateStruct*, TMC2209Stepper&, AccelStepper&);

void reportDriverStatus(TMC2209Stepper&);

void queryPosition(AccelStepper&);

void queryDistanceToGo(AccelStepper&);

void setMicrosteps(stateStruct*, TMC2209Stepper&);

void setSpeed(stateStruct*, AccelStepper&);

void setAccel(stateStruct*, AccelStepper&);