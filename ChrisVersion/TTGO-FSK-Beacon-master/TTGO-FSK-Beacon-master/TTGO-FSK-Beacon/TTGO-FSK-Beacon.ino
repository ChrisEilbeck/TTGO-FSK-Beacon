
/*
 * Extended life mode, enable and active flags, switches from two 
 * transmit bursts at configured power and cadent to one transmit 
 * burst at full power every 5 seconds.
 * 
 * Add cadence setting to test harness, add support to the scheduler for this
 * 
 * Integrate the web config interface
 */

#include <RadioLib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "SPIFFS.h"

#include "DefaultValues.h"
#include "HardwareAbstractionLayer.h"
#include "TTGO-FSK-Beacon.h"
	
#ifdef ARDUINO_TBeam
	#include <axp20x.h>
	AXP20X_Class axp;
#endif

#ifndef ADAFRUIT_FEATHER_M0
	// eeprom is not supported by the chip on the Adafruit Feather M0
	#include <EEPROM.h>
#endif

#if SUPPORT_WEBSERVER
	extern IPAddress apIP;
#endif

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_RESET     -1 
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,OLED_RESET);

SX1276 radio=new Module(LORA_CS,LORA_IRQ,LORA_RST,LORA_D1);

// settings for the current radio configuration

channel_settings current;

// extended life mode flags

//bool extended_life_mode_active=false;

// set up some defaults in case we can't read the programmed values from the eeprom

int old_frequency_band=4;
float old_frequency=DEFAULT_FREQUENCY4;
float min_frequency=MIN_FREQUENCY4;
float default_frequency=DEFAULT_FREQUENCY4;
float max_frequency=MAX_FREQUENCY4;

bool configmode=false;

int display_starttime=0;
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
//	Serial.print("Reading from PMIC\r\n");
	float batvolt=axp.getBattVoltage();
	return(batvolt);
#else
//	Serial.print("Reading analogue pin\r\n");
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
	
	SaveStoredFrequencyPreset(0);	
}

void update_power_level(int power_level)
{
	Serial.print("Setting power level to ");
	Serial.print(power_level);
	Serial.println(" dBm");
	
	int state=radio.setOutputPower((float)power_level);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	SaveStoredFrequencyPreset(0);	
}

void displaymenu(void)
{
	Serial.print("\nTTGO-FSK-BEACON\r\n===============\r\n\n");
	Serial.print("Command menu\r\n------------\r\n\n");
#if 0
	Serial.print("\tt\t-\tTransmit a burst\r\n");
#else
	Serial.print("\tt\t-\tGo into normal operating mode\r\n");
	Serial.print("\tT\t-\tStay in config mode with transmitter turned off\r\n");
#endif
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
#if 0
	Serial.print("\tr\t-\tEnter run mode\r\n");
#endif
	Serial.print("\tX\t-\tReset to default settings\r\n");
	Serial.print("\tm\t-\tRe-display this menu\r\n");
	Serial.print("\t.\t-\tTurn the display back on for 30 seconds\r\n");
	Serial.println();
}

int SetupPMIC(void)
{
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
	
	return(0);
}

void setup(void)
{
	Wire.begin();
	SPI.begin();
	
	Serial.begin(115200);
	
#ifdef ARDUINO_TBeam
	SetupPMIC();
#else
	pinMode(LED_BUILTIN,OUTPUT);
#endif
	
	if(USER_BUTTON>=0)
		pinMode(USER_BUTTON,INPUT);
	
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

#if 0
	delay(2000);
#endif
	
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
	ReadStoredFrequencyPreset(0);
#else
	current.frequency=DEFAULT_FREQUENCY;
	min_frequency=MIN_FREQUENCY4;
	max_frequency=MAX_FREQUENCY4;
	default_frequency=DEFAULT_FREQUENCY4;
#endif
	
	Serial.printf("Frequency: %03.3f MHz\r\n",current.frequency);
	
#ifndef ADAFRUIT_FEATHER_M0
#else
	current.power_level=DEFAULT_POWER_LEVEL;
#endif
	
	if(current.power_level>MAX_POWER_LEVEL)
		current.power_level=MAX_POWER_LEVEL;
	
	Serial.printf("Setting power level to %d dBm\r\n",current.power_level);
	
	state=radio.setFrequency(current.frequency);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Frequency Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state=radio.setBitRate(2.4);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("BitRate Success ...");		}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state=radio.setFrequencyDeviation(10.0);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Deviation Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state=radio.setOutputPower((float)current.power_level);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("PowerLevel Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state=radio.setCurrentLimit(100);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("CurrLimit Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state=radio.setDataShaping(RADIOLIB_SHAPING_0_5);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Shaping Success ...");		}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state=radio.setEncoding(RADIOLIB_ENCODING_NRZ);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("Encoding Success ...");		}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	uint8_t syncWord[]={	0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa	};
	
	state=radio.setSyncWord(syncWord,8);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("SyncWord Success ...");		}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	state=radio.setRxBandwidth(50.0);
	if(state==RADIOLIB_ERR_NONE)	{	Serial.println("ReceiveBW Success ...");	}	else	{	Serial.print(F("failed, code "));	Serial.println(state);	}
	
	displaymenu();
	
