#!/bin/bash

VERBOSE=--verbose

arduino-cli compile \
	--fqbn adafruit:samd:adafruit_feather_m0 \
	${VERBOSE} \
	TTGO-FSK-Beacon.ino

