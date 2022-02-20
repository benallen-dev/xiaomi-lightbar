#include <Arduino.h>
#include <radio.h>
#include <printf.h>

void setup()
{
	// put your setup code here, to run once:
	Serial.begin(9600);
	printf_begin();
	Serial.println("Serial interface initialised");

	// setupRadioScanner();
	setupRadioTransmitter();
}

void loop()
{
	// updateRadioScanner();

	sendCommand();
	delay(2000);
}