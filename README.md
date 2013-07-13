HT1632C Web interface for Arduino
=================================

This is a web interface for the powerful HT1632C bicolor(RG) led matrix library made by gauravmm and forked by flavio-fernandes. (cf http://www.sureelectronics.net/goods.php?id=1095)

Prerequisites
=============

- Arduino Mega 2560
- Ethernet Shield
- 1 or 2 HT1632 RG Bicolor Led Matrix boards (might support up to 4 boards)

and

- HT1632C library forked by flavio-fernandes (https://github.com/flavio-fernandes/HT1632-for-Arduino)
- webduino library

Installation
============

Follow the instructions to edit the ht1632.h file in the HT1632 library. This is needed so it can find out it is a bicolor board and the number of boards as well.

Open h1632c-arduino-web-ui.ino and edit the arduino ip address

Upload to your Arduino

Open your browser and enter the address http://{your-arduino-ip-addres}/index.html

Explanation
-----------

This web interface opens a http server on the arduino. Then you can access it from your browser and change the text on the display. It allows 2 lines editing of 1 to 4 boards and have blinking and scrolling options.

Bugs & Features
===============

Looks like both the HT1632 and ethernet library need to much SRAM memory so it can run on the Arduino Uno

Known Issues
------------

1. Using both blinking and scrolling on the two lines at the same makes the animation a little bit slower the more boards you have.

Future Plans
------------

- Add variables such as date, time, count and countdown minutes
- Automatic Switch on and switch off with default messages on display.
