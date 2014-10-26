FoamBot
=======

This is a project to build a simple bot using [Foam Board][1] as the
construction basis.

Features
--------
Two continuous rotation servos are used for the drive train and for now an
Arduino Uno is used as the controller.

Power for the servos is from 4 x AA batteries while the Arduino is powered from
a LiPo battery via [LiPo Rider Pro][2].

Bot control at the moment consists of:
* Remote control using an IR receiver and cheap IR remote control from a car FM
  audio transmitter.
* Line follower using two TCTR5000 IR sensors - still needs some tweaking.

Software - TODO: details to be added, but see the code...

Current status
--------------
At the moment it is difficult to tweak the line following functionality since
the bot needs to be tethered to the laptop via USB cable to see the see the
sensor readings and current movement state. In order to make debugging this
easier an LCD display is being added.

The LCD being used is one of the Nokia 5110 / PCD8544 displays easily found all
over. There are some variations in these modules with different pin outs although
the modules looks very similar. More details to follow.




[1]: http://en.wikipedia.org/wiki/Foamcore
[2]: http://www.seeedstudio.com/wiki/index.php?title=Lipo_Rider_Pro
