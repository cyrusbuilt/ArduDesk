#ifndef _TELEMETRYHELPER_H
#define _TELEMETRYHELPER_H

#include <Arduino.h>

/**
 * @brief Possible system states.
 */
enum class SystemState: uint8_t {
    /**
     * @brief System is currently running the boot sequence.
     */
    BOOTING = 0,

    /**
     * @brief System is operating normally.
     */
    NORMAL = 1,

    /**
     * @brief System is currently performing an OTA update.
     */
    UPDATING = 2,

    /**
     * @brief System is disabled. Either intentionally by the user or
     * by the system due to some internal system condition such
     * as falling into failsafe/configuration mode.
     */
    DISABLED = 3
};

/**
 * @brief Possible control commands.
 */
enum class ControlCommand: uint8_t {
    /**
     * @brief Disable the controller. This will cause ArduDesk to essentially
     * ignore all commands except ENABLE.
     */
    DISABLE = 0,

    /**
     * @brief Enable the controller. This will allow ArduDesk to process
     * commands.
     */
    ENABLE = 1,

    /**
     * @brief Reboot the controller.
     */
    REBOOT = 2,

    /**
     * @brief Request controller status. This will force ArduDesk to publish
     * a status payload to the status topic.
     */
    REQUEST_STATUS = 3,

    /**
     * @brief Initiate movement. This assumes a height value has been passed in.
     */
    MOVE = 4,

    /**
     * @brief Stop movement.
     */
    STOP = 5,

    /**
     * @brief Move to the preferred sitting height.
     * 
     */
    SIT = 6,

    /**
     * @brief Move to the preferred standing height.
     * 
     */
    STAND = 7
};

/**
 * @brief Helper class providing static telemetry helper methods.
 */
class TelemetryHelper
{
public:
    /**
     * @brief Gets a string describing the specified MQTT client state.
     * @param state The state to get the description for.
     * @return A string describing the MQTT state.
     */
    static String getMqttStateDesc(int state);
};

#endif