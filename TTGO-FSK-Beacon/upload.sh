#!/bin/bash

VERBOSE=--verbose
PORT=/dev/ttyUSB*
BOARD=esp32:esp32:ttgo-lora32:Revision=TTGO_LoRa32_v21new

arduino-cli upload \
 	--port ${PORT} \
	${VERBOSE} \
	--fqbn ${BOARD}

