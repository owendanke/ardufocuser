/**
 * stateStruct.h
 * (c) Owen Danke 2026
 * 
 * struct for holding information about the focuser's state.
 */

#include "commands.h"

#ifndef STATESTRUCT_H
#define STATESTRUCT_H

typedef struct {
    bool isConnected = false;
    Command cmd;
    uint32_t maxSpeed = 3200;
    uint32_t accel = 6400;
} stateStruct;

#endif // STATESTRUCT_H