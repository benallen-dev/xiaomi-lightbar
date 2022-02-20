#include <RF24.h>
#include <SPI.h>

#define PIN_RADIO_CE 9
#define PIN_RADIO_CSN 10

#define SEND_ATTEMPTS 20

const static uint8_t RADIO_ID = 0;
const static uint8_t DESTINATION_RADIO_ID = 1;

RF24 radio(PIN_RADIO_CE, PIN_RADIO_CSN);
const uint64_t address = 0xAAAAAAAAAAAA;

uint8_t counter = 0;

void setupRadioScanner()
{
	radio.begin();
	radio.openReadingPipe(0, address);

	radio.setChannel(68);
	radio.setDataRate(RF24_2MBPS);
	radio.disableCRC();
	radio.disableDynamicPayloads();
	radio.setPayloadSize(17);
	radio.setAutoAck(false);

	radio.startListening();
	radio.printPrettyDetails();
}

void updateRadioScanner()
{
	if (radio.available())
	{
		byte data[18] = {0};

		radio.read(&data, sizeof(data));

		if (data[0] == 0x67 && data[1] == 0x22 && data[13] == 0x20)
		// if (data[0] == 0x67 && data[1] == 0x22)
		{
			Serial.println("=========================");
			Serial.println("Light Bar Packet received");
			Serial.println("=========================");

			for (unsigned long i = 0; i < sizeof(data); i++)
			{
				if (i == 0)
					Serial.println("------ address");
				if (i == 7)
					Serial.println("------ product serial");
				if (i == 11)
					Serial.println("------ packet ID counter, I think. Avoids duplicate execution");
				if (i == 13)
					Serial.println("------ action");
				if (i == 14)
					Serial.println("------ checksum? parameters? noise? idk");
				Serial.print(i, DEC);
				Serial.print(": 0x");
				Serial.print(data[i], HEX);
				if (i == 13 && data[i] == 0x80)
					Serial.print(" | brightness +");
				if (i == 13 && data[i] == 0xBF)
					Serial.print(" | brightness -");
				if (i == 13 && data[i] == 0x20)
					Serial.print(" | on/off");
				if (i == 13 && data[i] == 0x7F)
					Serial.print(" | temperature - ");
				if (i == 13 && data[i] == 0x40)
					Serial.print(" | temperature + ");
				Serial.println();
			}

			Serial.println("=========================");
			Serial.println();
		}
	}
};

void setupRadioTransmitter()
{

	radio.begin();
	radio.openReadingPipe(0, address);

	Serial.println("Set to channel 68...");
	radio.setChannel(68);

	Serial.println("Set data rate to 2MBPS...");
	radio.setDataRate(RF24_2MBPS);

	Serial.println("Disable CRC...");
	radio.disableCRC();

	Serial.println("Disable dynamic payloads...");
	radio.disableDynamicPayloads();

	Serial.println("Disable auto-ACK...");
	radio.setAutoAck(false);

	// radio.startListening();
	// radio.printPrettyDetails();

	Serial.println("Set PA Level LOW...");
	radio.setPALevel(RF24_PA_LOW);

	Serial.println("Set payload size");
	radio.setPayloadSize(17);

	Serial.println("Set retries");
	radio.setRetries(15, 15);

	Serial.println("Open writing pipe");
	radio.openWritingPipe(address); // always uses pipe 0

	Serial.println("Stop listening");
	radio.stopListening();
}

void sendCommand()
{

	uint8_t commands[5][17] = {
		{0x67,
		 0x22,
		 0x9B,
		 0xA3,
		 0x89,
		 0x26,
		 0x82,
		 0x4A,
		 0x2D,
		 0xE4,
		 0x3F,
		 0xED,
		 0xA0,
		 0x20,
		 0x1B,
		 0xE5,
		 0x12},
		{0x67,
		 0x22,
		 0x9B,
		 0xA3,
		 0x89,
		 0x26,
		 0x82,
		 0x4A,
		 0x2D,
		 0xE4,
		 0x3F,
		 0xF0,
		 0x40,
		 0x20,
		 0xA,
		 0x65,
		 0x13},
		{0x67,
		 0x22,
		 0x9B,
		 0xA3,
		 0x89,
		 0x26,
		 0x82,
		 0x4A,
		 0x2D,
		 0xE4,
		 0x3F,
		 0xEE,
		 0x0,
		 0x20,
		 0x39,
		 0xC7,
		 0x66},
		{0x67,
		 0x22,
		 0x9B,
		 0xA3,
		 0x89,
		 0x26,
		 0x82,
		 0x4A,
		 0x2D,
		 0xE4,
		 0x3F,
		 0xF5,
		 0x40,
		 0x20,
		 0xF,
		 0x89,
		 0xE3},
		{0x67,
		 0x22,
		 0x9B,
		 0xA3,
		 0x89,
		 0x26,
		 0x82,
		 0x4A,
		 0x2D,
		 0xE4,
		 0x3F,
		 0xF7,
		 0x40,
		 0x20,
		 0x7,
		 0xE5,
		 0x93}};

	Serial.println("sendCommand");
	// uint8_t commandOnOff[17] = {
	// 	0x67,
	// 	0x22,
	// 	0x9B,
	// 	0xA3,
	// 	0x89,
	// 	0x26,
	// 	0x82,
	// 	0x4A,
	// 	0x2D,
	// 	0xE4,
	// 	0x3F,
	// 	0xE0, // 11: counter
	// 	0x80,
	// 	0x20, // 13: command
	// 	0x0D,
	// 	0xDB,
	// 	0xD2};

	counter++;

	// Serial.println("transmitting");
	// Serial.println(commandOnOff[11]);
	for (int i = 0; i < 17; i++)
	{
		bool report = radio.write(&commands[counter % 5], sizeof(commands[0]), true);

		if (!report)
			Serial.println("write fail...");

		delay(20);
	}
}
