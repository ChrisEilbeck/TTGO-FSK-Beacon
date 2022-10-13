
#ifdef ARDUINO_TBeam
	#include <axp20x.h>
	AXP20X_Class axp;
#endif

#define GREEN_LED	25

#include <EEPROM.h>
#include <RadioLib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{
	B00000000, B11000000,
	B00000001, B11000000,
	B00000001, B11000000,
	B00000011, B11100000,
	B11110011, B11100000,
	B11111110, B11111000,
	B01111110, B11111111,
	B00110011, B10011111,
	B00011111, B11111100,
	B00001101, B01110000,
	B00011011, B10100000,
	B00111111, B11100000,
	B00111111, B11110000,
	B01111100, B11110000,
	B01110000, B01110000,
	B00000000, B00110000
};

#define DISPLAYUPDATEPERIOD	250

#ifdef ARDUINO_TBeam
	#define LoRa_NSS	18
	#define LoRa_DIO0	26
	#define LoRa_RESET	14
	#define LoRa_DIO1	-1
#endif
#ifdef ARDUINO_TTGO_LoRa32_v21new
	#define LoRa_NSS	18
	#define LoRa_DIO0	26
	#define LoRa_RESET	23
	#define LoRa_DIO1	-1
#endif
#ifdef ARDUINO_TTGO_LoRa32_V1
	#define LoRa_NSS	5
	#define LoRa_DIO0	26
	#define LoRa_RESET	4
	#define LoRa_DIO1	-1
#endif

#define BATTERYVOLTAGE		35
#define BATTERYCALVALUE		1.734

#define EEPROM_SIZE 		8

#define MIN_FREQUENCY		430.000
#define DEFAULT_FREQUENCY	433.920
#define MAX_FREQUENCY		440.000

#define FINE_STEP			0.001
#define COARSE_STEP			0.005
#define VERY_COARSE_STEP	0.100

SX1278 radio=new Module(LoRa_NSS,LoRa_DIO0,LoRa_RESET,LoRa_DIO1);
float frequency=DEFAULT_FREQUENCY;
int txperiod=2000;

void tripleflash(void)
{
	digitalWrite(GREEN_LED,HIGH);
	delay(100);
	digitalWrite(GREEN_LED,LOW);
	delay(100);
	digitalWrite(GREEN_LED,HIGH);
	delay(100);
	digitalWrite(GREEN_LED,LOW);
	delay(100);
	digitalWrite(GREEN_LED,HIGH);
	delay(100);
	digitalWrite(GREEN_LED,LOW);
}

float readbatteryvoltage(void)
{
//	return(3850.0f);
	
	int adcvalue=analogRead(BATTERYVOLTAGE);
	
	return(BATTERYCALVALUE*(float)adcvalue);
}

void update_frequency(float freq)
{
	int state=radio.setFrequency(freq);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	Serial.print("Frequency: ");
	Serial.printf("%03.3f MHz",freq);
	Serial.println();
	
	// store the frequency to eeprom
	
	uint8_t buffer[4];
	memcpy(buffer,(uint8_t *)&freq,4);
	
    EEPROM.write(0,buffer[0]);
    EEPROM.write(1,buffer[1]);
    EEPROM.write(2,buffer[2]);
    EEPROM.write(3,buffer[3]);
	
    EEPROM.commit();
}

void drawtext(char *string,int y,int size)
{
	display.setCursor(64-8*strlen(string)*size/2,y);
	display.setTextSize(size);
	display.print(string);
}

void displaymenu(void)
{
	Serial.print("TTGO-FSK-BEACON\r\n===============\r\n\n");
	Serial.print("Command menu\r\n------------\r\n\n");
	Serial.print("\tt\t-\tTransmit a burst\r\n");
	Serial.print("\t0\t-\tRestore default settings\r\n");
	Serial.print("\t+/u/U\t-\tIncrease frequency in fine, coarse or very coarse steps\r\n");
	Serial.print("\t-/d/D\t-\tDecrease frequency in fine, coarse or very coarse steps\r\n");
	Serial.print("\tl/L\t-\tGreen LED on/off\r\n");
	Serial.print("\tr\t-\tEnter run mode\r\n");
	Serial.print("\tc\t-\tStay in config mode, don't exit to run mode\r\n");
	Serial.print("\tm\t-\tRe-display this menu\r\n");
	Serial.println();
}

