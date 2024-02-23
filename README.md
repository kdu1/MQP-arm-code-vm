# MQP-arm-code-vm

Code for arms for social robot MQP. 
All the important code so far is in the catkin_mqp folder in the src/arm_code/src folder. 
The main file is Robot.cpp, with functions for writing to the hid device using hidapi in SimpleComsDevice.cpp. There is a makefile under src. The executable is run with ./arm_code

### install:
- sudo apt-get install libusb-dev
- sudo apt-get install libusb-1.0-0

### used the include statement:
- include <hidapi/hidapi.h>

### udev rules:
- if testing on computer, need to edit udev rules to grant permission to access device through hid. Create a new .rules file under the /etc/udev/rules.d directory. Inside, put:
- KERNEL=="hidraw*", ATTRS{busnum}=="1", ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="0486", MODE="0666"
- where idVendor and idProduct are the vid and pid

## No longer using ros. Ros instructions below:
use rosrun arm_code arm_code for now

### On hidapi:
May have to clone hidapi repository under src/arm_code
Must add "hidapi-libusb" to target_link_libraries statement in CMakeLists.txt:
