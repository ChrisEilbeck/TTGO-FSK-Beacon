
#pragma once

// pin assignments for various boards we support

#ifdef ARDUINO_TBeam
	#define LoRa_NSS				18
	#define LoRa_DIO0				26
	#define LoRa_RESET				14
	#define LoRa_DIO1				-1
	
	#define BATTERY_VOLTAGE_PIN		-1
	#define BATTERY_CAL_VALUE		1.734
	
	#define USER_BUTTON				-1
	#define USER_LED				25
#endif
#ifdef ARDUINO_TTGO_LoRa32_v21new
	#define LoRa_NSS				18
	#define LoRa_DIO0				26
	#define LoRa_RESET				23
	#define LoRa_DIO1				-1
	
	#define BATTERY_VOLTAGE_PIN		35
	#define BATTERY_CAL_VALUE		1.734
	
	#define USER_BUTTON				-1
	#define USER_LED				25
#endif
#ifdef ARDUINO_TTGO_LoRa32_V1
	#define LoRa_NSS				5
	#define LoRa_DIO0				26
	#define LoRa_RESET				4
	#define LoRa_DIO1				-1
	
	#define BATTERY_VOLTAGE_PIN		35
	#define BATTERY_CAL_VALUE		1.734
	
	#define USER_BUTTON				-1
	#define USER_LED				25
#endif
#ifdef ADAFRUIT_FEATHER_M0
	#define LoRa_NSS				8
	#define LoRa_DIO0				3
	#define LoRa_RESET				4
	#define LoRa_DIO1				-1
	
	#define BATTERY_VOLTAGE_PIN		-1
	#define BATTERY_CAL_VALUE		1.734
	
	#define USER_BUTTON				-1
	#define USER_LED				25
#endif

