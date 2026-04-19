/**
 * parseCommand.cpp
 * (c) Owen Danke 2026
 */

#include "commands.h"
#include "parseCommand.h"

Command parseCommand(const String& raw) {
  Command cmd;

  cmd.word = WORD_UNKNOWN;
  cmd.machine.code = M_UNKNOWN;
  cmd.motion.code = G_UNKNOWN;

  if (raw.length() == 0) return cmd;

  // Read the command type
  // will either be 'M' or 'G' 
  char letter = toupper(raw.charAt(0));

  // find command code
  int code = raw.substring(1).toInt();

  // find position parameter (if motion code)
  int pIdx = raw.indexOf('P');

  // parse command
  if (letter == 'M') {
    cmd.word = WORD_M;
    if      (code == 1)  cmd.machine.code = M1;
    else if (code == 0)  cmd.machine.code = M0;
    else if (code == 4)  cmd.machine.code = M4;
    else if (code == 5)  cmd.machine.code = M5;
    else if (code == 99) cmd.machine.code = M99;
    else                 cmd.machine.code = M_UNKNOWN;

  } else if (letter == 'G') {
    cmd.word = WORD_G;
    if      (code == 0)  cmd.motion.code = G0;
    else                 cmd.motion.code = G_UNKNOWN;
  }

  /* -- Parse optional parameters -- */

  // Parse position parameter
  if (pIdx == -1) {
    // double check if accidently sent lowercase
    pIdx = raw.indexOf('p');
  }
  if (pIdx != -1) {
    cmd.motion.P = raw.substring(pIdx + 1).toInt();
  }

  return cmd;
}