int SetupPMIC(void)
{
#ifdef ARDUINO_TBeam
	Serial.print("Initialising the AXP192: ");
	if(!axp.begin(Wire, AXP192_SLAVE_ADDRESS))	{	Serial.println(" PASS\r\n");				} 
	else                                        {	Serial.println(" FAIL\r\n");	return(1);	}
	
	axp.setPowerOutPut(AXP192_DCDC1,AXP202_ON);
	axp.setPowerOutPut(AXP192_DCDC2,AXP202_OFF);
	axp.setPowerOutPut(AXP192_DCDC3,AXP202_ON);
	axp.setPowerOutPut(AXP192_LDO2,AXP202_ON);
	axp.setPowerOutPut(AXP192_LDO3,AXP202_OFF);
	axp.setPowerOutPut(AXP192_EXTEN,AXP202_ON);
	
	axp.setDCDC1Voltage(3300);
	
	Serial.printf("\tDCDC1 2v5: %s\r\n",axp.isDCDC1Enable()?"ENABLE":"DISABLE");
	Serial.printf("\tDCDC2 Not Used: %s\r\n",axp.isDCDC2Enable()?"ENABLE":"DISABLE");
	Serial.printf("\tDCDC3 3v3: %s\r\n",axp.isDCDC3Enable()?"ENABLE":"DISABLE");
	Serial.printf("\tLDO2 LoRa: %s\r\n",axp.isLDO2Enable()?"ENABLE":"DISABLE");
	Serial.printf("\tLDO3 GPS: %s\r\n",axp.isLDO3Enable()?"ENABLE":"DISABLE");
	Serial.printf("\tExten: %s\r\n\n",axp.isExtenEnable()?"ENABLE":"DISABLE");

	if(axp.isChargeingEnable())	{	Serial.print("Charging is enabled\r\n\n");	}
	else						{	Serial.print("Charging is disabled\r\n\n");	}
	
	axp.adc1Enable(AXP202_BATT_CUR_ADC1,true);
#endif
	
	return(0);
}

void setup(void)
{
	SetupPMIC();
	
	Serial.begin(115200);
	
	Serial.println("Press the C key to stay in config mode, you have 30 seconds ...");
	
	// initialize SX1278 FSK modem with default settings
//	Serial.print(F("[SX1278] Initializing ... "));
	
	int state = radio.beginFSK();
	if(state==RADIOLIB_ERR_NONE)
	{
		Serial.println(F("Radio module configured"));
	}
	else 
	{
		Serial.print(F("Radio module not found: failure code "));
		Serial.println(state);
		while (true);
	}
	
	// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
	if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
	{
		Serial.println(F("SSD1306 allocation failed"));
		for(;;); // Don't proceed, loop forever
	}
	
	// Clear the buffer
	display.clearDisplay();
	display.display();
	
  	EEPROM.begin(EEPROM_SIZE);
	
	uint8_t buffer[4];
	float freqval;
	
	buffer[0]=EEPROM.read(0);
	buffer[1]=EEPROM.read(1);
	buffer[2]=EEPROM.read(2);
	buffer[3]=EEPROM.read(3);
	
	memcpy((uint8_t *)&freqval,buffer,4);
	
	if(		(freqval<MIN_FREQUENCY)
		||	(freqval>MAX_FREQUENCY)		)
	{
		Serial.println("Stored frequency is invalid, subsituting the default");
		Serial.println(buffer[0],HEX);
		Serial.println(buffer[1],HEX);
		Serial.println(buffer[2],HEX);
		Serial.println(buffer[3],HEX);
		frequency=DEFAULT_FREQUENCY;
		update_frequency(frequency);
	}
	else
	{
		frequency=freqval;
		Serial.print("Frequency: ");
		Serial.printf("%03.3f MHz",frequency);
		Serial.println();
	}
	
	// if needed, you can switch between LoRa and FSK modes
	//
	// radio.begin()       start LoRa mode (and disable FSK)
	// radio.beginFSK()    start FSK mode (and disable LoRa)

	// the following settings can also
	// be modified at run-time
	state = radio.setFrequency(frequency);
//	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state = radio.setBitRate(2.4);
//	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state = radio.setFrequencyDeviation(10.0);
//	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state = radio.setRxBandwidth(250.0);
//	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state = radio.setOutputPower(3.0);
//	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state = radio.setCurrentLimit(100);
//	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state = radio.setDataShaping(RADIOLIB_SHAPING_0_5);
//	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state = radio.setEncoding(RADIOLIB_ENCODING_NRZ);
//	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	uint8_t syncWord[]={	0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa	};
	
	state = radio.setSyncWord(syncWord, 8);
//	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	pinMode(GREEN_LED,OUTPUT);
	digitalWrite(GREEN_LED,LOW);
	
	displaymenu();
}

