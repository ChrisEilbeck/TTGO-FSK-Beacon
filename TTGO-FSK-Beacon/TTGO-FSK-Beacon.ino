
#ifdef ARDUINO_TBeam
	#include <axp20x.h>
	AXP20X_Class axp;
#endif

#ifndef ADAFRUIT_FEATHER_M0
	// eeprom is not supported by the chip on the Adafruit Feather M0
	#include <EEPROM.h>
#endif

#include <RadioLib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "SPIFFS.h"

#include "DefaultValues.h"
#include "HardwareAbstractionLayer.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

// Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_RESET     -1 
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,OLED_RESET);

//SX1278 radio=new Module(LORA_CS,LORA_IRQ,LORA_RST,LORA_D1);
SX1276 radio=new Module(LORA_CS,LORA_IRQ,LORA_RST,LORA_D1);

// set up some defaults in case we can't read the programmed values from the eeprom

int frequency_band=4;
float frequency=DEFAULT_FREQUENCY4;

int old_frequency_band=4;
float old_frequency=DEFAULT_FREQUENCY4;

float min_frequency=MIN_FREQUENCY4;
float default_frequency=DEFAULT_FREQUENCY4;
float max_frequency=MAX_FREQUENCY4;

// time between transmissions

int txperiod=2000;

int power_level=DEFAULT_POWER_LEVEL;

int display_starttime=0;

bool configmode=false;
bool displayon=true;
int displayonuntil=30000;	// on for 30s from boot

void setledon(void)
{
#ifdef ARDUINO_TBeam
	axp.setChgLEDMode(AXP20X_LED_LOW_LEVEL);
#else
	digitalWrite(LED_BUILTIN,HIGH);
#endif
}

void setledoff(void)
{
#ifdef ARDUINO_TBeam
	axp.setChgLEDMode(AXP20X_LED_OFF);
#else
	digitalWrite(LED_BUILTIN,LOW);
#endif
}

float readbatteryvoltage(void)
{
#ifdef ARDUINO_TBeam
	float batvolt=axp.getBattVoltage();
	return(batvolt);
#else
	int adcvalue=analogRead(BATTERY_VOLTAGE_PIN);
	return(BATTERY_CAL_VALUE*(float)adcvalue);
#endif
}

void update_frequency(float freq)
{
	Serial.print("Frequency: ");
	Serial.printf("%03.3f MHz",freq);
	Serial.println();
	
	int state=radio.setFrequency(freq);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
#ifndef ADAFRUIT_FEATHER_M0
	uint8_t buffer[4];
	memcpy(buffer,(uint8_t *)&freq,4);
	
	EEPROM.write(EEPROM_F1_ADDRESS,buffer[0]);	EEPROM.write(EEPROM_F2_ADDRESS,buffer[1]);	EEPROM.write(EEPROM_F3_ADDRESS,buffer[2]);	EEPROM.write(EEPROM_F4_ADDRESS,buffer[3]);
	EEPROM.commit();
#endif
}

void update_power_level(int power_level)
{
	Serial.print("Setting power level to ");
	Serial.print(power_level);
	Serial.println(" dBm");
	
	int state=radio.setOutputPower((float)power_level);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
#ifndef ADAFRUIT_FEATHER_M0
	EEPROM.write(EEPROM_POWER_LEVEL_ADDRESS,power_level);
	EEPROM.commit();
#endif
}

