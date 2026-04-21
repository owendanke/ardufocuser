/**
 * parseCommand.cpp
 * (c) Owen Danke 2026
 */

#include <Arduino.h>

Command parseCommand(const char*);
MachineCommands parseMachineCode(int);
MotionCommands parseMotionCode(int);