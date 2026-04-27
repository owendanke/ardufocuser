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

        /**
         * Callback function to be called once SetTimer duration elapses.
         */
        void TimerHit() override;

    protected:
        /**
         * Move the focuser to an relative position.
         * @param dir Direction of focuser, either FOCUS_INWARD or FOCUS_OUTWARD.
         * @param ticks The relative ticks to move.
         * @return Return IPS_OK if motion is completed and focuser reached requested position. Return
         * IPS_BUSY if focuser started motion to requested position and is in progress.
         * Return IPS_ALERT if there is an error.
         */
        IPState MoveRelFocuser(FocusDirection, uint32_t) override;

        /**
         * Move the focuser to an absolute position.
         * @param ticks The new position of the focuser.
         * @return Return IPS_OK if motion is completed and focuser reached requested position. Return
         * IPS_BUSY if focuser started motion to requested position and is in progress.
         * Return IPS_ALERT if there is an error.
         */
        IPState MoveAbsFocuser(uint32_t targetTicks) override;

        /**
         * Cancel all focuser motion
         * @return True if abort is successful, false otherwise.
         */
        bool AbortFocuser() override;

        /**
         * Set current position to ticks without moving the focuser.
         * @param ticks Desired new sync position.
         * @return True if successful, false otherwise.
         */
        bool SyncFocuser(uint32_t) override;

        /**
         * Set Focuser Maximum position limit in the hardware.
         * @param ticks maximum steps permitted
         * @return True if successful, false otherwise.
         * @note If setting maximum position limit in the hardware is not available or not supported, do not override this function as the default
         * implementation will always return true.
         */
        bool SetFocuserMaxPosition(uint32_t) override;

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