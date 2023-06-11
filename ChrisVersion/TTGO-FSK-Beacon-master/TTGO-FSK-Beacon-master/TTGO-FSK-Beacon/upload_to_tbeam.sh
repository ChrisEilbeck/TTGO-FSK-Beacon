#!/bin/bash

~/arduino-cli/arduino-cli upload \
 	--port /dev/ttyUSB0 \
	--verbose \
	--fqbn esp32:esp32:t-beam \

