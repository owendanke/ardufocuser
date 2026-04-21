/**
 * microsteps.h
 * (c) Owen Danke 2026
 * 
 * Enumerated type for checking microstep setting
 */

#ifndef MICROSTEPS_H
#define MICROSTEPS_H

typedef enum Microsteps {
    EIGHTH =            1 << 3,     // 8
    SIXTEENTH =         1 << 4,     // 16
    THIRTYSECONDTH =    1 << 5,     // 32
    SIXTYFOURTH =       1 << 6      // 64
} Microsteps;

#endif // MICROSTEPS_H