void loop(void)
{
	static int runmode=0;
	static int configmode=0;
	
	if(millis()<(3*60*1000))
	{
		static int displaylastupdate=0;
		
		if(millis()>(displaylastupdate+DISPLAYUPDATEPERIOD))
		{
			float batvolt=readbatteryvoltage();
			
			display.clearDisplay();
			
			display.setTextColor(WHITE);
			
			display.setTextSize(1);
			display.setCursor(16,0);
			display.print("Battery Voltage");
			
			display.setTextSize(2);
			display.setCursor(24,12);
			display.printf("%4.0fmV",batvolt);
			
			display.setTextSize(1);
			display.setCursor(38,36);
			display.print("Frequency");
			
			display.setTextSize(2);
			display.setCursor(16,48);
			display.printf("%3.3fM",frequency);
			
			display.display(); 		
		
			displaylastupdate=millis();
		}
	}
	else
	{
		// turn the display off and everything else other than the radio that we can
		
		display.clearDisplay();
		display.display(); 		
	}
	
	if(!runmode)
	{
		int state;
		static int highlow=0;
		
		if(Serial.available())
		{
			char byte=Serial.read();
			
			Serial.write(byte);
			
			switch(byte)
			{
				case 't':
				case 'T':	{
								uint8_t TxPacket[256];
								uint16_t TxPacketLength=32;
								
								memset(TxPacket,0xaa,256);
								state=radio.transmit(TxPacket,TxPacketLength);
									
								if(state==RADIOLIB_ERR_NONE)					
								{
//									Serial.println(F("[SX1278] Packet transmitted successfully!"));	
								}
								else if(state==RADIOLIB_ERR_PACKET_TOO_LONG)	{	Serial.println(F("[SX1278] Packet too long!"));					}
								else if(state==RADIOLIB_ERR_TX_TIMEOUT)			{	Serial.println(F("[SX1278] Timed out while transmitting!"));	}
								else											{	Serial.println(F("[SX1278] Failed to transmit packet, code "));
																					Serial.println(state);											}
							}
							
							break;
				
				// frequency adjustment commands
				
				case '0':	frequency=DEFAULT_FREQUENCY;
							update_frequency(frequency);
							break;
				
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
				
				// misc features
				
				case 'l':	digitalWrite(GREEN_LED,HIGH);
							break;
				
				case 'L':	digitalWrite(GREEN_LED,LOW);
							break;
				
				case 'r':
				case 'R':	runmode=1;
							Serial.println("Entering run mode");
							tripleflash();
							break;
				
				case 'c':	configmode=!configmode;
							if(configmode)	Serial.println("Entering config mode, will not go into run mode after 30 seconds operation");
							else			Serial.println("Exiting config mode, will go into run mode after 30 seconds operation");
							
							break;
				
				case 'm':
				case 'M':	displaymenu();
							break;
							
				default:	// do nowt
							break;
			}
		}
		
		if(		!configmode
			&&	(millis()>(30*1000))	)
		{
			Serial.println("Exiting config mode");
			tripleflash();
			runmode=1;
		}
	}
	else
	{
		static int lasttx=0;
		
		if(millis()>(lasttx+txperiod))
		{
			uint8_t TxPacket[256];
			int state;
			
			lasttx=millis();
			
			memset(TxPacket,0xaa,32);
			digitalWrite(GREEN_LED,HIGH);
			state=radio.transmit(TxPacket,32);
			digitalWrite(GREEN_LED,LOW);
			
			if(state==RADIOLIB_ERR_NONE)
			{
//				Serial.println(F("[SX1278] Packet transmitted successfully!"));	
			}
			else if(state==RADIOLIB_ERR_PACKET_TOO_LONG)	{	Serial.println(F("[SX1278] Packet too long!"));					}
			else if(state==RADIOLIB_ERR_TX_TIMEOUT)			{	Serial.println(F("[SX1278] Timed out while transmitting!"));	}
			else											{	Serial.println(F("[SX1278] Failed to transmit packet, code "));
																Serial.println(state);											}
			delay(100);
			
			memset(TxPacket,0xaa,32);
			digitalWrite(GREEN_LED,HIGH);
			state=radio.transmit(TxPacket,32);
			digitalWrite(GREEN_LED,LOW);
			
			if(state==RADIOLIB_ERR_NONE)
			{
//				Serial.println(F("[SX1278] Packet transmitted successfully!"));
			}
			else if(state==RADIOLIB_ERR_PACKET_TOO_LONG)	{	Serial.println(F("[SX1278] Packet too long!"));					}
			else if(state==RADIOLIB_ERR_TX_TIMEOUT)			{	Serial.println(F("[SX1278] Timed out while transmitting!"));	}
			else											{	Serial.println(F("[SX1278] Failed to transmit packet, code "));
																Serial.println(state);											}
		}
	}
}

