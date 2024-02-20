

#include <vector>
#include <unordered_map>
//#include "PacketType.h"
//#include "FloatPacketType.h"
#include "SimpleComsDevice.h"
//#include "Runnable.h"

#include <iostream>
#include <memory>
#include <cassert>
//#include <windows.h>
//#include <process.h>
//#include <hidapi.h>
//#include <hidapi/hidapi.h>
//#include <libusb.h>
//#include <hidapi_libusb.h>

#include <memory>
#include <thread>
#include <complex>
#include <valarray>

#define _USE_MATH_DEFINES
# define M_PI           3.14159265358979323846  /* pi */


//struct hid_device_;
//typedef struct hid_device_ hid_device; /**< opaque hidapi structure */

typedef std::complex<float> Complex;
typedef std::valarray<Complex> CArray;


//definitions
 bool connected = false;
 bool virtualv = false;
 int readTimeout = 100;
 bool isTimedOut = false;
    

//going to open it in here instead of Robot
//device:
//from test.c
// Open the device using the VID, PID,
// and optionally the Serial number.
////handle = hid_open(0x4d8, 0x3f, L"12345");

/**
 * Constructor
*/
SimpleComsDevice::SimpleComsDevice(const char* path){
    ROS_INFO("scd constructor");
    SimpleComsDevice::handle = hid_open_path(path);
    hid_device_info* devinfo = hid_enumerate(0x239A, 0x802C);
    //print_device(devinfo);
    
    print_devices_with_descriptor(devinfo);
    hid_free_enumeration(devinfo);
}



//HIDPacketComs extends this, which calls read + write, which calls hidDevice.read+write, 
//which calls HidApi.read + write
//which calls hid_api.read + write
bool SimpleComsDevice::validHandle(hid_device * handle){
    ROS_INFO("in validHandle");
    if (!handle) {
        ROS_INFO("unable to open device\n");
        hid_exit();
        return 1;
    }
}

