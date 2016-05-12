# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile

ARDUINO_DIR = /usr/local/share/arduino-1.6.7

ARCHITECTURE  = avr
BOARD_TAG    = nano
BOARD_SUB    = atmega328
ISP_PORT = /dev/ttyUSB1
MONITOR_PORT = /dev/ttyUSB0
MONITOR_CMD = miniterm.py

ARDMK_DIR=/home/tomc/ownCloud/Electronics/Arduino/Arduino-Makefile

CXXFLAGS = -Wno-deprecated-declarations -g3

MONITOR_BAUDRATE = 57600

ARDUINO_LIBS = \
	       Streaming \
	       Task \
		   IRremote \
		   EEPROM \
		   Servo \
		   PCD8544_SPI \
		   MemoryFree \
		   SPI

-include $(ARDMK_DIR)/Arduino.mk

CFLAGS_STD = -Wall -Wundef -Werror
CXXFLAGS_STD = -Wall -Werror
# !!! Important. You have to use make ispload to upload when using ISP programmer