void displaymenu(void)
{
	Serial.print("\nTTGO-FSK-BEACON\r\n===============\r\n\n");
	Serial.print("Command menu\r\n------------\r\n\n");
	Serial.print("\tt\t-\tTransmit a burst\r\n");
	Serial.print("\t0\t-\tRestore default frequency\r\n");
	Serial.print("\t+/u/U\t-\tIncrease frequency in fine, coarse or very coarse steps\r\n");
	Serial.print("\t-/d/D\t-\tDecrease frequency in fine, coarse or very coarse steps\r\n");
	Serial.print("\tf\t-\tDisplay the current frequency\r\n");
	Serial.print("\t4\t-\tOperate in 433MHz band\r\n");
	Serial.print("\t8\t-\tOperate in 868MHz band\r\n");
	Serial.print("\t9\t-\tOperate in 915MHz band\r\n");
	Serial.print("\to\t-\tRestore the default power level\r\n");
	Serial.print("\tp\t-\tDecrease the power level\r\n");
	Serial.print("\tP\t-\tIncrease the power level\r\n");
	Serial.print("\ta|b|c\t-\tRecall stored frequency settings\r\n");
	Serial.print("\tA|B|C\t-\tStore frequency settings preset\r\n");
	Serial.print("\tv\t-\tDisplay the battery voltage\r\n");
	Serial.print("\tl/L\t-\tGreen LED on/off\r\n");
	Serial.print("\tr\t-\tEnter run mode\r\n");
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
	Wire.begin();
	SPI.begin();
	
	SetupPMIC();
	
	Serial.begin(115200);
	
#if 1
	if(Serial)
		configmode=true;
#endif
	
#ifndef ARDUINO_TBeam
	pinMode(LED_BUILTIN,OUTPUT);
	
	if(USER_BUTTON>=0)
		pinMode(USER_BUTTON,INPUT_PULLUP);
#endif
	
	setledoff();
	
	// SSD1306_SWITCHCAPVCC=generate display voltage from 3.3V internally
	if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C))
	{
		Serial.println(F("SSD1306 allocation failed"));
		for(;;); // Don't proceed, loop forever
	}
	
	// Clear the buffer and display a logo for 2 seconds
	display.clearDisplay();
	display.setTextColor(WHITE);
	display.setTextSize(2);
	display.setCursor(32,8);	display.print("TTGO");
	display.setCursor(40,24);	display.print("FSK");
	display.setCursor(18,40);	display.print("BEACON");
	display.display();
	delay(2000);
	
	// initialize SX1278 FSK modem with default settings
	Serial.print("Radio initializing ... ");
	
	int state=radio.beginFSK();
	if(state==RADIOLIB_ERR_NONE)
	{
		Serial.println("Radio module configured");
	}
	else 
	{
		Serial.print("Radio module not found: failure code ");
		Serial.println(state);
		
		display.setTextColor(WHITE);
		display.setTextSize(2);
		display.setCursor(0,0);
		display.print("Lora32\r\nError\r\n");
		display.println(state);
		
		if(state==RADIOLIB_ERR_CHIP_NOT_FOUND)
			display.print("Not found");
		
		display.display();
		
		while(true)	{	setledon();	delay(250);	setledoff();	delay(250);	}
	}
	
#ifndef ADAFRUIT_FEATHER_M0
  	EEPROM.begin(EEPROM_SIZE);
	
	uint8_t buffer[4];
	float freqval;
	
	buffer[0]=EEPROM.read(EEPROM_F1_ADDRESS);
	buffer[1]=EEPROM.read(EEPROM_F2_ADDRESS);
	buffer[2]=EEPROM.read(EEPROM_F3_ADDRESS);
	buffer[3]=EEPROM.read(EEPROM_F4_ADDRESS);
	
	#if 1
		Serial.println(buffer[0],HEX);
		Serial.println(buffer[1],HEX);
		Serial.println(buffer[2],HEX);
		Serial.println(buffer[3],HEX);
	#endif
	
	memcpy((uint8_t *)&freqval,buffer,4);
	
	if(isnan(freqval))
	{
		Serial.println("Stored frequency is invalid, subsituting the default");
		frequency=DEFAULT_FREQUENCY4;
		min_frequency=MIN_FREQUENCY4;
		max_frequency=MAX_FREQUENCY4;
		Serial.println();
	}
	else
		frequency=freqval;
	
	if(frequency<800.000)		{	min_frequency=MIN_FREQUENCY4;	max_frequency=MAX_FREQUENCY4;	default_frequency=DEFAULT_FREQUENCY4;	}
	else if(frequency<900.000)	{	min_frequency=MIN_FREQUENCY8;	max_frequency=MAX_FREQUENCY8;	default_frequency=DEFAULT_FREQUENCY8;	}
	else						{	min_frequency=MIN_FREQUENCY9;	max_frequency=MAX_FREQUENCY9;	default_frequency=DEFAULT_FREQUENCY9;	}
	
#else
	frequency=DEFAULT_FREQUENCY;
	min_frequency=MIN_FREQUENCY4;
	max_frequency=MAX_FREQUENCY4;
	default_frequency=DEFAULT_FREQUENCY4;