/**
 * print hid_device_info
 * @param cur_dev_ hid_device_info* struct
*/
void SimpleComsDevice::print_device(struct hid_device_info *cur_dev) {
	ROS_INFO("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
	ROS_INFO("\n");
	ROS_INFO("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
	ROS_INFO("  Product:      %ls\n", cur_dev->product_string);
	ROS_INFO("  Release:      %hx\n", cur_dev->release_number);
	ROS_INFO("  Interface:    %d\n",  cur_dev->interface_number);
	ROS_INFO("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
	//ROS_INFO("  Bus type: %u (%s)\n", (unsigned)cur_dev->bus_type, hid_bus_name(cur_dev->bus_type));
	ROS_INFO("\n");
}

/**
 * print hid_device_info
 * @param cur_dev_ hid_device_info* linkedlist
*/
void SimpleComsDevice::print_devices_with_descriptor(struct hid_device_info *cur_dev) {
	for (; cur_dev; cur_dev = cur_dev->next) {
        //print_hid_report_descriptor_from_device(SimpleComsDevice::handle);
		print_device(cur_dev);
		//print_hid_report_descriptor_from_path(cur_dev->path);
	}
}

/**
 * print hid_report_descriptor given path
 * @param path const char* path to open device
*/
/*void SimpleComsDevice::print_hid_report_descriptor_from_path(const char *path) {
	hid_device *device = hid_open_path(path);
	if (device) {
		print_hid_report_descriptor_from_device(device);
		hid_close(device);
	}
	else {
		printf("  Report Descriptor: Unable to open device by path\n");
	}
}*/

/**
 *  print hid_report_descriptor given hid_device
 * @param device hid_device* device
*/
/*void SimpleComsDevice::print_hid_report_descriptor_from_device(hid_device *device) {
	unsigned char descriptor[4096];
	int res = 0;

	printf("  Report Descriptor: ");
	res = hid_get_report_descriptor(device, descriptor, sizeof(descriptor));
	if (res < 0) {
		printf("error getting: %ls", hid_error(device));
	}
	else {
		printf("(%d bytes)", res);
	}
	for (int i = 0; i < res; i++) {
		if (i % 10 == 0) {
			printf("\n");
		}
		printf("0x%02x, ", descriptor[i]);
	}
	printf("\n");
}*/

 std::unordered_map<int, std::vector<SimpleComsDevice::Runnable>> events;
    
 //std::vector<FloatPacketType> pollingQueue;

 /*void SimpleComsDevice::addPollingPacket(FloatPacketType packet) {
        if (!(getPacket((int)packet.idOfCommand) == nullptr)){
            throw("Only one packet of a given ID is allowed to poll. Add an event to recive data"); 
        }
        SimpleComsDevice::pollingQueue.push_back(packet);
    }*/
    
/**
 * given id
 * return FloatPacketType pointer corresponding to it
*/
 FloatPacketType* SimpleComsDevice::getPacket(int ID) {
    ROS_INFO("getPacket");
    for (FloatPacketType q : SimpleComsDevice::getPollingQueue()) {
        if (q.idOfCommand == ID){
            ROS_INFO("id exists");
            return &q; 
        }
    } 
    return nullptr;
}


void SimpleComsDevice::writeFloats(int id, std::vector<Complex> values) {
        writeFloats(id, values, true);
    }
    
void SimpleComsDevice::writeFloats(int id, std::vector<Complex> values, bool polling) {
    float* floatvalues = reinterpret_cast<float(&)[2]>(values);
    if (getPacket(id) == NULL) {
        FloatPacketType pt = FloatPacketType(id, 64);
    //if (!polling)//TODO: it's not gonna go here
        //pt.oneShotMode(); 
    for (int i = 0; i < (pt.getDownstream()).size() && i < values.size(); i++){
        pt.getDownstream()[i] = floatvalues[i]; 
    }
    addPollingPacket(pt);
    try {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } catch (const std::exception& e) {
        printf("writeFloats thread sleep exception: ");
        printf(e.what());
    } 
    } else {
        std::vector<FloatPacketType, std::allocator<FloatPacketType>> pollingQueue = SimpleComsDevice::getPollingQueue();
        for (int j = 0; j < pollingQueue.size(); j++) {
            FloatPacketType pt = pollingQueue[j];
            if (/*FloatPacketType.class.isInstance(pt) && */pt.idOfCommand == id) {
                for (int i = 0; i < (pt.getDownstream()).size() && i < values.size(); i++){
                    pt.getDownstream()[i] = floatvalues[i]; 
                }
                /*if (!polling){
                    pt.oneShotMode(); 
                }*/
                return;
            } 
        }; 
    } 
}


/**
 * reads data given id
 * @param id reportid
 * @return vector of doubles containing read information
 * gets the information given the id from the pollingPacket, then returns it
*/
 std::vector<float> SimpleComsDevice::readFloats(int id) {
    ROS_INFO("readFloats");
    if (getPacket(id) == NULL) {//packets are assigned an id related to the purpose, simplepacketcoms manages them
        ROS_INFO("id null");
        FloatPacketType fl = FloatPacketType(id, 64);
        addPollingPacket(fl);
        try {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } catch (const std::exception& e) {
            printf("readFloats thread sleep exception: ");
            printf(e.what());
        } 
    } 

    ROS_INFO("readFloats");
    FloatPacketType* pt = getPacket(id);
    ROS_INFO("before getUpstream stuff");
    if(pt == nullptr){
        ROS_INFO("null");
        //I actually don't know what to do in this case
    }
    std::vector<float> values((pt->getUpstream()).size(), 0);
    for (int i = 0; i < (pt->getUpstream()).size() && i < values.size(); i++){
        values[i] = (float) pt->getUpstream()[i]; 
    }
    return values;
}


/**
 * TODO: not sure if this is correct for getting the id from the bytebuffer thing
*/
 int SimpleComsDevice::getId(std::vector<unsigned char> bytes) {
    //ByteBuffer 
    /*unsigned char* bAry = calloc(1024,1); //remember to free

    memcpy(bAry, &length, sizeof(int));    
    memcpy(bAry + sizeof(int), fAry, length*(sizeof(float)));*/

    //looks fancy but I'm guessing I can just do bytes[0]
    //return ByteBuffer.wrap(bytes).order(be).getInt(0); //ByteBuffer given array bytes, 
        //with byte order little endian
        //returns int from buffer's current position
    
    return bytes[0]; //ittle endian reminder
}

/**
 * The function where actually calls write and does the process of using the data to move the arm
 * @param packet a packet that is being processed
*/
 void SimpleComsDevice::process(FloatPacketType packet){
    packet.started = true;
    try {
        if (!isVirtual()) {
            try {
                std::vector<unsigned char> message = packet.command(packet.idOfCommand, packet.getDownstream());
                int val = write(message, message.size(), 1); //call to function that will call hid_write
                if (val > 0) {
                    //gets result after write
                    int readint = read(message, getReadTimeout()); //calls hid_read
                    //if read result greater than or equal to packet upstream size, do some timeout resolving packet managing
                    if (readint >= (packet.getUpstream()).size()) {
                        int ID = getId(message); //static method
                        if (ID == packet.idOfCommand) {
                            if (isTimedOut){
                                printf("Timeout resolved {}", ID); 
                            }
                            isTimedOut = false;
                            std::vector<double> up = packet.parse(message);
                            for (int i = 0; i < (packet.getUpstream()).size(); i++){
                                packet.getUpstream()[i] = up[i];
                            }
                        } else {
                            for (int i = 0; i < 3; i++){
                                read(message, getReadTimeout()); 
                            }
                            printf(" ");
                            isTimedOut = true;
                            return;
                        } 
                    } else {
                        isTimedOut = true;
                        return;
                    } 
                } else {
                    return;
                } 
            } catch (const std::exception& e) {
                //t.printStackTrace(System.out);
                printf("exception: in process under not isvirtual ");
                printf(e.what());
                disconnect();
            } 
    } else {
        for (int j = 0; j < (packet.getDownstream()).size() && j < (packet.getUpstream()).size(); j++){
            packet.getUpstream()[j] = packet.getDownstream()[j]; 
        }
    } 
    if (events.find(packet.idOfCommand) == events.end()){
        //Runnable ???
        //TODO not sure how to apply runnable correctly here
        //I mean do I even need this? not sure where it adds any events from
        if (events.find(packet.idOfCommand) != events.end()) {
            for (SimpleComsDevice::Runnable e : events[packet.idOfCommand]) {
                if (&e != nullptr) {
                    try {
                        //e(*this);//TODO: uh oh
                        e(*this);
                    } catch (const std::exception& e) {
                        std::cout << e.what() << std::endl;
                    }
                }
            }
        }

        /*
        for (SimpleComsDevice::Runnable e : this->events.at((int)packet.idOfCommand)) {
            if (&e != nullptr){
                try {
                    e(*this);
                } catch (const std::exception& e) {
                    printf("something went wrong inside process");
                    printf("exception: ");
                    printf(e.what());
                    //t.printStackTrace(System.out);
                }  
            }
        }  */
    }
} catch (const std::exception& e) {
    printf("exception: ");
    printf(e.what());
}
    packet.done = true;
}

 int SimpleComsDevice::getReadTimeout() {
    return readTimeout;
}

/**
 * Connects to device and begins to process packets and write to it
 * calls process which calls write
*/
 bool SimpleComsDevice::connect() {
    if (connectDeviceImp()) {
        setVirtual(false);
    } else {
        setVirtual(true);
    } 
    //this->connected = true;
    connected = true;
    
    //TODO: thread troubles may ensue
    std::thread([&]() {
        //while still connected to device, process everything in the pollingpacket which manages all the data being written and read
        while (connected) {
            try {
                std::vector<FloatPacketType, std::allocator<FloatPacketType>> pollingQueue = SimpleComsDevice::getPollingQueue();
                //process all packets in pollingQueue
                for (int i = 0; i < pollingQueue.size(); i++) {
                    FloatPacketType pollingPacket = pollingQueue[i];
                    if (pollingPacket.sendOk())
                        process(pollingPacket);
                }
            } catch (std::exception& e) {
                std::cout << e.what() << std::endl;
            }
            try {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            } catch (std::exception& e1) {
                std::cout << e1.what() << std::endl;
                connected = false;
            }
        }
        this->disconnectDeviceImp();
        std::cout << "SimplePacketComs disconnect" << std::endl;
    }).detach();
    //SimpleComsDevice::Runnable* t1 = new SimpleComsDevice::Runnable();
    //t1->start();
    //SimpleComsDevice::Runnable r(*this);// {
    /*while (SimpleComsDevice.this.connected) {
        try {
            for (int i = 0; i < SimpleComsDevice.this.SimpleComsDevice::pollingQueue.size(); i++) {
                FloatPacketType pollingPacket = SimpleComsDevice.this.SimpleComsDevice::pollingQueue.get(i);
                if (pollingPacket.sendOk())
                SimpleComsDevice.this.process(pollingPacket); 
            } 
        } catch (const std::exception& e) {
            printf("connect thread exception: ");
            printf(e.what());
        } 
        try {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } catch (const std::exception& e1) {
            printf("connect thread sleep exception: ");
            printf(e1.what());
            SimpleComsDevice.this.connected = false;
        } 
    } 

    SimpleComsDevice.this.disconnectDeviceImp();
    printf("SimplePacketComs disconnect");*/
    //}.start();
    return true;
}

/**
 * TODO: maybe not how it's supposed to be done but it's better to be safe
*/
 void SimpleComsDevice::disconnect() {
    hid_close(SimpleComsDevice::getHandle());
    hid_exit();
    connected = false;
}

 bool SimpleComsDevice::isVirtual() {
    return virtualv;
}

 void SimpleComsDevice::setVirtual(bool virtualv) {
    virtualv = virtualv;
}

 void SimpleComsDevice::setReadTimeout(int readTimeout) {
    readTimeout = readTimeout;
}

/*String getName() {
    return this->name;
}

void setName(String name) {
    this.name = name;
}*/

 bool SimpleComsDevice::getIsTimedOut() {
    return isTimedOut;
}

//int read(byte[] paramArrayOfbyte, int paramInt);

//int write(byte[] paramArrayOfbyte, int paramInt1, int paramInt2);

//k what does this do?
//does it actually do the disconnection
 bool SimpleComsDevice::disconnectDeviceImp(){
    //do this just in case
    hid_close(SimpleComsDevice::getHandle());
    hid_exit();
}

/**
 * TODO: why isn't this implemented
*/
bool SimpleComsDevice::connectDeviceImp(){
    return true;
}

/**
 * setConnected
 * @param connected value 
*/
void SimpleComsDevice::setConnected(bool set){
    connected = set;
}

/**
 * getConnected
 * @return connected value 
*/
bool SimpleComsDevice::getConnected(){
    return connected;
}



/**
 * setPollingQueue
 * @param pollingQueue
*/
/*
void SimpleComsDevice::setPollingQueue(std::vector<FloatPacketType> pollingQueue){
    pollingQueue = pollingQueue;
}*/


/**
 * write using hidapi
*/
int SimpleComsDevice::write(std::vector<unsigned char> packet, int len, unsigned char reportID){
    
    try{
        //data
            //java just has reportid being 0 cast to a byte
        //first byte is a thing
        //given input array with 3 values
        //
    /*  packet = zeros(15, 1, 'single'); % creates an empty 15x1 array to write to the robot
            packet(1) = 0; % bypasses interpolation
            packet(3) = array(1); % sets first motor's position value to the first value of array 
            packet(4) = array(2); % sets second motor's position value to the second value of array
            packet(5) = array(3); % sets third motor's position value to the third value of array */
        //size in matlab: self.write(1848, packet)
        if(!validHandle(SimpleComsDevice::getHandle())){
            ROS_INFO("unable to open device\n");
            hid_exit();
            throw ("unable to open device");
            return 1;
        }
        
        //put each packet value in double array
        //TODO: may need to change this to packet size anyway
        /*const int dslen = 3;//packet.size(); //we know it's 3 I'm not dealing with c++ rn
        double ds[dslen]; //now put this in buf somewhere
        for(int i = 0; i < dslen; i++){
            ds[i] = packet[i];
        }*/
        //option A: buf[1] = ds;
        //option B: buf[1] = packet[0];
        //          buf[2] = packet[1];
        //          buf[3] = packet[2];
        //based on the data type of buf it must be b..
        //I'm guessing it'd be index 1, 2, 3? 
        //I cannot be sure cause the matlab code is missing
    
        //data is buf
        /*const int length = 256; //I'd like to think it doesn't actually matter that much what the length is
        unsigned char buf[length + 1];
        //Complex buf[length + 1]; //problem is it needs to be unsigned char
        memset(buf,0,sizeof(buf));
        //in java they just put a byte 0
        buf[0] = reportID; //TODO:OKAY actually this is not reportID, it's a thing in the Packet structure in Java

        //Probably going to change this due to SimpleComsDevice stuff
        //set coordinates
        buf[1] = packet[0];
        buf[2] = packet[1];
        buf[3] = packet[2];*/

        // Precondition checks
        if (packet.size() < len) {
            len = packet.size();
        }

        //int lensend = 4; //TODO: hardcoded this as well, I think this is right

        unsigned char* bytesbuffer;

        bool dropReportIdZero = false;
        if (dropReportIdZero == 0 && reportID == 0) {
            // Use the alternative buffer representation that does
            // not include report ID 0x00
            // This overcomes "The parameter is incorrect" errors on
            // Windows 8 and 10
            // See the commentary on the dropReportIdZero flag for more info
            //report = new WideStringBuffer(len);
            /*if (len >= 1) {
                System.arraycopy(data, 0, report.buffer, 0, len);
            }*/
            bytesbuffer = &packet[0];
        } else {
            // Put report ID into position 0 and fill out buffer
            //report = new WideStringBuffer(len + 1);
            bytesbuffer[0] = reportID;
            if (len >= 1) {
                
                //copy packet into bytesbuffer at index 1
                for(int i = 0; i < packet.size(); i++){
                    bytesbuffer[i+1] = packet[i];
                }
                
            }
        }
        //TODO: can potentially replace this with a ROS node, and have it send information through the node.
        //flashing ROS code onto the arm board with micropython in order to have the ros node on the board
        //or just wire it to the raspberry pi which is probably easier
        int res = hid_write(SimpleComsDevice::getHandle(), bytesbuffer, len); //hid_device *dev, const unsigned char *data, size_t length
        printf("%d", res);
        if(res < 0){
            printf("Unable to write()/2: %ls\n", hid_error(SimpleComsDevice::getHandle()));
        }
        return res;
    }
    //TODO: rn the try catch stuff is wrong but I don't know what error to catch for this
    catch (int res) {
        printf("Command error, reading too fast\n");
    }
}


/**
 * read using hidapi
 * reads position data from each motor
*/
int SimpleComsDevice::read(std::vector<unsigned char> bytes, int milliseconds){



    
    //matlab has
    //self.read(1910); 1910 is idOfCommand

    //1910 is reportID aka what goes in the first position of the buffer
    if(!validHandle(SimpleComsDevice::getHandle())){
        ROS_INFO("unable to open device\n");
        hid_exit();
        throw std::runtime_error("unable to open device");
    }

    //data is buf
    /*const int length = 256; //I'd like to think it doesn't actually matter that much what the length is
    unsigned char buf[length + 1];
    memset(buf,0,sizeof(buf));*/
    
    //it wants unsigned char * instead of vector
    unsigned char* bytesbuffer = &bytes[0];
    int res = hid_read_timeout(SimpleComsDevice::getHandle(), bytesbuffer, bytes.size(), milliseconds); //hid_device *dev, unsigned char *data, size_t length, int milliseconds
    ROS_INFO("%d", res);
    
    /*
    //data is buf
    const int length = 256; //I'd like to think it doesn't actually matter that much what the length is
    unsigned char buf[length + 1];
    memset(buf,0,sizeof(buf));
    buf[0] = reportID;
    //I think this is how to get the returned info
    std::vector<double> retbuf;

    //int milliseconds = 500;
    

    //getting SimpleComsDevice::getHandle() from above
    int res = hid_read_timeout(SimpleComsDevice::getHandle(), buf, length, milliseconds); //hid_device *dev, unsigned char *data, size_t length, int milliseconds
    printf("%d", res);
    if(res < 0){
        printf("Unable to read()/2: %ls\n", hid_error(SimpleComsDevice::getHandle()));
        throw("unable to read");
    }

    //coordinate info is buf[1], buf[2], and buf[3]
    for(int i = 1; i < 4; i++){
        retbuf.push_back(buf[i]);
    }*/

    return res;
    //return retbuf; //TODO: I hope it returns the right thing
}