#if SUPPORT_WEBSERVER
	SetupSPIFFS();
	StartWebServer();
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
#if SUPPORT_WEBSERVER
	PollWebServer();
#endif
	
	PollDisplay();
	PollConfigMode();
	PollScheduler();
	CheckUserButton();
	
	if(current.use_extended_life_mode)
	{
		if(millis()>15*1000*60)
		{
			// after 15 minutes of operation, switch power to 
			// maximum and reduce cadence to one transmission 
			// every 5 seconds			
			radio.setOutputPower(MAX_POWER_LEVEL);
			current.tx_period=100;	// 100 periods of 50ms
		}
	}
	
	if(!displayon)
	{
		// sleep for 10ms
		esp_sleep_enable_timer_wakeup(10000);
		esp_light_sleep_start();
	}
}

void PollDisplay(void)
{
	if(millis()>2000)
	{
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
				display.setTextSize(1);		display.setCursor(38,28);	display.print("Frequency");
				display.setTextSize(2);		display.setCursor(16,40);	display.printf("%3.3fM",current.frequency);
				
#if SUPPORT_WEBSERVER
				display.setTextSize(1);		display.setCursor(16,56);	display.print("http://");	display.print(apIP);	display.print("/");
#endif
				
				display.display(); 		
			
				display_last_update=millis();
			}
		}
		else
		{
			// turn the display off and everything else other than the radio that we can
			
			display.clearDisplay();
			display.display();
		}
		
		if(		!configmode
			&&	displayon
			&&	(millis()>displayonuntil)	)
		{
			Serial.print("Turning the display off and entering low power mode\r\n");
			displayon=false;
			
#if SUPPORT_WEBSERVER
			StopWebServer();
#endif
		}
	}
}

int numbursts=5;
int lasttx=5000;
int nexttx=0;

void PollScheduler(void)
{
	if(millis()>(lasttx+current.tx_period*MS_PER_TICK))
	{
		if(current.tx_period<100)	numbursts=2;
		else						numbursts=1;
		
		nexttx=millis();
		lasttx=millis();
	}
	
	if(numbursts>0)
	{
		if(millis()>nexttx)
		{
			TransmitFSKBurst();
			numbursts--;
			nexttx=millis()+200;	// next burst in 200ms from now
		}
	}
}

void TransmitFSKBurst(void)
{
#if 0
	setledon();
	Serial.print("TX ");
	setledoff();
#else
	if(!configmode)
	{
		uint8_t TxPacket[32];
		int state;
				
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
	}
#endif
}

void CheckUserButton(void)
{
	if(USER_BUTTON>=0)
	{
		static int last_user_button=1;
		int user_button=digitalRead(USER_BUTTON);
		
		if(user_button&&!last_user_button)
		{
			Serial.print("Button pressed\r\n");
			displayonuntil=millis()+30000;
			displayon=true;
		}
		
		last_user_button=user_button;
	}	
}

void PollConfigMode(void)
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
#if 0
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
#else
			case 't':	Serial.print("Entering normal operating mode\r\n");
						configmode=false;
						displayon=true;
						displayonuntil=millis()+30000;
						break;
			
			case 'T':	Serial.print("Entering config mode and disabling the transmitter\r\n");
						configmode=true;
						break;
