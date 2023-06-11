#!/bin/bash

##VERBOSE=--verbose
PORT=/dev/ttyUSB*
BOARD=esp32:esp32:ttgo-lora32:Revision=TTGO_LoRa32_v21new
ESPTOOL=/home/chris/.arduino15/packages/esp32/tools/esptool_py/4.5.1/esptool.py

arduino-cli upload \
 	--port ${PORT} \
	${VERBOSE} \
	--fqbn ${BOARD}

##exit

rm data/*~ 2>/dev/null

mkspiffs -c data -b 4096 -p 256 -s 0x160000 ttgo-fsk-beacon.spiffs.bin

echo Created spiffs image, uploading ...

python ${ESPTOOL} \
	--chip esp32 \
	--port ${PORT} \
	--baud 921600 \
	write_flash -z 0x290000 ttgo-fsk-beacon.spiffs.bin


