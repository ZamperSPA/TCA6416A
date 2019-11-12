// TCA6416A module
// Copyright 2019 Håkon Nessjøen
#include <TCA6416A.h>

#ifdef DEBUG_TCA6416A
#define TCA6416A_DEBUG(x) if (_debugPort != NULL) { _debugPort->print(x);}
#define TCA6416A_DEBUGLN(x) if (_debugPort != NULL) { _debugPort->println(x);}
#define STORAGE(x) (x)
#else
#define TCA6416A_DEBUG(x)
#define TCA6416A_DEBUGLN(x)
#define STORAGE(x) (x)
#endif


TCA6416A::TCA6416A()
{
	_i2cPort = NULL;
	_debugPort = NULL;
	_deviceAddress = TCA6416A_ADDRESS_INVALID;
	_register = TCA6416A_REGISTER_CONFIGURATION_0;
}

// NB!: Only address 0 or 1
TCA6416A_error_t TCA6416A::begin(TwoWire &wirePort) 
{
	
	_deviceAddress = TCA6416A_DEFAULT_ADDRESS;
	_i2cPort = &wirePort;

	_i2cPort->begin();

	return TCA6416A_ERROR_SUCCESS;
}

void TCA6416A::setDebugStream(Stream& debugPort)
{
	_debugPort = &debugPort;
}

TCA6416A_error_t TCA6416A::pinMode(uint8_t pin, uint8_t mode, uint16_t custom_address)
{
	TCA6416A_error_t err;
	uint8_t cfgRegister = 0;

	if (pin > TCA6416A_BITS) return TCA6416A_ERROR_UNDEFINED;

	pin = getBitReg(pin, 3);
	_deviceAddress = TCA6416A_DEFAULT_ADDRESS | custom_address; //by Manux

	err = readI2CRegister(&cfgRegister, _register);
	if (err != TCA6416A_ERROR_SUCCESS)
	{
		return err;
	}
	cfgRegister &= ~(1 << pin); // Clear pin bit
	if (mode == INPUT) // Set the bit if it's being set to INPUT (opposite of Arduino)
	{
		cfgRegister |= (1 << pin);
	}
	return writeI2CRegister(cfgRegister, _register);
}

TCA6416A_error_t TCA6416A::write(uint8_t pin, uint8_t value, uint16_t custom_address)
{
	TCA6416A_error_t err;
	uint8_t outputRegister = 0;

	if (pin > TCA6416A_BITS) return TCA6416A_ERROR_UNDEFINED;

	pin = getBitReg(pin, 1);
	_deviceAddress = TCA6416A_DEFAULT_ADDRESS | custom_address; //by Manux
	err = readI2CRegister(&outputRegister, _register);
	if (err != TCA6416A_ERROR_SUCCESS)
	{
		return err;
	}
	// TODO: Break out of here if it's already set correctly
	outputRegister &= ~(1 << pin); // Clear pin bit
	if (value == HIGH) // Set the bit if it's being set to HIGH (opposite of Arduino)
	{
		outputRegister |= (1 << pin);
	}
	return writeI2CRegister(outputRegister, _register);
}

TCA6416A_error_t TCA6416A::digitalWrite(uint8_t pin, uint8_t value, uint16_t custom_address)
{
	return write(pin, value, custom_address);
}

uint8_t TCA6416A::readReg()
{
	TCA6416A_error_t err;
	uint8_t inputRegister = 0;

	err = readI2CRegister(&inputRegister, TCA6416A_REGISTER_INPUT_PORT_0);
	if (err != TCA6416A_ERROR_SUCCESS)
	{
		return err;
	}
	return (inputRegister & (0x0f));
}

uint8_t TCA6416A::read(uint8_t pin, uint16_t custom_address)
{
	TCA6416A_error_t err;
	uint8_t inputRegister = 0;

	if (pin > TCA6416A_BITS) return TCA6416A_ERROR_UNDEFINED;

	pin = getBitReg(pin, 0);
	_deviceAddress = TCA6416A_DEFAULT_ADDRESS | custom_address; //by Manux
	err = readI2CRegister(&inputRegister, _register);
	if (err != TCA6416A_ERROR_SUCCESS)
	{
		return err;
	}
	
	return (inputRegister & (1 << pin)) >> pin;
}


uint8_t TCA6416A::digitalRead(uint8_t pin, uint16_t custom_address)
{
	return read(pin, custom_address);
}