#endif
						
			// frequency adjustment commands
			
			case '0':	current.frequency=default_frequency;
						update_frequency(current.frequency);
						break;
			
			case 'f':	update_frequency(current.frequency);
						break;
			
			case '+':	if(current.frequency<max_frequency)	current.frequency+=FINE_STEP;
						if(current.frequency>max_frequency)	current.frequency=max_frequency;
						update_frequency(current.frequency);
						break;
			
			case '-':	if(current.frequency>min_frequency)	current.frequency-=FINE_STEP;
						if(current.frequency<min_frequency)	current.frequency=min_frequency;
						update_frequency(current.frequency);
						break;
			
			case 'u':	if(current.frequency<max_frequency)	current.frequency+=COARSE_STEP;
						if(current.frequency>max_frequency)	current.frequency=max_frequency;
						update_frequency(current.frequency);
						break;
						
			case 'd':	if(current.frequency>min_frequency)	current.frequency-=COARSE_STEP;
						if(current.frequency<min_frequency)	current.frequency=min_frequency;
						update_frequency(current.frequency);
						break;
						
			case 'U':	if(current.frequency<max_frequency)	current.frequency+=VERY_COARSE_STEP;
						if(current.frequency>max_frequency)	current.frequency=max_frequency;
						update_frequency(current.frequency);
						break;
						
			case 'D':	if(current.frequency>min_frequency)	current.frequency-=VERY_COARSE_STEP;
						if(current.frequency<min_frequency)	current.frequency=min_frequency;
						update_frequency(current.frequency);
						break;
			
			case '4':	Serial.println("Selecting operation in the 433MHz band");
						current.frequency=DEFAULT_FREQUENCY4;
						current.frequency_band=4;
						min_frequency=MIN_FREQUENCY4;
						max_frequency=MAX_FREQUENCY4;
						default_frequency=DEFAULT_FREQUENCY4;
						update_frequency(current.frequency);
						break;
			
			case '8':	Serial.println("Selecting operation in the 869MHz band");
						current.frequency=DEFAULT_FREQUENCY8;
						current.frequency_band=8;
						min_frequency=MIN_FREQUENCY8;
						max_frequency=MAX_FREQUENCY8;
						default_frequency=DEFAULT_FREQUENCY8;
						update_frequency(current.frequency);
						break;
			
			case '9':	Serial.println("Selecting operation in the 915MHz band");
						current.frequency=DEFAULT_FREQUENCY9;
						current.frequency_band=9;
						min_frequency=MIN_FREQUENCY9;
						max_frequency=MAX_FREQUENCY9;
						default_frequency=DEFAULT_FREQUENCY9;
						update_frequency(current.frequency);
						break;
			
			// power adjustment commands
			
			case 'p':	if(current.power_level>MIN_POWER_LEVEL)
						{
							current.power_level--;
							update_power_level(current.power_level);
						}
						
						break;
						
			case 'P':	if(current.power_level<MAX_POWER_LEVEL)
						{
							current.power_level++;
							update_power_level(current.power_level);
						}
						
						break;
			
			case 'o':	current.power_level=DEFAULT_POWER_LEVEL;
						update_power_level(current.power_level);
						
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

#if 0			
			case 'r':
			case 'R':	configmode=false;
						Serial.println("Entering run mode");
						displayon=true;
						displayonuntil=millis()+30000;
						break;
#endif

			case '.':	Serial.print("Turning the display on for 30s\r\n");
						displayon=true;
						displayonuntil=millis()+30000;
						break;
			
			case 'a':
			case 'b':
			case 'c':	ReadStoredFrequencyPreset(byte-'a'+1);
						break;
			
			case 'A':
			case 'B':
			case 'C':	SaveStoredFrequencyPreset(byte-'A'+1);
						break;
			
			case 'm':
			case 'M':	displaymenu();
						break;
			
			case 'x':	// hexdump of the eeprom for debugging
						
						{
							int cnt;
							for(cnt=0;cnt<32;cnt++)
							{
								if(cnt==16)	Serial.print("\r\n");
								
								Serial.printf("%02x ",EEPROM.read(cnt));
							}
							
							Serial.print("\r\n");
						}
						
						break;
			
			case 'X':	Serial.print("Resetting to default settings\r\n");
						set_default_values();
						set_frequency_band_edges();
						SaveStoredFrequencyPreset(0);	
						break;
						
			default:	// do nowt
						break;
		}
	}
}

// 0 is the defaults, 1, 2 and 3 correspond to saved presets

