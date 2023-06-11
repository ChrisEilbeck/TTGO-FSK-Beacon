# TTGO-FSK-Beacon
Simple application to use a TTGO Paxcounter to act a a beacon to be 
received with an FM scanner

If using arduino-cli, just use the included scripts.  If you're using
the arduino application, make sure you select the board "TTGO LoRa32-OLED
v2.1.6" or whatever matches your hardware.  It should pick up the correct 
pins for the LoRa module, display etc. through the arduino board variants 
settings.  I'm using the above listed board so that's the one that has 
been most tested with this.

The latest version as of 13/10/2022 also supports the TTGO T-Beam v1.1 
a.k.a. the meshtastic board.  This uses the axp192 library to control 
the PMIC on this board.

You will need to install the RadioLib library too.

# Instructions for Use

Program the firmware into the board, attach an antenna and a lipo battery 
and turn it on.  It will default to operation at 433.920MHz.  On the 
display you should see a battery voltage readout and the operating 
frequency.  This should stay on the screen for about 30s then the unit 
will drop into low power mode, only waking up periodically to transmit 
then going back to sleep.

# Instructions for Config Mode

Plugging the unit into a laptop via USB serves two purposes

-	It will recharge the lipo, and
-	It will present a test harness over the serial port which can be 
	used to configure the unit.

When plugging in with the unit turned on, only charging will happen.  
Reboot the unit to get into the config mode using the reset button.  
A menu is then presented to allow the user to change frequency etc.

The menu allows the user to set the frequency, read voltage, do a test 
transmission etc.  

A new feature is the ability to store three preset channels using the 
A, B or C commands to store the currently configured frequency and 
use a, b or c to recall one of these presets from the non-volatile 
memory store.

0 can be used to recall the default frequency for the configured band.  
4, 8 or 9 can be used to configure a unit to operate in the 430-440MHz 
band, 868-870MHz band or 902-930MHz band respectively.

