/**
 * ardufocuser.h
 * 
 * Copyright (C) 2026 Owen Danke
 * 
 * ArduFocuser driver header
 */

#pragma once

#include <indifocuser.h>
#include <connectionplugins/connectionserial.h>

class ArduFocuser : public INDI::Focuser
{ 
    public:
        /**
         * Constructor for the ArduFocuser class.
         */
        ArduFocuser();

        /**
         * Default deconstructor for the ArduFocuser class.
         */
        virtual ~ArduFocuser();

        /**
         * Properties to adjust the behavior and performance of the physical hardware.
         * @return True if successful, false otherwise.
         */
        bool initProperties();

        /**
         * Define or Delete properties based on the connection status of the focuser
         * @return True if successful, false otherwise.
         */
        bool updateProperties();        

        /**
         * Called when a client requests to connect to the device.
         * Establish a connection with physical hardware.
         * @return True if successful, false otherwise.
         */
        bool Connect();

        /**
         * Called when a client requests to disconnect from the device.
         * Close the connection with the physical hardware.
         * @return True if successful, false otherwise.
         */
        bool Disconnect();

        /**
         * Return the name of the device.
         * In this case getDefaultName() returns "ArduFocuser".
         */
        const char *getDefaultName();

    private:
        /**
         * Override INDI::Focuser::Handshake()
         * Send a known command and expect a response
         * @return True if successful, false otherwise.
         */
        bool Handshake() override;

        /**
         * Send a command and optionally read a response
         * @return True if successful, false otherwise.
         */
        bool sendCommand(
            const char *command,
            char *response,
            int responseLength
        );
};