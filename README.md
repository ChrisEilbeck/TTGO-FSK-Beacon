# TTGO-FSK-Beacon
Simple application to use a TTGO Paxcounter to act a a beacon to be 
received with an FM scanner

If using arduino-cli, just use the included scripts.  If you're using
the arduino application, make sure you select the board "TTGO LoRa32-OLED
v2.1.6" or whatever matches your hardware.

The latest version as of 13/10/2022 also supports the TTGO T-Beam v1.1 
a.k.a. the meshtastic board.  This uses the axp192 library to control 
the PMIC on this board.

ARDUINO_TTGO_LoRa32_V1 (Lilygo T95_v1.1 Support addded)

25/10/22
Latest updates from CE included now stores freq in eeprom correctly

You will need to install the RadioLib library too.

