

#include <vector>
#include <unordered_map>
//#include "PacketType.h"
//#include "FloatPacketType.h"u
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
//s bool connected = false;

    

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
    printf("SimpleComsDevice Constructor\n");
    fflush( stdout );
    //const wchar_t* serial = L"F09D5B6E5337524651202020FF083841";
    //hid_device* handle = hid_open_path(path); //open by path
    hid_device* handle = hid_open(0x16c0, 0x0486, NULL);//open by vid/pid
    //prints whether open was successful or not
    printf("hid_error: %ls\n", hid_error(handle));
    fflush( stdout );
    SimpleComsDevice::setHandle(handle);

    //print device info using enumerate for debugging
   /* hid_device_info* devinfo = hid_enumerate(0x16C0, 0x0486);
    print_devices_with_descriptor(devinfo);
    hid_free_enumeration(devinfo);*/
}



//HIDPacketComs extends this, which calls read + write, which calls hidDevice.read+write, 
//which calls HidApi.read + write
//which calls hid_api.read + write
bool SimpleComsDevice::validHandle(hid_device * handle){
    //printf("in validHandle");
    if (!handle) {
        printf("invalid handle\n");
        fflush( stdout );
        //printf("unable to open device\n");
        hid_exit();
        return 0;
    }
    return 1;
}

/**
 * print hid_device_info
 * @param cur_dev_ hid_device_info* struct
*/

