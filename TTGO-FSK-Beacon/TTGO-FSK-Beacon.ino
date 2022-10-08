
//#include "LoRaPlusFSK.h"

#define GREEN_LED	25

#include <EEPROM.h>
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

#define MIN_FREQUENCY		430.000
#define DEFAULT_FREQUENCY	433.920
#define MAX_FREQUENCY		440.000

#define FINE_STEP			0.001
#define COARSE_STEP			0.005
#define VERY_COARSE_STEP	0.100

SX1278 radio=new Module(LoRa_NSS,LoRa_DIO0,LoRa_RESET,LoRa_DIO1);
float frequency=DEFAULT_FREQUENCY;

void update_frequency(float freq)
{
	int state=radio.setFrequency(freq);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	Serial.print("Frequency: ");
	Serial.printf("%03.3f MHz",freq);
	Serial.println();
	
	// store the frequency to eeprom
	
}

void setup(void)
{
	Serial.begin(115200);
	
	
	
	
	
	
	
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
	state = radio.setFrequency(frequency);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state = radio.setBitRate(1.2);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state = radio.setFrequencyDeviation(10.0);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state = radio.setRxBandwidth(250.0);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state = radio.setOutputPower(3.0);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state = radio.setCurrentLimit(100);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state = radio.setDataShaping(RADIOLIB_SHAPING_0_5);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	uint8_t syncWord[]={	0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa	};
	
	state = radio.setSyncWord(syncWord, 8);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	pinMode(GREEN_LED,OUTPUT);
	digitalWrite(GREEN_LED,LOW);
}

void loop(void)
{
	if(Serial)
	{
		uint8_t TxPacket[255];
		uint16_t TxPacketLength=8;
		int state;
		static int highlow=0;
		
		memset(TxPacket,0xaa,255);
		
		if(Serial.available())
		{
			char byte=Serial.read();
			
			Serial.write(byte);
			
			digitalWrite(GREEN_LED,HIGH);
			delay(50);
			digitalWrite(GREEN_LED,LOW);
			
			switch(byte)
			{
				case 'l':
				case 'L':	state = radio.setBitRate(1.2);
							if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
							highlow=0;
							break;
							
				case 'h':
				case 'H':	state = radio.setBitRate(2.4);
							if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
							highlow=1;
							break;
							
				case 't':
				case 'T':	
							state=radio.transmit(TxPacket,32*(highlow+1));
								
							if(state==RADIOLIB_ERR_NONE)					{	Serial.println(F("[SX1278] Packet transmitted successfully!"));	}
							else if(state==RADIOLIB_ERR_PACKET_TOO_LONG)	{	Serial.println(F("[SX1278] Packet too long!"));					}
							else if(state==RADIOLIB_ERR_TX_TIMEOUT)			{	Serial.println(F("[SX1278] Timed out while transmitting!"));	}
							else											{	Serial.println(F("[SX1278] Failed to transmit packet, code "));
																				Serial.println(state);											}
							
							break;
				
				// frequency adjustment commands
				
				case '+':	if(frequency<MAX_FREQUENCY)	frequency+=FINE_STEP;
							if(frequency>MAX_FREQUENCY)	frequency=MAX_FREQUENCY;
							update_frequency(frequency);
							break;
				
				case '-':	if(frequency>MIN_FREQUENCY)	frequency-=FINE_STEP;
							if(frequency<MIN_FREQUENCY)	frequency=MIN_FREQUENCY;
							update_frequency(frequency);
							break;
				
				case 'u':	if(frequency<MAX_FREQUENCY)	frequency+=COARSE_STEP;
							if(frequency>MAX_FREQUENCY)	frequency=MAX_FREQUENCY;
							update_frequency(frequency);
							break;
							
				case 'd':	if(frequency>MIN_FREQUENCY)	frequency-=COARSE_STEP;
							if(frequency<MIN_FREQUENCY)	frequency=MIN_FREQUENCY;
							update_frequency(frequency);
							break;
							
				case 'U':	if(frequency<MAX_FREQUENCY)	frequency+=VERY_COARSE_STEP;
							if(frequency>MAX_FREQUENCY)	frequency=MAX_FREQUENCY;
							update_frequency(frequency);
							break;
							
				case 'D':	if(frequency>MIN_FREQUENCY)	frequency-=VERY_COARSE_STEP;
							if(frequency<MIN_FREQUENCY)	frequency=MIN_FREQUENCY;
							update_frequency(frequency);
							break;
							
				
				default:	// do nowt
							break;
			}
		}
	}
	else
	{
		
		
		
	}

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

