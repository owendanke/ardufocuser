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

/** 
 * Recursive descent parser
*/
Command parseCommand(const String& raw) {
  Command cmd;
  cmd.word          = WORD_UNKNOWN;
  cmd.machine.code  = M_UNKNOWN;
  cmd.motion.code   = G_UNKNOWN;
  cmd.motion.P      = 0;

  if (raw.length() == 0) return cmd;

  // position (cursor/pointer) of symbol to read
  unsigned int pos = 0;

  // consume word letter (M or G) and increment the position counter
  char letter = toupper(raw.charAt(pos++));

  // set starting position of code digits
  int codeStart = pos;

  // consume code digits
  while (pos < raw.length() && isDigit(raw.charAt(pos))) pos++;

  // save the code that was read
  int code = raw.substring(codeStart, pos).toInt();

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
  while (pos < raw.length()) {
    // skip whitespace
    while (pos < raw.length() && raw.charAt(pos) == ' ') pos++;
    // reached 'eof'
    if (pos >= raw.length()) break;

    // consume parameter letter
    char paramLetter = toupper(raw.charAt(pos++));

    // consume signed number
    bool negative = false;
    if (pos < raw.length() && raw.charAt(pos) == '-') {
      negative = true;
      pos++;
    }
    
    // set number starting position
    int numStart = pos;

    // consume value digits
    while (pos < raw.length() && isDigit(raw.charAt(pos))) pos++;

    // save the value that was read
    long value = raw.substring(numStart, pos).toInt();

    // if negative then negate value
    if (negative) value = -value;

    // assign value to the parameter
    switch (paramLetter) {
      case 'I': cmd.machine.I = value; break;
      case 'P': cmd.motion.P = value; break;
      default: break;  // unknown param — ignore
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
    case 10: return M10;
    case 11: return M11;
    case 12: return M12;
    case 99: return M99;
    default: return M_UNKNOWN;
  }
}

MotionCommands parseMotionCode(int code) {
  switch (code) {
    case 0: return G0;
    default: return G_UNKNOWN;
  }
}

int hasIntParam(const String& raw) {
  int iIdx = raw.indexOf('I');

  if (iIdx == -1) {
    // double check if accidently sent lowercase
    iIdx = raw.indexOf('i');
  }

  return iIdx;
}

int hasPosParam(const String& raw) {
  int pIdx = raw.indexOf('P');

  if (pIdx == -1) {
    // double check if accidently sent lowercase
    pIdx = raw.indexOf('p');
  }

  return pIdx;
}