#endif
	
	Serial.print("Frequency: ");
	Serial.printf("%03.3f MHz",frequency);
	Serial.println();
	
#ifndef ADAFRUIT_FEATHER_M0
	power_level=EEPROM.read(EEPROM_POWER_LEVEL_ADDRESS);
#else
	power_level=DEFAULT_POWER_LEVEL;
#endif
	
	if(power_level>MAX_POWER_LEVEL)
		power_level=MAX_POWER_LEVEL;
	
	Serial.print("Setting power level to ");
	Serial.print(power_level);
	Serial.println(" dBm");
	
	state=radio.setFrequency(frequency);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Frequency Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state=radio.setBitRate(2.4);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("BitRate Success ...");		}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state=radio.setFrequencyDeviation(10.0);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Deviation Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state=radio.setOutputPower((float)power_level);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("PowerLevel Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state=radio.setCurrentLimit(100);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("CurrLimit Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state=radio.setDataShaping(RADIOLIB_SHAPING_0_5);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Shaping Success ...");		}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state=radio.setEncoding(RADIOLIB_ENCODING_NRZ);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Encoding Success ...");		}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	uint8_t syncWord[]={	0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa	};
	
	state=radio.setSyncWord(syncWord, 8);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("SyncWord Success ...");		}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state=radio.setRxBandwidth(50.0);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("ReceiveBW Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	displaymenu();
	
	SetupSPIFFS();
	
#if 1
	SetupWebServer();
#endif
}

void loop(void)
{
#if 0
	while(1)
	{
		int rssi=radio.getRSSI(NULL,false);
		Serial.println(rssi);
		
		delay(200);
	}
#endif
#if 1
	PollWebServer();
#endif
	
	if(displayon)
	{
		static int display_last_update=0;
		
		if(millis()>(display_last_update+DISPLAY_UPDATE_PERIOD))
		{
			float batvolt=readbatteryvoltage();
			
			display.clearDisplay();
			display.setTextColor(WHITE);
			
			display.setTextSize(1);		display.setCursor(16,0);	display.print("Battery Voltage");
			display.setTextSize(2);		display.setCursor(24,12);	display.printf("%4.0fmV",batvolt);
			display.setTextSize(1);		display.setCursor(38,36);	display.print("Frequency");
			display.setTextSize(2);		display.setCursor(16,48);	display.printf("%3.3fM",frequency);
			
			display.display(); 		
		
			display_last_update=millis();
		}
		
		if(!configmode)
			if(millis()>displayonuntil)
				displayon=false;
	}
	else
	{
		// turn the display off and everything else other than the radio that we can
		
		display.clearDisplay();
		display.display(); 		
	}
	
	if(configmode)
		do_config_mode();
	else
	{
		static int lasttx=0;
		
		if(millis()>(lasttx+txperiod))
		{
			uint8_t TxPacket[256];
			int state;
			
			lasttx=millis();
			
			int cnt;
			int burstcount=2;
			
			for(cnt=0;cnt<burstcount;cnt++)
			{
				memset(TxPacket,0xaa,32);
				setledon();
				state=radio.transmit(TxPacket,32);
				setledoff();
				
				if(state==RADIOLIB_ERR_NONE)
				{
#if 0
					Serial.println("[SX1278] Packet transmitted successfully!");
#endif
				}
				else if(state==RADIOLIB_ERR_PACKET_TOO_LONG)	{	Serial.println("Radio packet too long!");					}
				else if(state==RADIOLIB_ERR_TX_TIMEOUT)			{	Serial.println("Radio timed out while transmitting!");		}
				else											{	Serial.println("Radio failed to transmit packet, code ");
																	Serial.println(state);										}
				
				if(cnt!=(burstcount-1))
					delay(100);
			}
		}
		
		if(USER_BUTTON>=0)
		{
			static int last_user_button=0;
			int user_button=digitalRead(USER_BUTTON);
			
			if(user_button&&!last_user_button)
			{
				Serial.print("Button pressed\r\n");
				displayonuntil=millis()+30000;
				displayon=true;
			}
			
			last_user_button=user_button;
		}
		
		if(!displayon)
		{
			// sleep for 50ms
			esp_sleep_enable_timer_wakeup(50000);
			esp_light_sleep_start();
		}
	}
}

