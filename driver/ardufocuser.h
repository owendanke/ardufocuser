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
        
        ArduFocuser();
        virtual ~ArduFocuser();

        bool initProperties();
        bool updateProperties();        

        bool Connect();
        bool Disconnect();

        const char *getDefaultName();

    private:
        // std::unique_ptr<FocuserSerial> m_serial;

        bool Handshake() override;

        bool sendCommand(
            const char *command,
            char *response,
            int responseLength
        );
};