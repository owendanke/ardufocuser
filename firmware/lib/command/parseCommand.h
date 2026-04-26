/**
 * parseCommand.cpp
 * (c) Owen Danke 2026
 */

#include <Arduino.h>

// Parameter requirement flags
#define PARAM_NONE 0x00
#define PARAM_P    0x01
#define PARAM_I    0x02

struct MotionCommandDef {
  MotionCommands code;
  uint8_t required;
};

struct MachineCommandDef {
  MachineCommands code;
  uint8_t required;
};

Command parseCommand(const char*);

MachineCommands parseMachineCode(int);

MotionCommands parseMotionCode(int);

bool validateCommand(Command &);