void do_config_mode(void)
{
	int state;
	
	if(!Serial)
	{
		display.clearDisplay();
		display.setTextColor(WHITE);
				
		display.setTextSize(2);		display.setCursor(0,0);		display.print("Entering");
									display.setCursor(0,16);	display.print("Normal");
									display.setCursor(0,32);	display.print("Operating");
									display.setCursor(0,48);	display.print("Mode");
		
		display.display();
		
		delay(2000);
		
		configmode=false;
	}
	
	if(Serial.available())
	{
		char byte=Serial.read();
		
		Serial.write(byte);
		Serial.println();
		
		switch(byte)
		{
			case 't':
			case 'T':	{
							uint8_t TxPacket[256];
							uint16_t TxPacketLength=32;
							
							memset(TxPacket,0xaa,256);
							setledon();
							state=radio.transmit(TxPacket,TxPacketLength);
							setledoff();
								
							if(state==RADIOLIB_ERR_NONE)					
							{
								Serial.println("Radio packet transmitted successfully!");	
							}
							else if(state==RADIOLIB_ERR_PACKET_TOO_LONG)	{	Serial.println("Radio packet too long!");					}
							else if(state==RADIOLIB_ERR_TX_TIMEOUT)			{	Serial.println("Radio timed out while transmitting!");		}
							else											{	Serial.println("Radio failed to transmit packet, code ");
																				Serial.println(state);										}
						}
						
						break;
			
#ifndef ADAFRUIT_FEATHER_M0
			// frequency adjustment commands
			
			case '0':	frequency=default_frequency;
						update_frequency(frequency);
						break;
			
			case 'f':	update_frequency(frequency);
						break;
			
			case '+':	if(frequency<max_frequency)	frequency+=FINE_STEP;
						if(frequency>max_frequency)	frequency=max_frequency;
						update_frequency(frequency);
						break;
			
			case '-':	if(frequency>min_frequency)	frequency-=FINE_STEP;
						if(frequency<min_frequency)	frequency=min_frequency;
						update_frequency(frequency);
						break;
			
			case 'u':	if(frequency<max_frequency)	frequency+=COARSE_STEP;
						if(frequency>max_frequency)	frequency=max_frequency;
						update_frequency(frequency);
						break;
						
			case 'd':	if(frequency>min_frequency)	frequency-=COARSE_STEP;
						if(frequency<min_frequency)	frequency=min_frequency;
						update_frequency(frequency);
						break;
						
			case 'U':	if(frequency<max_frequency)	frequency+=VERY_COARSE_STEP;
						if(frequency>max_frequency)	frequency=max_frequency;
						update_frequency(frequency);
						break;
						
			case 'D':	if(frequency>min_frequency)	frequency-=VERY_COARSE_STEP;
						if(frequency<min_frequency)	frequency=min_frequency;
						update_frequency(frequency);
						break;
#endif
			
			case '4':	Serial.println("Selecting operation in the 433MHz band");
						frequency_band=4;
						frequency=DEFAULT_FREQUENCY4;
						min_frequency=MIN_FREQUENCY4;
						max_frequency=MAX_FREQUENCY4;
						default_frequency=DEFAULT_FREQUENCY4;
						update_frequency(frequency);
						break;
			
			case '8':	Serial.println("Selecting operation in the 869MHz band");
						frequency_band=8;
						frequency=DEFAULT_FREQUENCY8;
						min_frequency=MIN_FREQUENCY8;
						max_frequency=MAX_FREQUENCY8;
						default_frequency=DEFAULT_FREQUENCY8;
						update_frequency(frequency);
						break;
			
			case '9':	Serial.println("Selecting operation in the 915MHz band");
						frequency_band=9;
						frequency=DEFAULT_FREQUENCY9;
						min_frequency=MIN_FREQUENCY9;
						max_frequency=MAX_FREQUENCY9;
						default_frequency=DEFAULT_FREQUENCY9;
						update_frequency(frequency);
						break;
			
			// power adjustment commands
			
			case 'p':	if(power_level>MIN_POWER_LEVEL)
						{
							power_level--;
							update_power_level(power_level);
						}
						
						break;
						
			case 'P':	if(power_level<MAX_POWER_LEVEL)
						{
							power_level++;
							update_power_level(power_level);
						}
						
						break;
			
			case 'o':	power_level=DEFAULT_POWER_LEVEL;
						update_power_level(power_level);
						
						break;
						
			// misc features
			
			case 'v':	Serial.print("Battery voltage: ");
						Serial.print(readbatteryvoltage());
						Serial.println("mV");
						break;
			
			case 'l':	setledon();
						break;
			
			case 'L':	setledoff();
						break;
			
			case 'r':
			case 'R':	configmode=false;
						Serial.println("Entering run mode");
						displayon=true;
						displayonuntil=millis()+30000;
						break;
			
			case 'a':
			case 'b':
			case 'c':	ReadStoredFrequencyPreset(byte-'a');
						break;
			
			case 'A':
			case 'B':
			case 'C':	SaveStoredFrequencyPreset(byte-'A');
						break;
			
			case 'm':
			case 'M':	displaymenu();
						break;
			
			case 'x':	{
							int cnt;
							for(cnt=0;cnt<32;cnt++)
							{
								if(cnt==16)	Serial.print("\r\n");
								
								Serial.printf("%02x ",EEPROM.read(cnt));
							}
							
							Serial.print("\r\n");
						}
						
						break;
						
			default:	// do nowt
						break;
		}
	}
}

