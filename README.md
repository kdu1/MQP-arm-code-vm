# MQP-arm-code-vm

Code for arms for social robot MQP. 
All the important code so far is in the catkin_mqp folder in the src/arm_code/src folder. 
The main file is Robot.cpp, with functions for writing to the hid device using hidapi in SimpleComsDevice.cpp. Can ignore all gazebo stuff.

## To run:
use rosrun arm_code arm_code for now

## On hidapi:
May have to clone hidapi repository under src/arm_code
Must add "hidapi-libusb" to target_link_libraries statement in CMakeLists.txt:

### install:
- sudo apt-get install libusb-dev
- sudo apt-get install libusb-1.0-0

### used the include statements:
- include <hidapi.h>
- include <hidapi/hidapi.h>
- include <libusb.h>
- include <hidapi_libusb.h>
