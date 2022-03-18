#include <Wire.h>
#include <TCA6416A.h>

TCA6416A tca;

void setup() {
	tca.begin(0); // Address 0 or 1, depending on your addr-pin

	tca.pin_mode(0, OUTPUT);
	tca.pin_mode(1, INPUT);
}

void loop() {
	tca.pin_write(0, HIGH);
	delay(200);
	tca.pin_write(0, LOW);
	delay(200;

	// If pin 1 (counted from 0) is high, stop the loop
	while (tca.pin_read(1) == HIGH);
}
