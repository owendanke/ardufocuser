/**
 * ardufocuser.cpp
 * 
 * Copyright (C) 2026 Owen Danke
 * 
 * ArduFocuser driver implementation
 */

#include "ardufocuser_config.h"
#include "ardufocuser.h"

#include "serialCommands.h"

// We declare an auto pointer to MyCustomDriver.
static std::unique_ptr<ArduFocuser> arduFocuser(new ArduFocuser());

/**
 * Constructor for the ArduFocuser class.
 */
ArduFocuser::ArduFocuser() {

    // set driver version
    setVersion(ARDUFOCUSER_VERSION_MAJOR, ARDUFOCUSER_VERSION_MINOR);

    // the driver will only communicate with serial
    setSupportedConnections(CONNECTION_SERIAL);
}

/**
 * Default deconstructor for the ArduFocuser class.
 */
ArduFocuser::~ArduFocuser() = default;

/**
 * Properties to adjust the behavior and performance of the physical hardware.
 */
bool ArduFocuser::initProperties() {

    LOG_INFO("ArduFocuser initializing properties...");

    // initialize the parent's properties first
    INDI::Focuser::initProperties();

    // create the FocuserSerial helper class
    //m_serial = std::make_unique<FocuserSerial>(this);

    // register connection with driver
    //registerConnection(m_serial->getSerialConnection());

    return true;
}

bool ArduFocuser::updateProperties() {

    // update the parent's properties first
    INDI::Focuser::updateProperties();

    // Connection
    // defineProperty()

    return true;
}

/**
 * Called when a client requests to connect to the device.
 * Establish a connection with physical hardware.
 */
bool ArduFocuser::Connect() {

    bool connection = INDI::Focuser::Connect();

    if (!connection) {
        LOG_ERROR("ArduFocuser encountered an error and could not connect!");
    }
    else {
        LOG_INFO("ArduFocuser has connected successfully!");
    }

    return connection;
}

/**
 * Called when a client requests to disconnect from the device.
 * Close the connection with the physical hardware.
 */
bool ArduFocuser::Disconnect() {

    char response[64] = {0};

    // send disconnect command to focuser
    bool disconnectCommand = sendCommand(SerialCodes::disconnect, response, sizeof(response));

    if (!disconnectCommand) {
        LOG_ERROR("disconnect failed: could not send command");

        return false;
    }
    
    // call parent class to disconnect the device
    bool connection = INDI::Focuser::Disconnect();

    if (!connection) {
        LOG_ERROR("disconnect failed");

        return false;
    }

    LOGF_INFO("disconnect successful. Device replied: %s", response);

    return true;
}

/**
 * Return the name of the device.
 * In this case getDefaultName() returns "ArduFocuser".
 */
const char *ArduFocuser::getDefaultName() {

    return "ArduFocuser";
}

/* -- PRIVATE -- */

/**
 * Override INDI::Focuser::Handshake()
 * Send a known command and expect a response
 */
bool ArduFocuser::Handshake(){

    char response[64] = {0};
    
    if (!sendCommand(SerialCodes::connect, response, sizeof(response))) {
        LOG_ERROR("Handshake failed: could not send command");

        return false;
    }

    if (std::strlen(response) == 0) {
        LOG_ERROR("Handshake failed: empty response");

        return false;
    }

    LOGF_INFO("Handshake successful. Device replied: %s", response);

    return true;
}

/**
 * Send a command and optionally read a response
 */
bool ArduFocuser::sendCommand(const char *command, char *response, int responseLength) {

    int nbytes_read = 0, nbytes_written = 0, tty_rc = 0;

    LOGF_DEBUG("Command: %s", command);

    // Write
    tty_rc = tty_write_string(PortFD, command, &nbytes_written);
    if (tty_rc != TTY_OK)
    {
        char errorMessage[MAXRBUF];
        tty_error_msg(tty_rc, errorMessage, MAXRBUF);
        LOGF_ERROR("Serial write error: %s", errorMessage);
        return false;
    }

    // Read until newline, 5 second timeout
    tty_rc = tty_read_section(PortFD, response, '\n', 5, &nbytes_read);
    if (tty_rc != TTY_OK)
    {
        char errorMessage[MAXRBUF];
        tty_error_msg(tty_rc, errorMessage, MAXRBUF);
        LOGF_ERROR("Serial read error: %s", errorMessage);
        return false;
    }

    response[nbytes_read - 1] = '\0'; // strip the \n
    LOGF_DEBUG("Response: %s", response);
    return true;
}