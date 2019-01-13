# Laser-target-game
Combine binary counting and laser pointer skills while checking progress on a web page spawned by an local Wi-Fi hotspot.. nice for CTF events :+1:
This was created in order to have some fun at a CTF event. 

It involves some Laser Detector diodes, wire, and a ESP-12E.. which is nicely packaged in a Lolin NodeMCU V3 board (currently shipped free of charge to Europe for 1,94 euro.. wow). 
To provide output to the users there is also an OLED involved.

The diodes are mounted on an electrical pipe of about one meter. 
Distance between audiance and targets may vary upon taste but 7 meters is quite a challenge :-)


# Usage:
First logon to the Wi-Fi and browse to the routers address.
![logon](https://github.com/plando2act/Laser-target-game/blob/master/1.PNG)

And then browse towards the web page to start reading about the things to do:
![instructions](https://github.com/plando2act/Laser-target-game/blob/master/2.PNG)

The sequence is changable in the code. Once the sequence is done, the flag is shown on the OLED display.

# Hardware:
The first start-up screen briefly shows the SSID and password to help logon.
The OLED display is an I2C version 0.96 inch and widely available. If you tinker around with this one, please note that the display code will have to change as well.

![Close-up](https://github.com/plando2act/Laser-target-game/blob/master/3.JPG)
Yes.. we made two.. for team A and team B.

![instructions](https://github.com/plando2act/Laser-target-game/blob/master/4.JPG)

Finally the display will show actual readings of the laser bits and updates in the stages.
![instructions](https://github.com/plando2act/Laser-target-game/blob/master/5.JPG)

Have fun!
