/**
 * serialCommands.h
 * 
 * Copyright (C) 2026 Owen Danke
 */

/**
 * Serial codes that are sent to the focuser, instructing it to do something.
 * M codes are adjusting settings and querying
 * G codes are for moving the motor
 * 
 * Codes with arguements are only null terminated and will require adding newline terminator.
 */
typedef struct SerialCodes {
    // M (machine) codes

    // send the handshake+connect code to the focuser
    static constexpr const char* connect = "M1\n";

    // send the disconnect code to the focuser
    static constexpr const char* disconnect = "M0\n";

    // query the focuser driver's status
    static constexpr const char* reportDriverStatus = "M4\n";

    // query the focuser's current position (in steps)
    static constexpr const char* currentPosition = "M5\n";

    // query if the focuser is done
    static constexpr const char* distanceToGo = "M6\n";

    // immedietly stop any focuser movement
    static constexpr const char* unconditionalStop = "M99\n";

    // G (geometric/motion) codes

    // move a relative number of steps. args: P<steps>. example to move 100 steps: G0 P100
    static constexpr const char* relativeMove = "G0";
} SerialCodes;