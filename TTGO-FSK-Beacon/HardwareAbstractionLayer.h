
#pragma once

// pin assignments for various boards we support

#ifdef ARDUINO_TBeam
	#define BATTERY_VOLTAGE_PIN		-1
	#define BATTERY_CAL_VALUE		1.734
	#define USER_BUTTON				-1
	#warning "Building for TBeam"
#else
#ifdef ARDUINO_TTGO_LoRa32_v21new
	#define BATTERY_VOLTAGE_PIN		35
	#define BATTERY_CAL_VALUE		1.734
	#define USER_BUTTON				-1
	#warning "Building for v21new
#else
#ifdef ARDUINO_TTGO_LoRa32_V1
	#define BATTERY_VOLTAGE_PIN		35
	#define BATTERY_CAL_VALUE		1.734
	#define USER_BUTTON				-1
	#error "Building for v1"
#else
#ifdef ARDUINO_TTGO_LoRa32_V2
	#define BATTERY_VOLTAGE_PIN		35
	#define BATTERY_CAL_VALUE		1.734
	#define USER_BUTTON				-1
	#define LORA_D1					-1
	#warning "Building for v2"
12#else
#ifdef ADAFRUIT_FEATHER_M0
	#define LoRa_NSS				8
	#define LoRa_DIO0				3
	#define LoRa_RESET				4
	#define LoRa_DIO1				-1
	
	#define BATTERY_VOLTAGE_PIN		-1
	#define BATTERY_CAL_VALUE		1.734
	
	#define USER_BUTTON				-1
	#warning "Building for Adafruit Feather M0"
#else
	#warning "Building for something else ..."
#endif
#endif
#endif
#endif
#endif