void SaveStoredFrequencyPreset(int preset)
{
	uint8_t buffer[4];
	memcpy(buffer,(uint8_t *)&frequency,4);
	
	#if 1
		Serial.println(buffer[0],HEX);
		Serial.println(buffer[1],HEX);
		Serial.println(buffer[2],HEX);
		Serial.println(buffer[3],HEX);
	#endif
	
	EEPROM.write(EEPROM_PRESET_ADDRESS+4*preset+0,buffer[0]);
	EEPROM.write(EEPROM_PRESET_ADDRESS+4*preset+1,buffer[1]);
	EEPROM.write(EEPROM_PRESET_ADDRESS+4*preset+2,buffer[2]);
	EEPROM.write(EEPROM_PRESET_ADDRESS+4*preset+3,buffer[3]);

	EEPROM.commit();
	
	Serial.printf("Stored frequency of %03.3f MHz to Preset %c\r\n",frequency,'A'+preset);
}

void ReadStoredFrequencyPreset(int preset)
{
	float freqval;

	char buffer[4];
	
	buffer[0]=EEPROM.read(EEPROM_PRESET_ADDRESS+4*preset+0);
	buffer[1]=EEPROM.read(EEPROM_PRESET_ADDRESS+4*preset+1);
	buffer[2]=EEPROM.read(EEPROM_PRESET_ADDRESS+4*preset+2);
	buffer[3]=EEPROM.read(EEPROM_PRESET_ADDRESS+4*preset+3);
	
	#if 1
		Serial.println(buffer[0],HEX);
		Serial.println(buffer[1],HEX);
		Serial.println(buffer[2],HEX);
		Serial.println(buffer[3],HEX);
	#endif
	
	memcpy((uint8_t *)&freqval,buffer,4);
	
	if(isnan(freqval))
	{
		Serial.println("Stored frequency is invalid, subsituting the default");
		frequency=DEFAULT_FREQUENCY4;
		min_frequency=MIN_FREQUENCY4;
		max_frequency=MAX_FREQUENCY4;
		Serial.println();
	}
	else
	{
		frequency=freqval;
		Serial.printf("Recalled frequency of %03.3f MHz to Preset %c\r\n",frequency,'A'+preset);
	}
	
	if(frequency<800.000)		{	min_frequency=MIN_FREQUENCY4;	max_frequency=MAX_FREQUENCY4;	default_frequency=DEFAULT_FREQUENCY4;	}
	else if(frequency<900.000)	{	min_frequency=MIN_FREQUENCY8;	max_frequency=MAX_FREQUENCY8;	default_frequency=DEFAULT_FREQUENCY8;	}
	else						{	min_frequency=MIN_FREQUENCY9;	max_frequency=MAX_FREQUENCY9;	default_frequency=DEFAULT_FREQUENCY9;	}
	
}

