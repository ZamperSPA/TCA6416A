// TCA6416A module
#pragma once

//#ifndef TCA6416A_H
//#define TCA6416A_H

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

//#define DEBUG_TCA6416A

#define TCA6416A_BITS 8 //0 - 8
#define TCA6416A_DEFAULT_ADDRESS 0x20
#define TCA6416A_ADDRESS_INVALID 0xFF

// TCA6416A error code returns:
typedef enum {
	TCA6416A_ERROR_READ = -4,
	TCA6416A_ERROR_WRITE = -3,
	TCA6416A_ERROR_INVALID_ADDRESS = -2,
	TCA6416A_ERROR_UNDEFINED = -1,
	TCA6416A_ERROR_SUCCESS = 1
} TCA6416A_error_t;
const TCA6416A_error_t TCA6416A_SUCCESS = TCA6416A_ERROR_SUCCESS;

typedef enum {
	TCA6416A_REGISTER_INPUT_PORT_0 = 0x00, //0
	TCA6416A_REGISTER_INPUT_PORT_1 = 0x01, //0
	TCA6416A_REGISTER_OUTPUT_PORT_0 = 0x02, //1
	TCA6416A_REGISTER_OUTPUT_PORT_1 = 0x03, //1
	TCA6416A_REGISTER_POLARITY_INVERSION_0 = 0x04, //2
	TCA6416A_REGISTER_POLARITY_INVERSION_1 = 0x05, //2
	TCA6416A_REGISTER_CONFIGURATION_0 = 0x06, //3
	TCA6416A_REGISTER_CONFIGURATION_1 = 0x07, //3
	TCA6416A_REGISTER_INVALID
} TCA6416A_REGISTER_t;

// TCA6416A invert/normal values:
typedef enum {
	TCA6416A_RETAIN,
	TCA6416A_INVERT,
	TCA6416A_INVERT_END
} TCA6416A_invert_t;
/*
 TODO: If I understood the manual correctly, the command only needs to be sent to change active register
 so for performance I want to store the last used register, and if you are for example reading multiple times,
 you would not need to resend the command the second time to read two more bytes. And the same way about writing.
 The registers are paired, so each byte writes to register (REGISTER | (i % 2)) indefinitely.
 */

class TCA6416A {
public:
	TCA6416A();

	// begin initializes the Wire port and I/O expander
	TCA6416A_error_t begin(void);
	// give begin a TwoWire port to specify the I2C port
	TCA6416A_error_t begin(TwoWire& wirePort);

	// setDebugStream to enable library debug statements
	void setDebugStream(Stream& debugPort = Serial);

	// pinMode can set a pin (0-3) to INPUT or OUTPUT
	TCA6416A_error_t pinModes(uint8_t pin, uint8_t mode, uint16_t custom_address = TCA6416A_DEFAULT_ADDRESS);

	TCA6416A_error_t portMode(uint8_t mode, uint16_t custom_address = TCA6416A_DEFAULT_ADDRESS);

	// digitalWrite and write can be used to set a pin HIGH or LOW
	TCA6416A_error_t digitalWrite(uint8_t pin, uint8_t value, uint16_t custom_address = TCA6416A_DEFAULT_ADDRESS);
	TCA6416A_error_t write(uint8_t pin, uint8_t value, uint16_t custom_address = TCA6416A_DEFAULT_ADDRESS);


	TCA6416A_error_t digitalPortWrite(uint8_t value, uint16_t custom_address = TCA6416A_DEFAULT_ADDRESS);

	// readReg can be used to read the whole input register (4 bits)
	uint8_t readReg();

	// digitalRead and read can be used to read a pin (0-3)
	uint8_t digitalRead(uint8_t pin, uint16_t custom_address = TCA6416A_DEFAULT_ADDRESS);
	uint8_t read(uint8_t pin, uint16_t custom_address = TCA6416A_DEFAULT_ADDRESS);

	// invert and revert can be used to invert (or not) the I/O logic during a read
	TCA6416A_error_t invert(uint8_t pin, TCA6416A_invert_t inversion = TCA6416A_INVERT, uint16_t custom_address = TCA6416A_DEFAULT_ADDRESS);
	TCA6416A_error_t revert(uint8_t pin, uint16_t custom_address = TCA6416A_DEFAULT_ADDRESS);

private:
	TwoWire *_i2cPort;
	Stream *_debugPort;
	uint16_t _deviceAddress;
	TCA6416A_REGISTER_t _register;
//
	uint8_t getBitReg(uint8_t pin, uint8_t conf);
// I2C Read/Write
	TCA6416A_error_t readI2CBuffer(uint8_t* dest, TCA6416A_REGISTER_t startRegister, uint16_t len);
	TCA6416A_error_t writeI2CBuffer(uint8_t* src, TCA6416A_REGISTER_t startRegister, uint16_t len);
	TCA6416A_error_t readI2CRegister(uint8_t* dest, TCA6416A_REGISTER_t registerAddress);
	TCA6416A_error_t writeI2CRegister(uint8_t data, TCA6416A_REGISTER_t registerAddress);
};

//#endif // TCA6416A_H
