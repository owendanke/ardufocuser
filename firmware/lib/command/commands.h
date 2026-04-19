/**
 * commands.h
 * (c) Owen Danke 2026
 *
 * G-code style command definitions for stepper motor control.
 *
 * Command format:
 *   M-codes — machine/state commands, no parameters
 *   G-codes — motion commands, optional parameters
 *
 * Supported commands:
 *   M1       — Connect handshake, Arduino identifies itself to host
 *   M0       — Disconnect, graceful stop and notify host
 *   M99      — Unconditional stop, halt motion immediately
 *   G0 P<n>  — Relative move, P specifies number of steps (+/-)
 */
 
#ifndef COMMANDS_H
#define COMMANDS_H

/* -- Command Identifiers -- */

/**
 * Define the two different types of command words
 */
enum CommandWord {
  WORD_UNKNOWN,
  WORD_M,   // Machine command
  WORD_G,   // Motion command
};

/**
 * Machine command identifiers
 * Specifies which command is being asked (i.e. CMD_M1)
 */
enum MachineCommands {
  M_UNKNOWN,
  M1,   // Connect / handshake
  M0,   // Disconnect
  M4,   // Report driver status
  M5,   // Report current position
  M99,  // Unconditional stop
};

/**
 * Motion command identifiers
 * Specifies which command is being asked (i.e. CMD_G0)
 */
enum MotionCommands {
  G_UNKNOWN,
  G0,   // Relative move
};

struct MachineCmd {
    MachineCommands code;
};

struct MotionCmd {
    MotionCommands code;
    long P; // position parameter
};

struct Command {
    CommandWord word;
    MachineCmd machine;
    MotionCmd motion;
};

#endif // COMMANDS_H