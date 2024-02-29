//Robot.cpp

#include <iostream>
#include <vector>
#include <thread>
#include <math.h>
#include <hidapi/hidapi.h>
//#include <libusb.h>
//#include <hidapi_libusb.h>
//#include "hidapi.h"
//#include <ForwardDeclarations.h>
//#include <eigen3/Eigen/Dense> //error
#include <complex>
#include "rt_nonfinite.h"
#include "rt_defines.h"
#include <cmath>
#include <valarray>
#include "cubic_traj.h"
typedef std::complex<float> Complex;
typedef std::valarray<Complex> CArray;

#include "ik3001.h"

//#include "FloatPacketType.h"
//#include "PacketType.h"


#define _USE_MATH_DEFINES
//# define M_PI           3.14159265358979323846  /* pi */

//also in hid.c
//struct hid_device_;
//		typedef struct hid_device_ hid_device; /**< opaque hidapi structure */

//device:
//from test.c
// Open the device using the VID, PID,
// and optionally the Serial number.
/*unsigned short vid = 0x16c0;
unsigned short pid = 0x0486;
hid_device * handle = hid_open(vid, pid, NULL); //from hid.c

bool validHandle(hid_device * handle);*/


class Robot{
    public:
        CArray endpts = {Complex(0), Complex(0), Complex(0)};
        int MAX = 3;
        SimpleComsDevice* s;

        Robot(SimpleComsDevice *s);
        /**
         * runs a trajectory using coefficients passed in as tc, and a total
         * runtime of t. if s is true, tc was calculated for joint space, if
         * false tc was calculated for task space. Can be run on cubic or
         * quintic trajectories. It also takes in a model to update the model
         * live as the robot moves through the trajectory.
        */
        void run_trajectory(bool s, CArray tc, float t);
        //std::vector<std::vector<double>> run_trajectory(bool s, std::vector<std::vector<double>> tc, double t);


        /**
         * write converting it to the correct packet thing format
        */
    /**
        void writeFloats(int id, std::vector<double> values) {
            writeFloats(id, values, true);
        }
        
        //and then it calls hid_write... where???
        //okay so there's a connect method that calls a process method that calls the actual write using
        //the pollingpacket 
        void writeFloats(int id, std::vector<double> values, bool polling) {
            if (getPacket(id) == NULL) {
                FloatPacketType pt = FloatPacketType(id, 64);
                if (!polling.booleanValue()){
                    pt.oneShotMode(); 
                }
                //why does the thing not have braces??
                for (int i = 0; i < (pt.getDownstream()).size() && i < values.size(); i++){
                    pt.getDownstream()[i] = values[i];
                    addPollingPacket(pt);
                }
                try {
                    Thread.sleep(10L);
                } catch (const std::exception& e) {
                    printf("writeFloats thread sleep exception: ");
                    printf(e.what());
                } 
            } else {
                for (int j = 0; j < this->pollingQueue.size(); j++) {
                    PacketType pt = this->pollingQueue.get(j);
                    if (FloatPacketType.class.isInstance(pt) && 
                        pt.idOfCommand == id) {
                        for (int i = 0; i < (pt.getDownstream()).length && i < values.size(); i++){
                            pt.getDownstream()[i] = (float)values[i]; 
                        }
                        if (!polling.booleanValue()){
                            pt.oneShotMode(); 
                        }
                        return;
                    } 
                } 
            } 
        }*/
        /**
         * write using hidapi
        */
        void write(int id, std::vector<Complex> values);

    /**
     * Write an Output report to a HID device using a simplified interface
     *
     * <b>HID API notes</b>
     *
     * In USB HID the first byte of the data packet must contain the Report ID.
     * For devices which only support a single report, this must be set to 0x00.
     * The remaining bytes contain the report data. Since the Report ID is mandatory,
     * calls to <code>hid_write()</code> will always contain one more byte than the report
     * contains.
     *
     * For example, if a HID report is 16 bytes long, 17 bytes must be passed to <code>hid_write()</code>,
     * the Report ID (or 0x00, for devices with a single report), followed by the report data (16 bytes).
     * In this example, the length passed in would be 17.
     *
     * <code>hid_write()</code> will send the data on the first OUT endpoint, if one exists.
     * If it does not, it will send the data through the Control Endpoint (Endpoint 0)
     *
     * @param device   The device
     * @param data     The report data to write (should not include the Report ID)
     * @param len      The length of the report data (should not include the Report ID)
     * @param reportId The report ID (or (byte) 0x00)
     *
     * @return The number of bytes written, or -1 if an error occurs
     */

        /**
         * reads information, calls readFloats method from SimpleComsDevice
         * @param reportID the report ID (or (byte) 0x00)
         * @return The number of bytes written, or -1 if an error occurs
        */
        std::vector<float> read(int reportID);

        /**
         * moves the motors to the positions specified by the input array,
         * without interpolation
         * 
         * */
        void servo_jp(CArray array); 

        /**
         * calculates 6x3 manipulator Jacobian from 3x1 array of joint angles
         * @param ja input joint angles
        */
        std::vector<std::vector<Complex>> jacob3001(CArray ja);

        /**
         * @param GETPOS, GETVEL bools that determine whether to return the positions, velocity, or both
         * @return the position and/or velocity values of the motors in a 2x3 array. 
        */
        std::vector<CArray> measured_js(bool GETPOS, bool GETVEL);


        /**
         * returns the determinant
         * @param matrix, size
         * input matrix to calculate the determinant of and dimensions of it
        */
        Complex det(std::vector<std::vector<Complex>> matrix, int size);


         /**
         * completes pick and place operation
         * @param xi, yi, zi, color - the location to navigate to
        */
        void pickAndPlace(float xi, float yi, float zi, int color);

        /**
         * Disconnects from SimpleComsDevice
        */
        void scddisconnect();

        /**
         * Disconnects from hid and stops device
        */
        void stop();
};
