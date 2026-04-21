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

ArduFocuser::ArduFocuser() {

    // set driver version
    setVersion(ARDUFOCUSER_VERSION_MAJOR, ARDUFOCUSER_VERSION_MINOR);

    // the driver will only communicate with serial
    setSupportedConnections(CONNECTION_SERIAL);

    // set focuser capabilities
    SetCapability(
        INDI::FocuserInterface::FOCUSER_CAN_REL_MOVE
        | INDI::FocuserInterface::FOCUSER_CAN_ABS_MOVE
        | INDI::FocuserInterface::FOCUSER_CAN_ABORT
        | INDI::FocuserInterface::FOCUSER_CAN_SYNC
    );


}

ArduFocuser::~ArduFocuser() = default;

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

const char *ArduFocuser::getDefaultName() {

    return "ArduFocuser";
}

void ArduFocuser::TimerHit()
{
    if (!isConnected())
        return;

    // Only poll the focuser if currently moving
    if (FocusRelPosNP.getState() == IPS_BUSY) {
        char response[64] = {0};

        // Ask focuser if move has finished
        if (!sendCommand(SerialCodes::distanceToGo, response, sizeof(response))) {
            LOG_ERROR("TimerHit: failed to get status");

            FocusRelPosNP.setState(IPS_ALERT);
            FocusRelPosNP.apply();
        }
        else if (strncmp("ok: done", response, 8) == 0) {
            LOG_INFO("Focuser move complete");

            FocusRelPosNP.setState(IPS_OK);
            FocusRelPosNP.apply();
        }
        else if (strncmp("err", response, 3) == 0) {
            LOGF_ERROR("Focuser error: %s", response);
            FocusRelPosNP.setState(IPS_ALERT);
            FocusRelPosNP.apply();
        }
        // if still moving then check again next tick
    }

    // Must call this at the end to reschedule the timer
    SetTimer(getCurrentPollingPeriod());
}

/* -- PROTECTED -- */

IPState ArduFocuser::MoveRelFocuser(FocusDirection dir, uint32_t ticks) {
    char cmdToSend[32] = {0};
    char response[64] = {0};

    // build the relative movement command
    strcpy(cmdToSend, SerialCodes::relativeMove);

    if (dir == FOCUS_INWARD) {
        strcat(cmdToSend, " P-");
    }
    else {
        strcat(cmdToSend, " P");
    }
    strcat(cmdToSend, std::to_string(ticks).c_str());
    strcat(cmdToSend, "\n");

    LOGF_INFO("Sending command: %s", cmdToSend);


    // send move message to focuser
    if (!sendCommand(cmdToSend, response, sizeof(response)))
    {
        LOG_ERROR("Relative move failed: could not send command");
        return IPS_ALERT;
    }

    // if focuser acknowledges move command, set IPS_BUSY
    if (strncmp("ok: moving", response, 10) == 0)
    {
        FocusRelPosNP.setState(IPS_BUSY);
        FocusRelPosNP.apply();
        return IPS_BUSY;
    }

    LOGF_ERROR("Unexpected response: %s", response);
    return IPS_ALERT;
}

IPState ArduFocuser::MoveAbsFocuser(uint32_t targetTicks) {
    char cmdToSend[32] = {0};
    char response[64] = {0};

    // build the relative movement command
    strcpy(cmdToSend, SerialCodes::absoluteMove);
    strcat(cmdToSend, " P");
    strcat(cmdToSend, std::to_string(targetTicks).c_str());
    strcat(cmdToSend, "\n");

    LOGF_INFO("Sending command: %s", cmdToSend);


    // send move message to focuser
    if (!sendCommand(cmdToSend, response, sizeof(response)))
    {
        LOG_ERROR("Relative move failed: could not send command");
        return IPS_ALERT;
    }

    // if focuser acknowledges move command, set IPS_BUSY
    if (strncmp("ok: moving to", response, 13) == 0)
    {
        FocusRelPosNP.setState(IPS_BUSY);
        FocusRelPosNP.apply();
        return IPS_BUSY;
    }

    LOGF_ERROR("Unexpected response: %s", response);
    return IPS_ALERT;
}

bool ArduFocuser::AbortFocuser() {
    char response[64] = {0};

    LOGF_INFO("Sending command: %s", SerialCodes::unconditionalStop);

    // send move message to focuser
    if (!sendCommand(SerialCodes::unconditionalStop, response, sizeof(response)))
    {
        LOG_ERROR("abort failed: could not send command");
        return false;
    }

    // if focuser acknowledges abort command, set true
    if (strncmp("ok: stop", response, 8) == 0)
    {
        LOG_INFO("focuser movement stopped.");
        //FocusRelPosNP.setState(IPS_OK);
        //FocusRelPosNP.apply();
        return true;
    }

    LOGF_ERROR("Unexpected response: %s", response);
    return false;
}

bool ArduFocuser::SyncFocuser(uint32_t ticks) {
    char cmdToSend[32] = {0};
    char response[64] = {0};

    // build the relative movement command
    strcpy(cmdToSend, SerialCodes::setCurrentPosition);
    strcat(cmdToSend, " I");
    strcat(cmdToSend, std::to_string(ticks).c_str());
    strcat(cmdToSend, "\n");

    LOGF_INFO("Sending command: %s", cmdToSend);


    // send move message to focuser
    if (!sendCommand(cmdToSend, response, sizeof(response)))
    {
        LOG_ERROR("Focuser sync failed: could not send command");
        return false;
    }

    // if focuser acknowledges move command, set IPS_BUSY
    if (strncmp("ok: set", response, 7) == 0)
    {
        return true;
    }

    LOGF_ERROR("Unexpected response: %s", response);
    return false;
}

/* -- PRIVATE -- */

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