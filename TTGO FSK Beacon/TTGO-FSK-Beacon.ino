
//#include "LoRaPlusFSK.h"

#define GREEN_LED	25

#include <RadioLib.h>

// SX1278 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// RESET pin: 9
// DIO1 pin:  3

#define LoRa_NSS	18
#define LoRa_DIO0	26
#define LoRa_RESET	-1
#define LoRa_DIO1	-1

SX1278 radio=new Module(LoRa_NSS,LoRa_DIO0,LoRa_RESET,LoRa_DIO1);

void setup(void)
{
	Serial.begin(115200);
	
	while(!Serial);
	
#if 0
	LoRaPlusFSK.setFSKMode();
	LoRaPlusFSK.setTxPower(5);

	LoRaPlusFSK.setPins(18,23,26);
	LoRaPlusFSK.begin(433920000);
#endif
#if 1
	// initialize SX1278 FSK modem with default settings
	Serial.print(F("[SX1278] Initializing ... "));
	
	int state = radio.beginFSK();
	if(state==RADIOLIB_ERR_NONE)
	{
		Serial.println(F("success!"));
	}
	else 
	{
		Serial.print(F("failed, code "));
		Serial.println(state);
		while (true);
	}

	// if needed, you can switch between LoRa and FSK modes
	//
	// radio.begin()       start LoRa mode (and disable FSK)
	// radio.beginFSK()    start FSK mode (and disable LoRa)

	// the following settings can also
	// be modified at run-time
	state = radio.setFrequency(433.920);
	if(state!=RADIOLIB_ERR_NONE)	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state = radio.setBitRate(1200.0);
	if(state!=RADIOLIB_ERR_NONE)	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state = radio.setFrequencyDeviation(10.0);
	state = radio.setRxBandwidth(250.0);
	state = radio.setOutputPower(3.0);
	state = radio.setCurrentLimit(100);
	state = radio.setDataShaping(RADIOLIB_SHAPING_0_5);
	uint8_t syncWord[]={	0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa	};
	
	state = radio.setSyncWord(syncWord, 8);
	if(state != RADIOLIB_ERR_NONE)
	{
		Serial.print(F("Unable to set configuration, code "));
		Serial.println(state);
		while (true);
	}
#endif
	
	pinMode(GREEN_LED,OUTPUT);
	digitalWrite(GREEN_LED,LOW);
}

void loop(void)
{
//	uint8_t TxPacket[]={0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa};
	uint8_t TxPacket[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	uint16_t TxPacketLength=8;
	
	if(Serial.available())
	{
		char byte=Serial.read();
		
		Serial.write(byte);
		
		digitalWrite(GREEN_LED,HIGH);
		delay(50);
		digitalWrite(GREEN_LED,LOW);
		
		switch(byte)
		{
			case 't':
			case 'T':	
#if 0
						LoRaPlusFSK.writeRegister(REG_PACKET_CONFIG1,0b10010000);
						LoRaPlusFSK.writeRegister(REG_PACKET_CONFIG2,0b01000000);
						
						LoRaPlusFSK.writeRegister(REG_FSK_PREAMBLE_MSB,0);
						LoRaPlusFSK.writeRegister(REG_FSK_PREAMBLE_LSB,8);
						
//						LoRaPlusFSK.writeRegister(REG_FSK_PAYLOAD_LENGTH,8);
//						LoRaPlusFSK.writeRegister(REG_PAYLOAD_LENGTH,8);
							
//						LoRaPlusFSK.setPayloadLength(TxPacketLength);
						LoRaPlusFSK.setFSKBitRate(1200);
						
						LoRaPlusFSK.beginPacket(false);
						
#if 0
						LoRaPlusFSK.writeRegister(REG_FIFO,0x08);	// length
						
						LoRaPlusFSK.writeRegister(REG_FIFO,0xaa);
						LoRaPlusFSK.writeRegister(REG_FIFO,0xaa);
						LoRaPlusFSK.writeRegister(REG_FIFO,0xaa);
						LoRaPlusFSK.writeRegister(REG_FIFO,0xaa);
						LoRaPlusFSK.writeRegister(REG_FIFO,0xaa);
						LoRaPlusFSK.writeRegister(REG_FIFO,0xaa);
						LoRaPlusFSK.writeRegister(REG_FIFO,0xaa);
						LoRaPlusFSK.writeRegister(REG_FIFO,0xaa);
#else
						LoRaPlusFSK.write(TxPacket,TxPacketLength);
#endif
						
						LoRaPlusFSK.endPacket(false);
#endif
						{
//							int state = radio.transmit("Hello World!");
							
							uint8_t byteArr[]={	0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
												0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa		};
												
							int state=radio.transmit(byteArr,sizeof(byteArr));
							
							if(state==RADIOLIB_ERR_NONE)
							{
								Serial.println(F("[SX1278] Packet transmitted successfully!"));
							}
							else if(state==RADIOLIB_ERR_PACKET_TOO_LONG)
							{
								Serial.println(F("[SX1278] Packet too long!"));
							}
							else if(state==RADIOLIB_ERR_TX_TIMEOUT)
							{
								Serial.println(F("[SX1278] Timed out while transmitting!"));
							}
							else
							{
								Serial.println(F("[SX1278] Failed to transmit packet, code "));
								Serial.println(state);
							}
						}
						
						break;
			
			case 'd':
			case 'D':
#if 0
						LoRaPlusFSK.dumpRegisters(Serial);
#endif
						
						
						break;
			
			default:	// do nowt
						break;
		}
	}
	
#if 0
	uint8_t version=LoRaPlusFSK.readRegister(REG_VERSION);
	Serial.print("LoRa chip version: ");
	Serial.println(version);
#endif
#if 0
	
	uint8_t TxPacket[]={0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa};
	uint16_t TxPacketLength=32;
	
	LoRaPlusFSK.beginPacket(false);
	LoRaPlusFSK.write(TxPacket,TxPacketLength);
	LoRaPlusFSK.endPacket(false);
#endif
#if 0
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
#endif
}

