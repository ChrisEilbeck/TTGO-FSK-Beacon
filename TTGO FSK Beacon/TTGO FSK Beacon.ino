
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
#if 0
	uint8_t version=LoRaPlusFSK.readRegister(REG_VERSION);
	Serial.print("LoRa chip version: ");
	Serial.println(version);
#endif
#if 1
	LoRaPlusFSK.setFSKMode();
	LoRaPlusFSK.setTxPower(5);
	LoRaPlusFSK.setFSKBitRate(1000);
	
	uint8_t TxPacket[]={0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa};
	uint16_t TxPacketLength=32;
	
	LoRaPlusFSK.beginPacket(false);
	LoRaPlusFSK.write(TxPacket,TxPacketLength);
	LoRaPlusFSK.endPacket(false);
#endif
	
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

