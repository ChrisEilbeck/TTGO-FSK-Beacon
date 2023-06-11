
#pragma once

typedef struct
{
	// frequency of transmission in Hz
	float frequency;
	
	// transmit power in dBm
	byte power_level;
	
	// period between transmit bursts in steps of 50ms
	byte tx_period;
	
	// flag to switch into a longer period between transmissions to give extended life
	byte use_extended_life_mode;
	
	// operating band, 430-440MHz, 868-870MHz or 902-930MHz, represented as 4, 8 or 9
	byte frequency_band;
} channel_settings;

#define MS_PER_TICK 50

