/**
 * ArduFocuser.h
 * 
 * Copyright (C) 2026 Owen Danke
 * 
 * ArduFocuser driver header
 */

#pragma once

#include <indifocuser.h>

class ArduFocuser : public INDI::Focuser
{ 
    public:
        
        ArduFocuser();
        virtual ~ArduFocuser();

        const char *getDefaultName();

        bool initProperties();
        // bool updateProperties();        

        bool disconnect();
        bool connect();

    private:
};