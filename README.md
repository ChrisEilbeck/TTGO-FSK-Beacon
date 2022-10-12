# TTGO-FSK-Beacon
Simple application to use a TTGO Paxcounter to act a a beacon to be received with an FM scanner

If using arduino-cli, just use the two included scripts.  If you're using
the arduino application, make sure you select the board "TTGO LoRa32-OLED
v2.1.6" or whatever matches your hardware.  

This will not work with a T-Beam as yet because it doesn't support the PMIC
used on that board and the power to the LoRa module is off by default.

You will need to install the RadioLib library too.

