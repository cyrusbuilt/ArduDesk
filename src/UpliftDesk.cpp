#include "UpliftDesk.h"
#include "config.h"   // Just for debug flag

#define OFFSET_BELOW 2
#define OFFSET_ABOVE 4
#define MIN_DESK_HEIGHT 0
#define MAX_DESK_HEIGHT 256

void latchPin(short pin) {
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
}

void unlatchPin(short pin) {
	pinMode(pin, INPUT);
	digitalWrite(pin, HIGH);
}

UpliftDeskClass::UpliftDeskClass() {
	_deskPacket = new DeskPacket();
}

void UpliftDeskClass::begin(short htx, short dtx, short hs0, short hs1, short hs2, short hs3) {
	_htx = htx;
	_dtx = dtx;
	_hs0 = hs0;
	_hs1 = hs1;

	// TODO We don't currently make use of HS2 yet. Do we actually need it?
	_hs2 = hs2;
	
	_hs3 = hs3;
	_deskSerial = new SoftwareSerial(_dtx);
	_deskSerial->begin(DESK_BAUD_RATE);
	pinMode(_dtx, INPUT);

	// TODO We don't currently actually make use of the serial connection from the handset/button pad.
	// TODO But we should probably handle commands from the more intelligent handsets at some point.
	// TODO The simpler handsets that just use the control lines (HS0 - HS3) will work fine though since
	// we pass through the electrical signals (ie. button pads with just up/down buttons). See schematic.
	_hsSerial = new SoftwareSerial(_htx);
	_hsSerial->begin(DESK_BAUD_RATE);
	pinMode(_htx, INPUT);

	latchPin(_hs0);
	latchPin(_hs3);
	delay(30);
	unlatchAll();
}

void UpliftDeskClass::readDeskSerial() {
	_heightChanged = false;
	while (_deskSerial->available()) {
		auto ch = _deskSerial->read();
		if (_deskPacket->put(ch)) {
			int height = _deskPacket->decode();
			if ((height != _height) && ((_height == -1) || abs(height - _height) < 5)) {
				#ifdef DEBUG
				Serial.printf("DEBUG: Read desk serial - height changed. new=%d old=%d\n", height, _height);
				#endif
				_height = height;
				_heightChanged = true;
			}
		}
	}
}

void UpliftDeskClass::moveDown() {
	if (_state != DeskState::MOVING_DOWN) {
		#ifdef DEBUG
		Serial.println(F("DEBUG: Moving down"));
		#endif
		_state = DeskState::MOVING_DOWN;
		latchPin(_hs0);
		unlatchPin(_hs1);
	}
}

void UpliftDeskClass::moveUp() {
	if (_state != DeskState::MOVING_UP) {
		#ifdef DEBUG
		Serial.println(F("DEBUG: Moving up"));
		#endif
		_state = DeskState::MOVING_UP;
		unlatchPin(_hs0);
		latchPin(_hs1);
	}
}

void UpliftDeskClass::stop() {
	if (_state != DeskState::STOPPED) {
		#ifdef DEBUG
		Serial.println(F("DEBUG: Movement stopped"));
		#endif
		_state = DeskState::STOPPED;
		_targetHeight = -1;
		unlatchAll();
	}
}

void UpliftDeskClass::goToHeight(int16_t targetHeight) {
	if (targetHeight >= MIN_DESK_HEIGHT && targetHeight <= MAX_DESK_HEIGHT) {
		_targetHeight = targetHeight;
		#ifdef DEBUG
		Serial.printf("DEBUG: New target height: %d\n", targetHeight);
		#endif
	}
	else {
		_targetHeight = -1;
		#ifdef DEBUG
		Serial.println(F("DEBUG: New target height invalid"));
		#endif
	}
}

void UpliftDeskClass::loop() {
	readDeskSerial();
	#ifdef DEBUG
	if (_heightChanged) {
		Serial.printf("DEBUG: Height: %d moving: %d: Target=%d\n", _height, _state, _targetHeight);
	}
	#endif

	if (_targetHeight > 0) {
		if (_height < _targetHeight - OFFSET_BELOW) {
			moveUp();
		}
		else if (_height > _targetHeight + OFFSET_ABOVE) {
			moveDown();
		}
		else {
			stop();
		}
	}
}

DeskState UpliftDeskClass::getState() {
	return _state;
}

String UpliftDeskClass::getStateString() {
	String stateStr = "UNKNOWN";
	switch (_state) {
		case DeskState::STOPPED:
			stateStr = "STOPPED";
			break;
		case DeskState::MOVING_DOWN:
			stateStr = "MOVING_DOWN";
			break;
		case DeskState::MOVING_UP:
			stateStr = "MOVING_UP";
			break;
		default:
			break;
	}

	return stateStr;
}

void UpliftDeskClass::unlatchAll() {
	unlatchPin(_hs0);
	unlatchPin(_hs1);
	unlatchPin(_hs2);
	unlatchPin(_hs3);
}

UpliftDeskClass UpliftDesk;