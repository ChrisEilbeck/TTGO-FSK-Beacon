#!/bin/bash

VERBOSE=--verbose

arduino-cli compile \
	--fqbn esp32:esp32:ttgo-lora32-v21new \
	${VERBOSE} \
	TTGO-FSK-Beacon.ino

