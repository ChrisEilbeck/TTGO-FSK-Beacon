#!/bin/bash

~/arduino-cli/arduino-cli upload \
 	--port /dev/ttyACM0 \
	--verbose \
	--fqbn esp32:esp32:ttgo-lora32-v1

