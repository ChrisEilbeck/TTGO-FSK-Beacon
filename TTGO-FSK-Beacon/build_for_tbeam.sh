#!/bin/bash

VERBOSE=--verbose

##	--fqbn esp32:esp32:ttgo-lora32-v21new \

arduino-cli compile \
	--fqbn esp32:esp32:t-beam \
	${VERBOSE} \
	TTGO-FSK-Beacon.ino
