# MQP-arm-code-vm

Code for arms for social robot MQP. 
All the important code so far is in the catkin_mqp folder in the src/arm_code/src folder. 
ROSWrapper.cpp is a ROS wrapper that allows the code to interact with the personality node shown_personality.
The main file with the functions that actually move the arm and include interpolation and inverse kinematics is Robot.cpp, with functions for writing to the hid device using hidapi in SimpleComsDevice.cpp. 

##ROS and integraton
- ROSWrapper.cpp contains a ROS node, which publishes to and is subscribed to the shown_personality node from the personality. The subscriber recieves a string message for an emotion, which calls a callback function which calls the method corresponding to the emotion. These methods call servo_jp with the corresponding coordinates for the pose corresponding to the emotion. The input for servo_jp is an array of 3 Complex float values (conversion is done in the code from float to Complex). These represent the respective angle values for each of the servos. 

##Arm movement and Joint vs Task space.
- Currently, calling servo_jp is what will move the arm. This moves the arm in joint space. For task space, the angles can be inputted into the ik3001() inverse kinematics method then inputted into servo_jp. There exists a lot of code for interpolation that is currently unused since it is based off of end effector position. PickAndPlace() is a method that takes in end effector position and calculates the trajectory to get there. Most of it is translated from a MATLAB RBE3001 lab and should be usable.  

##Running code
use rosrun arm_code arm_code for now

### install:
- don't necessarily need these as using hidraw instead of libusb, but it could help if errors are appearing for no reason
- sudo apt-get install libusb-dev
- sudo apt-get install libusb-1.0-0

### On hidapi:
Clone hidapi repository under src/arm_code. Using hidapi hidraw, as apposed to hidapi libusb.

#### used the include statement:
- include <hidapi/hidapi.h>

#### cmakelists:
- include hidapi/hidapi directory in include_directories
- use add_library to create a hidapi_hidraw library and include the path to hidapi/linux/hid.c
- add hidapi_hidraw or whatever you named the library to target_link_libraries
- also add udev to target_link_libraries

### udev rules:
- if testing on computer, need to edit udev rules to grant permission to access device through hid. Create a new .rules file under the /etc/udev/rules.d directory. Inside, put:
- KERNEL=="hidraw*", ATTRS{busnum}=="1", ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="0486", MODE="0666"
- where idVendor and idProduct are the vid and pid