TCA6416A_error_t TCA6416A::invert(uint8_t pin, TCA6416A_invert_t inversion, uint16_t custom_address)
{
	TCA6416A_error_t err;
	uint8_t invertRegister = 0;

	if (pin > TCA6416A_BITS) return TCA6416A_ERROR_UNDEFINED;

	pin = getBitReg(pin, 2);
	_deviceAddress = custom_address; //by Manux
	err = readI2CRegister(&invertRegister, _register);
	if (err != TCA6416A_ERROR_SUCCESS)
	{
		return err;
	}
	// TODO: Break out of here if it's already set correctly
	invertRegister &= ~(1 << pin); // Clear pin bit
	if (inversion == TCA6416A_INVERT) // Set the bit if it's being set to inverted
	{
		invertRegister |= (1 << pin);
	}
	return writeI2CRegister(invertRegister, _register);
}

TCA6416A_error_t TCA6416A::revert(uint8_t pin, uint16_t custom_address)
{
	return invert(pin, TCA6416A_RETAIN, custom_address);
}

uint8_t TCA6416A::getBitReg(uint8_t pin, uint8_t conf)
{
	uint8_t asd = 0;

	if(conf == 0)
		_register = TCA6416A_REGISTER_INPUT_PORT_0;
	else if(conf == 1)
		_register = TCA6416A_REGISTER_OUTPUT_PORT_0;
	else if(conf == 2)
		_register = TCA6416A_REGISTER_POLARITY_INVERSION_0;
	else if (conf == 3)
		_register = TCA6416A_REGISTER_CONFIGURATION_0;

	if(pin > 7) {
		pin = pin - 8;
		if (conf == 0)
			_register = TCA6416A_REGISTER_INPUT_PORT_1;
		else if (conf == 1)
			_register = TCA6416A_REGISTER_OUTPUT_PORT_1;
		else if (conf == 2)
			_register = TCA6416A_REGISTER_POLARITY_INVERSION_1;
		else if (conf == 3)
			_register = TCA6416A_REGISTER_CONFIGURATION_1;
	}
	return pin;
}

TCA6416A_error_t TCA6416A::readI2CBuffer(uint8_t* dest, TCA6416A_REGISTER_t startRegister, uint16_t len)
{
	TCA6416A_DEBUGLN((STORAGE("(readI2CBuffer): read ") + String(len) +
		STORAGE(" @ 0x") + String(startRegister, HEX)));
	if (_deviceAddress == TCA6416A_ADDRESS_INVALID)
	{
		TCA6416A_DEBUGLN(STORAGE("    ERR (readI2CBuffer): Invalid address"));
		return TCA6416A_ERROR_INVALID_ADDRESS;
	}
	_i2cPort->beginTransmission((uint8_t)_deviceAddress);
	_i2cPort->write(startRegister);
	if (_i2cPort->endTransmission(false) != 0)
	{
		TCA6416A_DEBUGLN(STORAGE("    ERR (readI2CBuffer): End transmission"));
		return TCA6416A_ERROR_READ;
	}

	_i2cPort->requestFrom((uint8_t)_deviceAddress, (uint8_t)len);
	for (int i = 0; i < len; i++)
	{
		dest[i] = _i2cPort->read();
		TCA6416A_DEBUGLN((STORAGE("    ") + String(i) + STORAGE(": 0x") + String(dest[i], HEX)));
	}

	return TCA6416A_ERROR_SUCCESS;
}

TCA6416A_error_t TCA6416A::writeI2CBuffer(uint8_t* src, TCA6416A_REGISTER_t startRegister, uint16_t len)
{
	if (_deviceAddress == TCA6416A_ADDRESS_INVALID)
	{
		TCA6416A_DEBUGLN(STORAGE("ERR (readI2CBuffer): Invalid address"));
		return TCA6416A_ERROR_INVALID_ADDRESS;
	}
	_i2cPort->beginTransmission((uint8_t)_deviceAddress);
	_i2cPort->write(startRegister);
	for (int i = 0; i < len; i++)
	{
		_i2cPort->write(src[i]);
	}
	if (_i2cPort->endTransmission(true) != 0)
	{
		return TCA6416A_ERROR_WRITE;
	}
	return TCA6416A_ERROR_SUCCESS;
}

TCA6416A_error_t TCA6416A::readI2CRegister(uint8_t* dest, TCA6416A_REGISTER_t registerAddress)
{
	return readI2CBuffer(dest, registerAddress, 1);
}

TCA6416A_error_t TCA6416A::writeI2CRegister(uint8_t data, TCA6416A_REGISTER_t registerAddress)
{
	return writeI2CBuffer(&data, registerAddress, 1);
}
