/**
 * parseCommand.cpp
 * (c) Owen Danke 2026
 * 
 * Grammar of focuser commands:
 * 
 *  command -> m_cmd | g_cmd
 *  m_cmd   -> 'M' number | 'M' number params
 *  g_cmd   -> 'G' number params
 *  params  -> param params | epsilon
 *  param   -> 'P' number | 'I' number
 *  number  -> pos | neg
 *  pos     -> digits
 *  neg     -> '-' digits
 *  digits  -> digit digits | digit
 *  digit   -> '0'...'9'
 */

#include "commands.h"
#include "parseCommand.h"

static const MotionCommandDef motionDefs[] = {
  { G0, PARAM_P },
  { G1, PARAM_P },
};

static const MachineCommandDef machineDefs[] = {
  { M1,  PARAM_NONE },
  { M0,  PARAM_NONE },
  { M4,  PARAM_NONE },
  { M5,  PARAM_NONE },
  { M6,  PARAM_NONE },
  { M7,  PARAM_NONE },
  { M8,  PARAM_NONE },
  { M9,  PARAM_NONE },
  { M10, PARAM_I },
  { M11, PARAM_I },
  { M12, PARAM_I },
  { M13, PARAM_NONE },
  { M14, PARAM_I },
  { M15, PARAM_I },
  { M99, PARAM_NONE },
};

/** 
 * Recursive descent parser
*/
Command parseCommand(const char* raw) {
  Command cmd;
  cmd.word         = WORD_UNKNOWN;
  cmd.machine.code = M_UNKNOWN;
  cmd.machine.I    = 0;
  cmd.machine.hasI = false;
  cmd.motion.code  = G_UNKNOWN;
  cmd.motion.P     = 0;
  cmd.motion.hasP  = false;

  uint8_t len = strlen(raw);  // calculate once

  if (len == 0) return cmd;

  // position (cursor/pointer) of symbol to read
  uint8_t pos = 0;

  // consume word letter (M or G) and increment the position counter
  char letter = toupper(raw[pos++]);

  // set starting position of code digits
  int codeStart = pos;

  // consume code digits
  while (pos < len && isDigit(raw[pos])) pos++;

  // save the code that was read
  int code = (codeStart == pos) ? -1 : atoi(raw + codeStart);

  // Parse the word letter
  if (letter == 'M') {
    cmd.word = WORD_M;
    cmd.machine.code = parseMachineCode(code);

  }
  else if (letter == 'G') {
    cmd.word = WORD_G;
    cmd.motion.code = parseMotionCode(code);
  }
  else {
    // incorrect word letter, neither M or G
    // skip the parameter parsing
    // return initially set UNKNOWN values
    return cmd;
  }

  // consume optional parameters
  while (pos < len) {
    // skip whitespace
    while (pos < len && raw[pos] == ' ') pos++;
    // reached 'eof'
    if (pos >= len) break;

    // consume parameter letter
    char paramLetter = toupper(raw[pos++]);

    // consume signed number
    bool negative = false;
    if (pos < len && raw[pos] == '-') {
      negative = true;
      pos++;
    }
    
    // set number starting position
    int numStart = pos;

    // consume value digits
    while (pos < len && isDigit(raw[pos])) pos++;

    // save the value that was read
    long value = atol(raw + numStart);
    
    // if negative then negate value
    if (negative) value = -value;

    // assign value to the parameter
    switch (paramLetter) {
      case 'I':
        cmd.machine.I = value;
        cmd.machine.hasI = true;
        break;
      case 'P':
        cmd.motion.P = value;
        cmd.motion.hasP = true;
        break;
      default: break;  // unknown param
    }

  }

  return cmd;
}

MachineCommands parseMachineCode(int code) {
  switch (code) {
    case 0:  return M0;
    case 1:  return M1;
    case 4:  return M4;
    case 5:  return M5;
    case 6:  return M6;
    case 7:  return M7;
    case 8:  return M8;
    case 9:  return M9;
    case 10: return M10;
    case 11: return M11;
    case 12: return M12;
    case 13: return M13;
    case 14: return M14;
    case 15: return M15;
    case 99: return M99;
    default: return M_UNKNOWN;
  }
}

MotionCommands parseMotionCode(int code) {
  switch (code) {
    case 0: return G0;
    case 1: return G1;
    default: return G_UNKNOWN;
  }
}

bool validateCommand(Command& cmd) {
  if (cmd.word == WORD_G) {
    for (uint8_t i = 0; i < sizeof(motionDefs)/sizeof(motionDefs[0]); i++) {
      if (motionDefs[i].code == cmd.motion.code) {
        uint8_t req = motionDefs[i].required;
        if ((req & PARAM_P) && cmd.motion.P == 0 && !cmd.motion.hasP) {
          Serial.println(F("err: missing P parameter"));
          return false;
        }
        return true;
      }
    }
  } else if (cmd.word == WORD_M) {
    for (uint8_t i = 0; i < sizeof(machineDefs)/sizeof(machineDefs[0]); i++) {
      if (machineDefs[i].code == cmd.machine.code) {
        uint8_t req = machineDefs[i].required;
        if ((req & PARAM_I) && !cmd.machine.hasI) {
          Serial.println(F("err: missing I parameter"));
          return false;
        }
        return true;
      }
    }
  }
  return true;
}