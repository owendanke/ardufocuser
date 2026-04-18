/**
 * ArduFocuser.cpp
 * 
 * Copyright (C) 2026 Owen Danke
 * 
 * ArduFocuser driver implementation
 */

#include "ardufocuser_config.h"
#include "ardufocuser.h"

// We declare an auto pointer to MyCustomDriver.
static std::unique_ptr<ArduFocuser> arduFocuser(new ArduFocuser());

/**
 * Constructor for the ArduFocuser class.
 */
ArduFocuser::ArduFocuser()
{
    setVersion(ARDUFOCUSER_VERSION_MAJOR, ARDUFOCUSER_VERSION_MINOR);
}

/**
 * Default deconstructor for the ArduFocuser class.
 */
ArduFocuser::~ArduFocuser() = default;

/**
 * Properties to adjust the behavior and performance of the physical hardware.
 */
bool ArduFocuser::initProperties() {

    // initialize the parent's properties first
    INDI::Focuser::initProperties();

    // Important to update driver interface to indicate that it supports GUIDER interface
    setDriverInterface(AUX_INTERFACE | GUIDER_INTERFACE);

    return true;
}

/**
 * Called when a client requests to connect to the device.
 * Establish a connection with physical hardware.
 */
bool ArduFocuser::connect() {

    LOG_INFO("ArduFocuser has connected successfully!");

    return true;
}

/**
 * Called when a client requests to disconnect from the device.
 * Close the connection with the physical hardware.
 */
bool ArduFocuser::disconnect() {

    LOG_INFO("ArduFocuser has disconnected successfully!");

    return true;
}

/**
 * Return the name of the device.
 * In this case getDefaultName() returns "ArduFocuser".
 */
const char *ArduFocuser::getDefaultName() {

    return "ArduFocuser";
}