void SimpleComsDevice::print_device(struct hid_device_info *cur_dev) {
	printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
    fflush( stdout );
	printf("\n");
    fflush( stdout );
	printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
    fflush( stdout );
	printf("  Product:      %ls\n", cur_dev->product_string);
    fflush( stdout );
	printf("  Release:      %hx\n", cur_dev->release_number);
    fflush( stdout );
	printf("  Interface:    %d\n",  cur_dev->interface_number);
    fflush( stdout );
	printf("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
    fflush( stdout );
	//printf("  Bus type: %u (%s)\n", (unsigned)cur_dev->bus_type, hid_bus_name(cur_dev->bus_type));
	printf("\n");
    fflush( stdout );
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

std::unordered_map<int, std::vector<Runnable>> events;
    
/*std::vector<FloatPacketType> pollingQueue;

void SimpleComsDevice::addPollingPacket(FloatPacketType packet) {
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
    printf("getPacket\n");
    fflush( stdout );
    printf("ID: %d\n", ID);
    fflush( stdout );
    if(SimpleComsDevice::getPollingQueue().size() == 0){
        return nullptr;
    }
    for (FloatPacketType q : SimpleComsDevice::getPollingQueue()) {
        printf("idOfCommand: %d \n", q.idOfCommand);
        fflush( stdout );
        if (q.idOfCommand == ID){
            //printf("id exists");
            return &q; 
        }
    } 
    return nullptr;
}

/**
 * writes target endpoint info from servo_jp and adds it to the pollingPacket to be polled and read by Process()
 * Sets polling to true and has it be called by other writeFloats method
 * @param id, values reportID and values to be written
 */
void SimpleComsDevice::writeFloats(int id, std::vector<Complex> values) {
    //double checking stuff inside values for debugging
    printf("writeFloats\n");
    fflush( stdout );
    /*for(int i = 0; i < values.size(); i++){
        printf("values[%d]: %f\n", i, values[i].real());
    }*/
    writeFloats(id, values, true);
}
   
/**
 * writes target endpoint info from servo_jp and adds it to the pollingPacket to be polled and read by Process()
 * Actual code for writeFloats with polling
 * @param id, values reportID and values to be written
 */    
void SimpleComsDevice::writeFloats(int id, std::vector<Complex> values, bool polling) {
    //TODO: this seems wrong
    //float* floatvalues = reinterpret_cast<float(&)[2]>(values);//cast from complex to float 

    //TODO: only taking the real component of the complex values because I don't need them for now. May cause issues with pickAndPlace
    float floatvalues[values.size()] = {};
    /*for(int i = 0; i < values.size(); i++){
        floatvalues[i] = values[i].real(); //TODO: why does it do that it doesn't look right
    }*/

    //another debugging check
    printf("floatvalues:\n");
    fflush( stdout );
    /*for(int i = 0; i < values.size(); i++){
        printf("floatvalues[%d]: %f\n", i,  floatvalues[i]);
    }*/
    if (getPacket(id) == nullptr) {
        FloatPacketType pt = FloatPacketType(id, 64);
        //if (!polling)//TODO: it's not gonna go here
            //pt.oneShotMode(); 
        std::vector<float> newdownstream;
        for (int i = 0; static_cast<std::vector<float>::size_type>(i) < (pt.getDownstream()).size() && static_cast<std::vector<float>::size_type>(i) < values.size(); i++){
            newdownstream.push_back(floatvalues[i]); 
            
            //TODO: is this correct?
        }
        pt.setDownstream(newdownstream);
        SimpleComsDevice::addPollingPacket(pt); //size added here will be size of FloatPacketType called in process()
        try {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } catch (const std::exception& e) {
            printf("writeFloats thread sleep exception: ");
            fflush( stdout );
            printf(e.what());
            fflush( stdout );
        } 
    } else {
        std::vector<FloatPacketType, std::allocator<FloatPacketType>> pollingQueue = SimpleComsDevice::getPollingQueue();
        for (int j = 0; static_cast<std::vector<FloatPacketType>::size_type>(j) < pollingQueue.size(); j++) {
            FloatPacketType pt = pollingQueue[j];
            std::vector<float> newdownstream; //new vector for downstream
            if (/*FloatPacketType.class.isInstance(pt) && */pt.idOfCommand == id) {
                for (int i = 0; static_cast<std::vector<float>::size_type>(i) < pt.getDownstream().size() && static_cast<std::vector<float>::size_type>(i) < values.size(); i++){
                    //check if in floatvalues bounds
                    if(i < sizeof(floatvalues)/sizeof(float)){
                        newdownstream.push_back(floatvalues[i]); 
                    }
                    //otherwise just put 0
                    else{
                        newdownstream.push_back(0);
                    }
                }
                pt.setDownstream(newdownstream);
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
 * @return vector of floats containing read information
 * gets the information given the id from the pollingPacket, then returns it
*/
 std::vector<float> SimpleComsDevice::readFloats(int id) {
    printf("readFloats\n");
    fflush( stdout );
    if (getPacket(id) == nullptr) {//packets are assigned an id related to the purpose, simplepacketcoms manages them
        //printf("id null");
        FloatPacketType fl = FloatPacketType(id, 64);
        SimpleComsDevice::addPollingPacket(fl);
        try {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } catch (const std::exception& e) {
            printf("readFloats thread sleep exception: ");
            fflush( stdout );
            printf(e.what());
            fflush( stdout );
        } 
    } 

    //printf("readFloats");
    printf("before getPacket id: %d\n", id);
    fflush( stdout );
    FloatPacketType* pt = getPacket(id);
    //printf("before getUpstream stuff");
    if(pt == nullptr){
        printf("null");
        fflush( stdout );
        //TODO: I actually don't know what to do in this case
        throw std::runtime_error("FloatPacketType null");
    }
    std::vector<float> values((pt->getUpstream()).size(), 0);
    for (int i = 0; i < (pt->getUpstream()).size() && i < values.size(); i++){
        values[i] = (float) pt->getUpstream()[i]; 
    }
    return values;
}


/**
 * gets first 4 bytes as reportID from message
 * @return int reportId
*/
 int SimpleComsDevice::getId(std::vector<unsigned char> bytes) {
    
    //return ByteBuffer.wrap(bytes).order(be).getInt(0); //ByteBuffer given array bytes, 
        //with byte order little endian
        //returns int from buffer's current position
    //get first four bytes
    //https://stackoverflow.com/questions/34943835/convert-four-bytes-to-integer-using-c
    int id = int((unsigned char)(bytes[0]) << 24 |
            (unsigned char)(bytes[1]) << 16 |
            (unsigned char)(bytes[2]) << 8 |
            (unsigned char)(bytes[3]));
    return id; //ittle endian reminder
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
                //convert downstream from float to unsigned char
                std::vector<unsigned char> message = packet.command(packet.idOfCommand, packet.getDownstream()); //this converts floats to unsigned char format for writing
                /*printf("message:\n");
                for(int the = 0; the < (int)message.size(); the++){
                    printf("message[%d]: %u\n", the, message[the]);
                }*/
                int val = write(message, message.size(), 1); //call to function that will call hid_write
                printf("result from write: %d\n", val);
                fflush( stdout );
                if (val > 0) {
                    //gets result after write
                    int readint = read(message, SimpleComsDevice::getReadTimeout()); //calls hid_read
                    printf("read result: %d \n", readint);
                    fflush( stdout );
                    //if read result greater than or equal to packet upstream size, do some timeout resolving packet managing
                    if (static_cast<std::vector<float>::size_type>(readint) >= (packet.getUpstream()).size()) {
                        int ID = SimpleComsDevice::getId(message); // if current packet being processed matches current input id 
                        if (ID == packet.idOfCommand) {
                            if (SimpleComsDevice::isTimedOut){
                                printf("Timeout resolved %d\n", ID); 
                                fflush( stdout );
                            }
                            SimpleComsDevice::isTimedOut = false;
                            std::vector<float> up = packet.parse(message);
                            for (int i = 0; static_cast<std::vector<float>::size_type>(i) < (packet.getUpstream()).size(); i++){
                                packet.getUpstream()[i] = up[i];
                            }
                        } else {
                            for (int i = 0; i < 3; i++){
                                read(message, SimpleComsDevice::getReadTimeout()); 
                            }
                            printf(" ");
                            fflush( stdout );
                            SimpleComsDevice::isTimedOut = true;
                            return;
                        } 
                    } else {
                        SimpleComsDevice::isTimedOut = true;
                        return;
                    } 
                } else {
                    printf("write return value not 1\n");
                    fflush( stdout );
                    return;
                } 
            } catch (const std::exception& e) {
                //t.printStackTrace(System.out);
                printf("exception: in process under not isvirtual ");
                printf(e.what());
                fflush( stdout );
                disconnect();
            } 
    } 
    else {
        std::vector<float> newupstream;
        std::vector<float> currentDownstream = packet.getDownstream(); 
        for (int j = 0; static_cast<std::vector<float>::size_type>(j) < (packet.getDownstream()).size() && static_cast<std::vector<float>::size_type>(j) < (packet.getUpstream()).size(); j++){
            newupstream[j] = currentDownstream[j];
            //packet.getUpstream()[j] = packet.getDownstream()[j]; 
        }
        packet.setUpstream(newupstream);
    } 
    //if there are packets that were added to events, run each one
    if (events.find(packet.idOfCommand) == events.end()){
        printf("going through events\n");
        fflush(stdout);
        //Runs runnable
        //if (events.find(packet.idOfCommand) != events.end()) {
        for (Runnable e : events[packet.idOfCommand]) {
            printf("runnable\n");
            fflush(stdout);
            if (&e != nullptr) {
                try {
                    //e(*this);
                    e(*this);
                } catch (const std::exception& e) {
                    std::cout << e.what() << std::endl;
                    fflush( stdout );
                }
            }
        }
        //}

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
        fflush( stdout );
    }
    printf("end of process\n");
    fflush( stdout );
    packet.done = true;
}

 int SimpleComsDevice::getReadTimeout() {
    return SimpleComsDevice::readTimeout;
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
    SimpleComsDevice::connected = true;
    //TODO: in the original inside of the thread they just make a new runnable
    //TODO: thread troubles may ensue
    std::thread([&]() {
        //while still connected to device, process everything in the pollingpacket which manages all the data being written and read
        //Runnable(this);
        while (SimpleComsDevice::connected) {
            try {
                std::vector<FloatPacketType, std::allocator<FloatPacketType>> pollingQueue = SimpleComsDevice::getPollingQueue();
                //process all packets in pollingQueue
                //printf("Polling queue size: %d\n", pollingQueue.size());
                for (int i = 0; static_cast<std::vector<FloatPacketType>::size_type>(i) < pollingQueue.size(); i++) {
                    printf("polling i: %d\n", i);
                    fflush( stdout );
                    FloatPacketType pollingPacket = pollingQueue[i];
                    fflush( stdout );
                    if (pollingPacket.sendOk() && pollingPacket.done == false){
                        process(pollingPacket);
                    }
                    //mark polling packet done
                    pollingQueue[i].done = true;
                    SimpleComsDevice::setPollingQueue(pollingQueue);
                }
            } catch (std::exception& e) {
                std::cout << e.what() << std::endl;
                //printf(e.what());
                fflush( stdout );
            }
            try {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            } catch (std::exception& e1) {
                std::cout << e1.what() << std::endl;
                //printf(e1.what());
                fflush( stdout );
                SimpleComsDevice::connected = false;
            }
        }
        //this->disconnectDeviceImp();
        //printf("SimplePacketComs disconnect\n");
        //fflush( stdout );
    }).detach();//TODO: figure out threads

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
    std::cout << "end of connect" << std::endl;
    fflush( stdout );
    return true;
}

/**
 * TODO: maybe not how it's supposed to be done but it's better to be safe
*/
 void SimpleComsDevice::disconnect() {
    hid_close(SimpleComsDevice::getHandle());
    hid_exit();
    SimpleComsDevice::connected = false;
}

 bool SimpleComsDevice::isVirtual() {
    return SimpleComsDevice::virtualv;
}

 void SimpleComsDevice::setVirtual(bool virtualv) {
    SimpleComsDevice::virtualv = virtualv;
}

 void SimpleComsDevice::setReadTimeout(int readTimeout) {
    SimpleComsDevice::readTimeout = readTimeout;
}

/*String getName() {
    return this->name;
}

void setName(String name) {
    this.name = name;
}*/

 bool SimpleComsDevice::getIsTimedOut() {
    return SimpleComsDevice::isTimedOut;
}

//int read(byte[] paramArrayOfbyte, int paramInt);

//int write(byte[] paramArrayOfbyte, int paramInt1, int paramInt2);

//disconnection
 bool SimpleComsDevice::disconnectDeviceImp(){
    SimpleComsDevice::connected = false;
    printf("closing device...\n");
    fflush( stdout );
    hid_close(SimpleComsDevice::getHandle());
    printf("device closed\n");
    fflush( stdout );
    hid_exit();
    return true;
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
    SimpleComsDevice::connected = set;
}

/**
 * getConnected
 * @return connected value 
*/
bool SimpleComsDevice::getConnected(){
    return SimpleComsDevice::connected;
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
 * @param packet, len, reportID 
 * the packet to write, the length of the packet, and the reportID of the packet
*/
int SimpleComsDevice::write(std::vector<unsigned char> packet, int len, unsigned char reportID){
    
    try{
        printf("write\n");
        fflush( stdout );
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
            //printf("unable to open device\n");
            printf("invalid handle\n");
            fflush( stdout );
            hid_exit();
            throw std::runtime_error("invalid handle");
            return -1;
        }
        
        //put each packet value in float array
        //TODO: may need to change this to packet size anyway
        /*const int dslen = 3;//packet.size(); //we know it's 3 I'm not dealing with c++ rn
        float ds[dslen]; //now put this in buf somewhere
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
        if (packet.size() < static_cast<std::vector<long unsigned int>::size_type>(len)) {
            len = packet.size();
        }

        //int lensend = 4; //TODO: hardcoded this as well, I think this is right

        unsigned char bytesbuffer[packet.size()+1];

        bool dropReportIdZero = false;
        if (dropReportIdZero == 0 && reportID == 0) { //TODO: not 100% on the data type conversion on reportID here, but this is for windows so it doesn't matter for this case
            // Use the alternative buffer representation that does
            // not include report ID 0x00
            // This overcomes "The parameter is incorrect" errors on
            // Windows 8 and 10
            // See the commentary on the dropReportIdZero flag for more info
            //report = new WideStringBuffer(len);
            /*if (len >= 1) {
                System.arraycopy(data, 0, report.buffer, 0, len);
            }*/
            for(int i = 0; i < len; i++){
                bytesbuffer[i] = packet[i];
            }
            //bytesbuffer = &packet[0];
        } else {
            // Put report ID into position 0 and fill out buffer
            //report = new WideStringBuffer(len + 1);
            bytesbuffer[0] = reportID; //TODO: this is redundant
            if (len >= 1) {
                
                //copy packet into bytesbuffer at index 1
                for(int i = 0; static_cast<std::vector<long unsigned int>::size_type>(i) < packet.size(); i++){
                    bytesbuffer[i+1] = packet[i];
                }
                
            }
        }
        //TODO: can potentially replace this with a ROS node, and have it send information through the node.
        //flashing ROS code onto the arm board with micropython in order to have the ros node on the board, or can use arduino
        //or just wire it to the raspberry pi which is probably easier but will have to rewrite firmware

        printf("before hid_write\n");
        fflush( stdout );
        if(!validHandle(SimpleComsDevice::getHandle())){
            throw std::runtime_error("device handle invalid");
            return -1;
        }
        //TODO: here's the issue, idk if I can write to hid_write with a float array twice as long
        int res = hid_write(SimpleComsDevice::getHandle(), bytesbuffer, len); //hid_device *dev, const unsigned char *data, size_t length
        printf("%d\n", res);
        fflush( stdout );
        printf("after hid_write\n");
        fflush( stdout );
        if(res < 0){
            printf("Unable to write()\n: %ls\n", hid_error(SimpleComsDevice::getHandle()));
            fflush( stdout );
            return -1;
        }
        return res;
    } 
    catch (const std::exception& e) {
        printf("Command error, reading too fast\n");
        fflush( stdout );
        std::cout << e.what() << std::endl;
        //printf(e.what());
        fflush( stdout );
        return -1;
    }
}


/**
 * read using hidapi
 * reads position data from each motor
*/
int SimpleComsDevice::read(std::vector<unsigned char> bytes, int milliseconds){
    printf("read\n");
    fflush( stdout );
    //reportID is what goes in the first position of the buffer
    if(!validHandle(SimpleComsDevice::getHandle())){
        hid_exit();
        throw std::runtime_error("unable to open device");
    }


    //it wants unsigned char * instead of vector
    unsigned char* bytesbuffer = &bytes[0];
    printf("before hid_read_timeout:\n");
    fflush( stdout );
    int res = hid_read_timeout(SimpleComsDevice::getHandle(), bytesbuffer, bytes.size(), milliseconds); //hid_device *dev, unsigned char *data, size_t length, int milliseconds
    printf("after hid_read_timeout\n"); //TODO: unreachable??
    fflush( stdout );
    printf("hid_read_timeout: %d\n", res);
    fflush( stdout );
    
    /*
    //data is buf
    const int length = 256; //I'd like to think it doesn't actually matter that much what the length is
    unsigned char buf[length + 1];
    memset(buf,0,sizeof(buf));
    buf[0] = reportID;
    //I think this is how to get the returned info
    std::vector<float> retbuf;

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

