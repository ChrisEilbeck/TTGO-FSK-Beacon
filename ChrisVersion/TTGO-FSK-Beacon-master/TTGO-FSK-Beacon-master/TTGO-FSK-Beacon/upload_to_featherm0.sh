#!/bin/bash

~/arduino-cli/arduino-cli upload \
 	--port /dev/ttyACM0 \
	--verbose \
	--fqbn adafruit:samd:adafruit_feather_m0

