#!/bin/bash

~/arduino-cli/arduino-cli upload \
 	--port /dev/ttyACM1 \
	--verbose \
	--fqbn esp32:esp32:ttgo-lora32-v21new

