
#include "LoRaPlusFSK.h"

#define GREEN_LED	25

void setup(void)
{
	Serial.begin(115200);
	
	while(!Serial);
	
	LoRaPlusFSK.setPins(18,23,26);
	LoRaPlusFSK.begin(433920000);
	
	pinMode(GREEN_LED,OUTPUT);
	digitalWrite(GREEN_LED,LOW);
}

void loop(void)
{
	uint8_t version=LoRaPlusFSK.readRegister(REG_VERSION);
	Serial.print("LoRa chip version: ");
	Serial.println(version);
	
	digitalWrite(GREEN_LED,HIGH);
	delay(50);
	digitalWrite(GREEN_LED,LOW);
	delay(50);
	digitalWrite(GREEN_LED,HIGH);
	delay(50);
	digitalWrite(GREEN_LED,LOW);
	delay(50);
	digitalWrite(GREEN_LED,HIGH);
	delay(50);
	digitalWrite(GREEN_LED,LOW);
	delay(750);
}

