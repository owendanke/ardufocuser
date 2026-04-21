/**
 * parseCommand.cpp
 * (c) Owen Danke 2026
 */

#include <Arduino.h>

Command parseCommand(const String&);
MachineCommands parseMachineCode(int);
MotionCommands parseMotionCode(int);

int hasIntParam(const String&);
int hasPosParam(const String&);

void setIntParam(const String&, int);
void setPosParam(const String&, int);