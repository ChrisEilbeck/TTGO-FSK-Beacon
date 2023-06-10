
#pragma once

// pin assignments for various boards we support

#ifdef ARDUINO_TBEAM_USE_RADIO_SX1276
	#define BATTERY_VOLTAGE_PIN		-1
	#define BATTERY_CAL_VALUE		1.734
	#define USER_BUTTON				-1
	#define LORA_D1					-1
	#define ARDUINO_TBeam			1
	#define SUPPORT_WEBSERVER 		0
	
	#warning "Building for TBeam SX1276"
#else
#ifdef ARDUINO_TBEAM_USE_RADIO_SX1278
	#define BATTERY_VOLTAGE_PIN		-1
	#define BATTERY_CAL_VALUE		1.734
	#define USER_BUTTON				-1
	#define LORA_D1					-1
	#define ARDUINO_TBeam
	#define SUPPORT_WEBSERVER 		0
	
	#warning "Building for TBeam SX1278"
#else
#ifdef ARDUINO_TTGO_LoRa32_v21new
	#define BATTERY_VOLTAGE_PIN		35
	#define BATTERY_CAL_VALUE		1.734
	#define USER_BUTTON				-1
	#define LORA_D1					-1
	#define SUPPORT_WEBSERVER 		1
	
	#warning "Building for v21new
#else
#ifdef ARDUINO_TTGO_LoRa32_V1
	#define BATTERY_VOLTAGE_PIN		35
	#define BATTERY_CAL_VALUE		1.734
	#define USER_BUTTON				-1
	#define LORA_D1					-1
	#define SUPPORT_WEBSERVER 		0
	
	#error "Building for v1"
#else
#ifdef ARDUINO_TTGO_LoRa32_V2
	#define BATTERY_VOLTAGE_PIN		35
	#define BATTERY_CAL_VALUE		1.734
	#define USER_BUTTON				-1
	#define LORA_D1					-1
	#define SUPPORT_WEBSERVER 		0
	
	#warning "Building for v2"
#else
#ifdef ADAFRUIT_FEATHER_M0
	#define LoRa_NSS				8
	#define LoRa_DIO0				3
	#define LoRa_RESET				4
	#define LoRa_DIO1				-1
	
	#define BATTERY_VOLTAGE_PIN		-1
	#define BATTERY_CAL_VALUE		1.734	// probably wrong
	
	#define USER_BUTTON				-1

	#define SUPPORT_WEBSERVER 		0
	
	#warning "Building for Adafruit Feather M0"
#else
	#error "Building for something else ..."
#endif
#endif
#endif
#endif
#endif
#endif
