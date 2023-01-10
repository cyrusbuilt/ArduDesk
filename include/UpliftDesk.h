#ifndef _UPLIFT_DESK_H
#define _UPLIFT_DESK_H

#include <Arduino.h>
#include "SoftwareSerial.h"

#define DESK_BAUD_RATE 9600

/**
 * @brief Possible desk states.
 */
enum class DeskState : uint8_t {
	/**
	 * @brief Desk movement has stopped or desk is idle.
	 */
	STOPPED = 0,

	/**
	 * @brief The desk is raising it's height.
	 */
	MOVING_UP = 1,

	/**
	 * @brief The desk is lowering it's height.
	 */
	MOVING_DOWN = 2
};

/**
 * @brief Internal controller state used for packet decoding.
 */
enum class ControllerState : uint8_t {
	/**
	 * @brief Beginning of height sync packet.
	 */
	SYNC1,

	/**
	 * @brief Got a height command response.
	 */
	SYNC2,

	/**
	 * @brief Got the first byte of the height value.
	 */
	HEIGHT1,

	/**
	 * @brief Got the second byte of the height value.
	 */
	HEIGHT2
};

/**
 * @brief Represents a desk packet (specifically, a current height report).
 */
struct DeskPacket {
	/**
	 * @brief The current state (packet byte processing).
	 */
	ControllerState state = ControllerState::SYNC1;
	
	/**
	 * @brief Data buffer to hold height values.
	 */
	uint8_t buffer[2];

	/**
	 * @brief Stores desk packet data in the buffer and sets the internal state.
	 *
	 * @param b The byte value to store. 
	 * @return true If the packet is complete.
	 * @return false If the packet is incomplete and more data is expected.
	 */
	bool put(uint8_t b) {
		bool packetComplete = false;
		switch (state) {
			case ControllerState::SYNC1:
				state = (b == 0x01) ? ControllerState::SYNC2 : ControllerState::SYNC1;
				break;
			case ControllerState::SYNC2:
				state = (b == 0x01) ? ControllerState::HEIGHT1 : ControllerState::SYNC1;
				break;
			case ControllerState::HEIGHT1:
				// TODO Should probably have command byte definitions somewhere.
				if (b == 0x00 || b == 0x01) {
					buffer[0] = b;
					state = ControllerState::HEIGHT2;
				}
				else {
					state = ControllerState::SYNC1;
				}
				break;
			case ControllerState::HEIGHT2:
				buffer[1] = b;
				state = ControllerState::SYNC1;
				packetComplete = true;
				break;
			default:
				break;
		}

		return packetComplete;
	}

	/**
	 * @brief Decodes the data buffer into an integer value.
	 * 
	 * @return uint16_t The decoded value.
	 */
	uint16_t decode() {
		return 256 * buffer[0] + buffer[1] - 255;
	}
};

/**
 * @brief Driver class for UpliftDesk V2 standing desks.
 */
class UpliftDeskClass {
public:
	/**
	 * @brief Construct a new Uplift Desk Class object.
	 */
	UpliftDeskClass();

	/**
	 * @brief Initializes the driver.
	 * 
	 * @param htx Handset serial TX pin.
	 * @param dtx Desk serial TX pin.
	 * @param hs0 Handset control line 0.
	 * @param hs1 Handset control line 1.
	 * @param hs2 Handset control line 2.
	 * @param hs3 Handset control line 3.
	 */
	void begin(short htx, short dtx, short hs0, short hs1, short hs2, short hs3);

	/**
	 * @brief Processing loop. Reads current state and processes height changes.
	 */
	void loop();

	/**
	 * @brief Request height change. Sets the target height to be processed on
	 * on the next loop() call.
	 * 
	 * @param targetHeight The target height (0 - 256).
	 */
	void goToHeight(int16_t targetHeight);

	/**
	 * @brief Gets the current height.
	 * 
	 * @return int16_t The current height value.
	 */
	int16_t getHeight() { return _height; }

	/**
	 * @brief Gets the current state as string.
	 * 
	 * @return String The string representation of the current state.
	 */
	String getStateString();

	/**
	 * @brief Gets the current state.
	 * 
	 * @return DeskState The current state.
	 */
	DeskState getState();

	/**
	 * @brief Stops movement immediately.
	 */
	void stop();

private:
	void readDeskSerial();
	void moveDown();
	void moveUp();
	void unlatchAll();

	short _htx;
	short _dtx;
	short _hs0;
	short _hs1;
	short _hs2;
	short _hs3;
	int16_t _height;
	int16_t _targetHeight;
	bool _heightChanged;
	DeskState _state;
	SoftwareSerial *_deskSerial;
	SoftwareSerial *_hsSerial;
	DeskPacket *_deskPacket;
};

/**
 * @brief Global UpliftDesk instance.
 */
extern UpliftDeskClass UpliftDesk;
#endif