void SaveStoredFrequencyPreset(int preset)
{
	EEPROM.writeFloat(EEPROM_ADDRESS+sizeof(channel_settings)*preset+0,current.frequency);
	EEPROM.writeByte(EEPROM_ADDRESS+sizeof(channel_settings)*preset+4,current.power_level);
	EEPROM.writeByte(EEPROM_ADDRESS+sizeof(channel_settings)*preset+5,current.tx_period);
	EEPROM.writeByte(EEPROM_ADDRESS+sizeof(channel_settings)*preset+6,current.use_extended_life_mode);
	EEPROM.writeByte(EEPROM_ADDRESS+sizeof(channel_settings)*preset+7,current.frequency_band);
	
	EEPROM.commit();
	
#if 0
	char buffer[8];
	EEPROM.readBytes(EEPROM_ADDRESS+sizeof(channel_settings)*preset,buffer,sizeof(channel_settings));
	Serial.printf("EEPROM Read: %02x %02x %02x %02x %02x %02x %02x %02x\r\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7]);
#endif
	
	if(preset!=0)
		Serial.printf("Stored frequency of %03.3f MHz to Preset %c\r\n",current.frequency,'A'+preset-1);
}

// 0 is the defaults, 1, 2 and 3 correspond to saved presets

void ReadStoredFrequencyPreset(int preset)
{
	channel_settings temp;
	
	temp.frequency=EEPROM.readFloat(EEPROM_ADDRESS+sizeof(channel_settings)*preset+0);
	temp.power_level=EEPROM.readByte(EEPROM_ADDRESS+sizeof(channel_settings)*preset+4);
	temp.tx_period=EEPROM.readByte(EEPROM_ADDRESS+sizeof(channel_settings)*preset+5);
	temp.use_extended_life_mode=EEPROM.readByte(EEPROM_ADDRESS+sizeof(channel_settings)*preset+6);
	temp.frequency_band=EEPROM.readByte(EEPROM_ADDRESS+sizeof(channel_settings)*preset+7);
	
#if 0
	char buffer[8];
	EEPROM.readBytes(EEPROM_ADDRESS+sizeof(channel_settings)*preset,buffer,sizeof(channel_settings));
	Serial.printf("EEPROM Read: %02x %02x %02x %02x %02x %02x %02x %02x\r\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7]);
#endif
	
	if(		isnan(temp.frequency)
		||	(temp.frequency<MIN_FREQUENCY4)
		||	((temp.frequency>MAX_FREQUENCY4)&&(temp.frequency<MIN_FREQUENCY8))
		||	((temp.frequency>MAX_FREQUENCY8)&&(temp.frequency<MIN_FREQUENCY9))
		||	(temp.frequency>MAX_FREQUENCY9)
		||	(temp.power_level<MIN_POWER_LEVEL)
		||	(temp.power_level>MAX_POWER_LEVEL)
		||	(temp.tx_period<MIN_CADENCE)
		||	(temp.tx_period>MAX_CADENCE)
		||	((temp.frequency_band!=4)&&(temp.frequency_band!=8)&&(temp.frequency_band!=9))	)
	{
		Serial.println("Invalid stored settings detected, subsituting the defaults");
		set_default_values();
	}
	else
	{
		memcpy((void *)&current,(void *)&temp,sizeof(channel_settings));
		
		if(preset!=0)
			Serial.printf("Recalled frequency of %03.3f MHz from Preset %c\r\n",current.frequency,'A'+preset-1);
	}
	
	set_frequency_band_edges();
	SaveStoredFrequencyPreset(0);	
}

void set_default_values(void)
{
	current.frequency=DEFAULT_FREQUENCY4;
	current.power_level=DEFAULT_POWER_LEVEL;
	current.tx_period=DEFAULT_CADENCE;
	current.use_extended_life_mode=DEFAULT_EXTENDED_LIFE_MODE;
	current.frequency_band=FREQUENCY_BAND_4;
}

void set_frequency_band_edges(void)
{
	if(current.frequency_band==4)		{	min_frequency=MIN_FREQUENCY4;	max_frequency=MAX_FREQUENCY4;	default_frequency=DEFAULT_FREQUENCY4;	}
	else if(current.frequency_band==8)	{	min_frequency=MIN_FREQUENCY8;	max_frequency=MAX_FREQUENCY8;	default_frequency=DEFAULT_FREQUENCY8;	}
	else if(current.frequency_band==9)	{	min_frequency=MIN_FREQUENCY9;	max_frequency=MAX_FREQUENCY9;	default_frequency=DEFAULT_FREQUENCY9;	}
	else
	{
		Serial.print("Corrupt setting detected, resetting to defaults\r\n");
		set_default_values();
	}
}
