#!/bin/bash

VERBOSE=--verbose
BOARD=esp32:esp32:ttgo-lora32:Revision=TTGO_LoRa32_v21new

arduino-cli compile \
	--fqbn ${BOARD} \
	${VERBOSE} \
	TTGO-FSK-Beacon.ino

