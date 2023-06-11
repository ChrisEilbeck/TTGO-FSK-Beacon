#!/bin/bash

VERBOSE=--verbose
BOARD=esp32:esp32:tbeam

arduino-cli compile \
	--fqbn ${BOARD} \
	${VERBOSE} \
	TTGO-FSK-Beacon.ino
