#!/bin/bash

VERBOSE=--verbose

arduino-cli compile \
	--fqbn esp32:esp32:t-beam \
	${VERBOSE} \
	TTGO-FSK-Beacon.ino

