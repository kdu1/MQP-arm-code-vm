//Robot.cpp
#include <ros/ros.h>
//#include "iostream"
#include <vector>
#include <thread>
#include <math.h>
//#include <std_msgs/Int32.h>
//#include <hidapi/hidapi.h>
//#include <libusb.h>
//#include <hidapi_libusb.h>
//#include "hidapi.h"
//#include <ForwardDeclarations.h>
#include <complex.h>
//#include "rt_nonfinite.h"
//#include "rt_defines.h"
#include <cmath>
#include <valarray>
#include "SimpleComsDevice.h"
//#include <sensor_msgs/JointState.h>
#include "ik3001.h"
#include "cubic_traj.h"
#include "Robot.h"
typedef std::complex<float> Complex;
typedef std::valarray<Complex> CArray;


//#include "ik3001.h"



//#include "FloatPacketType.h"
//#include "PacketType.h"


#define _USE_MATH_DEFINES
# define M_PI           std::complex<float>(3.14159265358979323846, 0)  /* pi */

//also in hid.c
struct hid_device_;
		typedef struct hid_device_ hid_device; /**< opaque hidapi structure */


/**
 * TODO: technically this does not account for the complex component.
*/
/*bool lessthancomplexreal (Complex l, const float r)
{
    //return l.imag() < r && l.real() < r;
    
    float floatval = reinterpret_cast<float(&)>(l);
    return floatval < r * r;

}*/

//device:
// Open the device using the VID, PID,
// and optionally the Serial number.


//defining this in SimpleComsDevice instead
/*
unsigned short vid = 0x16c0;
unsigned short pid = 0x0486;
hid_device * handle = hid_open(vid, pid, NULL); //from hid.c*/

/*bool validHandle(hid_device * handle){
    if (!handle) {
        printf("unable to open device\n");
        hid_exit();
        return 1;
    }
}*/


//class Robot{
   // public:
        /*explicit Robot();
        Robot(const Robot&) = delete;
        Robot& operator=(const Robot&) = delete;
        ~Robot() = default;*/
     /*   CArray endpts = {std::complex<float>(0, 0), std::complex<float>(0,0), std::complex<float>(0,0)};
        int MAX = 3;
        SimpleComsDevice* s;*/

    /**
     * Robot constructor
     * @param s - SimpleComsDevice called when calling ReadFloats and initializing
     *  Calls connect method from SimpleComsDevice
    */
    Robot::Robot(SimpleComsDevice *s){
        //SimpleComsDevice a;
        //this->a = a;

        Robot::s = s;
        s->connect();
    };

    void Robot::scddisconnect(){
        //ROS_INFO("Disconnecting...");
        s->disconnect();
    }


    /**
     * runs a trajectory using coefficients passed in as tc, and a total
     * runtime of t. if s is true, tc was calculated for joint space, if
     * false tc was calculated for task space. Can be run on cubic or
     * quintic trajectories. It also takes in a model to update the model
     * live as the robot moves through the trajectory.
     * TODO: I'm just going to try to get rid of D since I don't see why it's needed
    */
    void Robot::run_trajectory(bool s, CArray tc, float t) {

        //ROS_INFO("run_trajectory");
        //int Dcolsize = 8;
        //std::vector<std::vector<Complex>> D(8000, std::vector<Complex>(Dcolsize)); //local vector moved on return, maybe that's the problem

        try{
            Complex a1, a2, a3; //output to servo
            //number of columns in tc
            //int tt = tc[0].size(); 
            //Complex tt = tc[0].size();
            int tt = 4; //TODO: HARDCODING TO GET RID OF ERROR

            
            int jpsize = 4;
            int jvsize = 3;
            //creates empty array to store joint position data
            //ROS_INFO("before the initialization");
            
            //ROS_INFO("before measured_js");
            std::vector<CArray> jd = Robot::measured_js(true, true); //returns a 2X3 array
            //ROS_INFO("after measured_js");

            //jd size check
            if(jd.size() < 2 || jd[0].size() < 3){
                //ROS_INFO("jd size: %ld", jd.size());
                //ROS_INFO("jd[0] size: %ld", jd[0].size());
                throw std::runtime_error("Error: measured_js output is wrong size");
            }
            
            CArray jp(jpsize);
            //TODO: will it work if jd is smaller? pretty sure it's capacity vs amount used
            //jp = jd[0]; //size: 1X3 //matlab is 1 indexed!! pretty sure this is the same as jd(1, :)

            //put jd values in like this so that it avoids memory issues
            for(int k = 0; k < jd[0].size(); k++){
                jp[k] = jd[0][k];
            }

            CArray jv(jvsize);
            jv = jd[1]; //size: 1X3 //TODO: 2d or 3d?
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); //pause(10)
            jp[3] = Complex(0); //TODO: I don't know why it's 2 dimensions here*/

           
            //setting rows 0-3 to jp, 4-6 to jv TODO float check this
            //ROS_INFO("before D");

            //D size check
            /*if(D.size() < 1 || D[0].size() < Dcolsize){
                ROS_ERROR("D size: %ld", D[0].size());
                throw std::runtime_error("Error: D is wrong size");
            }

            D[0][0] = jp[0];
            D[0][1] = jp[1];
            D[0][2] = jp[2];  
            D[0][3] = 0; //This is my best guess since in matlab it's initialized to 0, so it probably just fills in what it can and leaves the rest as 0
            D[0][4] = jv[0];
            D[0][5] = jv[1];
            D[0][6] = jv[2];

            ROS_INFO("D values first init:");
            for(int k = 0; k < 8; k++){
                printf("D[0][%ld]: %f\n", k, D[0][k].real());
            };*/
            
            
            int i = 2;
            //tic
            auto start = std::chrono::high_resolution_clock::now(); //TODO something off with currTLoop
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            std::cout << "Elapsed time: " << elapsed.count() << " s\n";
            double currTLoop = elapsed.count();
            //ROS_INFO("%lf", currTLoop);

            //TODO: commenting out while loop to see what one recursion is
            while (currTLoop < t) {
                //ROS_INFO("in while");
                
                std::vector<CArray> detCheckGet = measured_js(true, false); //2 rows by 3 cols

                
                //another size check for detCheckGet
                if(detCheckGet.size() !=2 || detCheckGet[0].size() != 3){
                    //ROS_ERROR("detCheckGet size: %ld, %ld\n", detCheckGet.size(), detCheckGet[0].size());
                    throw std::runtime_error("Error: measured_js output wrong size");
                }
                //what it returns
                for(int k = 0; k < detCheckGet.size(); k++){
                    for(int l = 0; l < detCheckGet[0].size(); l++){
                        printf("detCheckGet[%d][%d]: %f\n", k, l, detCheckGet[k][l].real());
                        fflush( stdout );
                    }
                }

                //ROS_INFO("currTLoop %f", currTLoop);
                CArray detCheck = detCheckGet[0]; //row 1 ?

                //ROS_INFO("Before jacob");
                std::vector<std::vector<Complex>> jacobv = jacob3001(detCheck.apply(std::conj)); //transpose 3 by 1
                //ROS_INFO("After jacob");

                //size check jacobv, again
                if(jacobv.size() != 6, jacobv[0].size() != 3){
                    //ROS_INFO("jacobv size: %ld", jacobv.size());
                    //ROS_ERROR("jacobv size: %ld", jacobv.size());
                    throw std::runtime_error("Error: jacobv wrong size");
                }

                //transpose
                std::vector<std::vector<Complex>> jacobvtp = {jacobv[0], jacobv[1], jacobv[2]};

                for(int k = 0; k < jacobvtp.size(); k++){
                    for(int l=0; l < jacobvtp[k].size(); l++){
                        printf("jacobvtp[%d][%d]: %f\n", k, l, jacobvtp[k][l].real());
                        fflush( stdout );
                    }
                }
                //jacobv = {jacobv[0], jacobv[1], jacobv[2]};
                ////ROS_INFO("Before det");
                Complex DetJv = det(jacobvtp,3);
                //ROS_INFO("After det");

                // checks for when the arm is close to reaching singularity
                //Complex diff = DetJv - std::complex<float>(1.1, 0);
                if (DetJv.real() < 1.1) {
                    break;
                }
                //update clock
                end = std::chrono::high_resolution_clock::now();
                elapsed = end - start;
                //ROS_INFO("elapsed %lf", elapsed.count());
                Complex curT = elapsed.count(); //TODO: this could very much be causing a lot of errors
            
                //TODO: not sure if it maybe be that the next calls to aset actually replace the 2nd and 3rd row respectively but I'm going with this for now
                //tc = {{tc}, {tc}, {tc}};
                //4 is cubic, 6 is quintic
                switch (tt) {
                    case 4: 
                        //ROS_INFO("case 4");
                        //tc size check
                        if(tc.size() != 12){
                            //ROS_ERROR("tc size: %ld", tc.size());
                            throw std::runtime_error("Error: tc wrong size");
                        }
                        //tc is a one dimensional representation of a 4x3 array because c++ struggles
                        a1 = tc[0] + tc[1] * curT + tc[2] * curT * curT + tc[3] * curT * curT * curT;
                        a2 = tc[4] + tc[5] * curT + tc[6] * curT * curT + tc[7] * curT * curT * curT;
                        a3 = tc[8] + tc[9] * curT + tc[10] * curT * curT + tc[11] * curT * curT * curT;
                        break;
                    case 6:
                        //tc size check
                        /*if(tc.size() != 3 || tc[0].size() != 6){
                            ROS_INFO("tc size: %d", tc.size());
                            throw std::runtime_error("Error: tc wrong size");
                        }*/
                        //TODO: change to 1d
                        //a1 = tc[0][0] + tc[0][1] * curT + tc[0][2] * curT * curT + tc[0][3] * curT * curT * curT + tc[0][4] * curT * curT * curT * curT + tc[0][5] * curT * curT * curT * curT * curT;
                        //a2 = tc[1][0] + tc[1][1] * curT + tc[1][2] * curT * curT + tc[1][3] * curT * curT * curT + tc[1][4] * curT * curT * curT * curT + tc[1][5] * curT * curT * curT * curT * curT;
                        //a3 = tc[2][0] + tc[2][1] * curT + tc[2][2] * curT * curT + tc[2][3] * curT * curT * curT + tc[2][4] * curT * curT * curT * curT + tc[2][5] * curT * curT * curT * curT * curT;
                      
                        break;
                }
                
                //the example I'm using creates the publishers under joint and task space, same format of calling servojp
                //joint space
                if (s == true) {
                    //ROS_INFO("joint space, before servo_jp");
                    servo_jp({a1, a2, a3});//angles to get to desired position
                }
                //task space
                else {

                    //can use these to have inverse kinematics with just the
                    //TODO: also seems to require there be an extra TransformStamped message be sent for the task space. Is the ik3001 enough for that? Or do we still need to do that
                    //A = self.ik30();
                    //ROS_INFO("task space");
                    Complex angles [3] = {a1, a2, a3}; //angles to get to desired position
                    CArray A = ik3001(angles); //TODO: just have this be a CArray

                    CArray conttrans = A.apply(std::conj);

                    //ROS_INFO("before servo_jp");
                    servo_jp(conttrans);
                }
                /*else {
                    std::vector<Complex> A = self.ik3001({a1, a2, a3});
                    self.servo_jp({A[0], A[1], A[2]});
                }*/
                //ROS_INFO("before measured_js 2");
                jd = this->measured_js(true, true);
                //ROS_INFO("after measured_js 2");

                //jp = jd[0];

                //put jd values in jp like this so that it avoids memory issues
                for(int k = 0; k < jd[0].size(); k++){
                    jp[k] = jd[0][k];
                }

                jv = jd[1];
                //ROS_INFO("before init jp[3]: %f", jp[3].real());
                jp[3] = curT;
                //ROS_INFO("after init jp[3]: %f", jp[3].real());

                //if it reaches size limit
                if(i >= 8000){
                    //ROS_ERROR("reached size limit");
                    throw std::runtime_error("Error: reached size limit");
                }

                //ROS_INFO("before sleep");
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
               /*D[i][0] = jp[0];
                D[i][1] = jp[1];
                D[i][2] = jp[2];
                D[i][3] = jp[3];//now this is initialized
                D[i][4] = jv[0];
                D[i][5] = jv[1];
                D[i][6] = jv[2];
                D[i][7] = DetJv; // records determinate of top 3x3 of Jacobian

                ROS_INFO("D values:");
                for(int k = 0; k < 8; k++){
                    printf("D[i][%d]: %f\n", k, D[i][k].real());
                };
                i = i + 1;
                ROS_INFO("after D stuff");
                ROS_INFO("%f", D.size());*/
                
                //update clock again for while loop
                end = std::chrono::high_resolution_clock::now();
                elapsed = end - start;
                currTLoop = elapsed.count();
                //ROS_INFO("currTLoop: %f", currTLoop);
                //ROS_INFO("i: %d", i);
            }
            //ROS_INFO("after loop");
            //promlem has to do with D
            //trying to return results in memory error
           /*D.resize(i-1, std::vector<Complex>(Dcolsize)); // cuts D to size and returns it
            D.shrink_to_fit();
            //ROS_INFO("D row: %d col: %d", D.size(), D[0].size());*/
            //ROS_INFO("end of run_trajectory");
            //return D; 
        }
        catch (const std::exception& e) {
            //ROS_ERROR(e.what());
            throw std::runtime_error(e.what());
        }
    };





    /**
     * writes information, calls writeFloats method from SimpleComsDevice
     * @param reportID, values the id of the info to write, pretty sure it's the id in the first byte of the information for calling hid_write, and the values to write
    */
    void Robot::write(int id, std::vector<Complex> values){
        printf("write\n");
        fflush( stdout );
        printf("id: %d\n", id);
        fflush( stdout );
        try{
             Robot::s->writeFloats(id, values);//Then convert it to floats cause the complex is just floats with twice as big array
            
        }
        catch (const std::exception& e) {
            //ROS_ERROR("Command error, reading too fast\n");
            throw std::runtime_error("Command error, reading too fast\n");
        }
    }

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
    std::vector<float> Robot::read(int reportID){
        
        //matlab has
        //self.read(1910); 1910 is idOfCommand
        
        //check if connected to device correctly
        /*if(!validHandle(handle)){
            printf("unable to open device\n");
            hid_exit();
            throw ("unable to open device");
        }*/
        
        return  s->readFloats(reportID);
        //data is buf
        /*const int length = 256;
        unsigned char buf[length + 1];
        memset(buf,0,sizeof(buf));
        buf[0] = reportID;
        //I think this is how to get the returned info
        std::vector<Complex> retbuf;

        //int milliseconds = 500;
        

        //getting handle from above
        int res = hid_read_timeout(handle, buf, length, milliseconds); //hid_device *dev, unsigned char *data, size_t length, int milliseconds
        printf("%d", res);
        if(res < 0){
            printf("Unable to read()/2: %ls\n", hid_error(handle));
            throw("unable to read");
        }

        //coordinate info is buf[1], buf[2], and buf[3]
        for(int i = 1; i < 4; i++){
            retbuf.push_back(buf[i]);
        }

        return retbuf; 
        */
   }

    /**
     * moves the motors to the positions specified by the input array,
     * without interpolation
     * */
    void Robot::servo_jp(CArray array) {
        //ROS_INFO("In servo jp");

        //check array size
        if(array.size() != 3){
            //ROS_ERROR("servo_jp input size incorrect");
            throw std::runtime_error("servo_jp input size incorrect");
        }

        std::vector<Complex> packet(15); // creates an empty 15x1 array to write to the robot
        //TODO: according to the reference this is wrong - see how 0 and 1 are filled here
        packet[0] = Complex(1848); //ID
        packet[1] = Complex(1000);//Duration of movement: TODO: HARDCODED, making it extra slow just in case
        packet[2] = Complex(0.0); // Interpolation mode: 0=linear, 1=sinusoidal; bypasses interpolation
        packet[3] = array[0]; // Motor 1 target position
        packet[4] = array[1]; // Motor 2 target position
        packet[5] = array[2]; // Motor 3 target position

        printf("Data to be sent to hid_write:\n");
        fflush( stdout );
        for(int i = 0; i < packet.size(); i++){
            printf("%f\n", packet[i].real());
            fflush( stdout );
        }
        
        //TODO:uncomment
        write(1848, packet); // sends the desired motion command to the robot
        //set motor setpoints with time: id, mS duration of move, interpolation mode 0=linear 1=sinusoidal, motor 1 target position, motor 2 target position, motor 3 target position
        endpts = array; // sets the Robot's endpoint as the endpoint specified by the input array
    }

    /**
     * calculates 6x3 manipulator Jacobian from 3x1 array of joint angles
     * @param ja input joint angles
    */
    std::vector<std::vector<Complex>> Robot::jacob3001(CArray ja) {

        //ROS_INFO("jacob3001");

        //check size of ja
        if(ja.size() != 3){
            //ROS_INFO("ja size: %ld", ja.size());
            throw std::runtime_error("Error: jacobian input is wrong size");
        }

        Complex t1 = ja[0];
        Complex  t2 = ja[1];
        Complex  t3 = ja[2];

        //ROS_INFO("jacobian");
        std::vector<std::vector<Complex>> J(6, std::vector<Complex>(3));
        //hardcoded Jacobian calculation
        J = {{(std::complex<float>(5,0)*M_PI*sin((M_PI*t1)/std::complex<float>(180, 0))*sin((M_PI*(t2 - std::complex<float>(90,0)))/std::complex<float>(180, 0))*sin((M_PI*(t3 + std::complex<float>(90,0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0) - (std::complex<float>(5,0)*M_PI*std::complex<float>(sin((M_PI*t1)/std::complex<float>(180, 0)))*std::complex<float>(cos((M_PI*(t2 - std::complex<float>(90,0)))/std::complex<float>(180, 0)))*std::complex<float>(cos((M_PI*(t3 + std::complex<float>(90,0)))/std::complex<float>(180, 0))))/std::complex<float>(9,0) - (std::complex<float>(5,0)*M_PI*sin((M_PI*t1)/std::complex<float>(180, 0))*cos((M_PI*(t2 - std::complex<float>(90,0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0), 
                                                            -(std::complex<float>(5,0)*M_PI*cos((M_PI*t1)/std::complex<float>(180, 0))*sin((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0) - (std::complex<float>(5,0)*M_PI*cos((M_PI*t1)/std::complex<float>(180, 0))*cos((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0))*sin((M_PI*(t3 + std::complex<float>(90,0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0) - (std::complex<float>(5,0)*M_PI*cos((M_PI*t1)/std::complex<float>(180, 0))*cos((M_PI*(t3 + std::complex<float>(90,0)))/std::complex<float>(180, 0))*sin((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0), 
                                                            -(std::complex<float>(5,0)*M_PI*cos((M_PI*t1)/std::complex<float>(180, 0))*cos((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0))*sin((M_PI*(t3 + std::complex<float>(90,0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0) - (std::complex<float>(5,0)*M_PI*cos((M_PI*t1)/std::complex<float>(180, 0))*cos((M_PI*(t3 + std::complex<float>(90,0)))/std::complex<float>(180, 0))*sin((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0)},
                                                              
                                                              {(std::complex<float>(5,0)*M_PI*cos((M_PI*t1)/std::complex<float>(180, 0))*cos((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0) + (std::complex<float>(5,0)*M_PI*cos((M_PI*t1)/std::complex<float>(180, 0))*cos((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0))*cos((M_PI*(t3 + std::complex<float>(90,0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0) - (std::complex<float>(5,0)*M_PI*cos((M_PI*t1)/std::complex<float>(180, 0))*sin((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0))*sin((M_PI*(t3 + std::complex<float>(90,0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0), 
                                                              -(std::complex<float>(5,0)*M_PI*sin((M_PI*t1)/std::complex<float>(180, 0))*sin((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0) - (std::complex<float>(5,0)*M_PI*sin((M_PI*t1)/std::complex<float>(180, 0))*cos((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0))*sin((M_PI*(t3 + std::complex<float>(90,0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0) - (std::complex<float>(5,0)*M_PI*sin((M_PI*t1)/std::complex<float>(180, 0))*cos((M_PI*(t3 + std::complex<float>(90,0)))/std::complex<float>(180, 0))*sin((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0), 
                                                              -(std::complex<float>(5,0)*M_PI*sin((M_PI*t1)/std::complex<float>(180, 0))*cos((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0))*sin((M_PI*(t3 + std::complex<float>(90,0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0) - (std::complex<float>(5,0)*M_PI*sin((M_PI*t1)/std::complex<float>(180, 0))*cos((M_PI*(t3 + std::complex<float>(90,0)))/std::complex<float>(180, 0))*sin((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0)},
                                                              
                                                              {std::complex<float>(0,0), (std::complex<float>(5,0)*M_PI*sin((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0))*sin((M_PI*(t3 + std::complex<float>(90,0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0) - (std::complex<float>(5,0)*M_PI*cos((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0))*cos((M_PI*(t3 + std::complex<float>(90,0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0) - (std::complex<float>(5,0)*M_PI*cos((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0), (std::complex<float>(5,0)*M_PI*sin((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0))*sin((M_PI*(t3 + std::complex<float>(90,0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0) - (std::complex<float>(5,0)*M_PI*cos((M_PI*(t2 - std::complex<float>(90, 0)))/std::complex<float>(180, 0))*cos((M_PI*(t3 + std::complex<float>(90,0)))/std::complex<float>(180, 0)))/std::complex<float>(9,0)},
                                                              {std::complex<float>(0,0), -sin((M_PI*t1)/std::complex<float>(180, 0)), -sin((M_PI*t1)/std::complex<float>(180, 0))},
                                                              {std::complex<float>(0,0), cos((M_PI*t1)/std::complex<float>(180, 0)), cos((M_PI*t1)/std::complex<float>(180, 0))},
                                                              {std::complex<float>(1,0), std::complex<float>(0,0), std::complex<float>(0,0)}};

        //check size of output
        if(J.size() != 6 || J[0].size() != 3){
            ////ROS_INFO("jd size: %ld", J.size());
            //ROS_INFO("jd[0] size: %ld", J[0].size());
            throw std::runtime_error("Error: jacobian input is wrong size");
        }

        return J;
    }

        
        /**
         * wrong
         * calculates 6x3 manipulator Jacobian from 3x1 array of joint angles
        */
       /*
        double* jacob3001(const double ja[3])
        {
        double J_tmp;
        double J_tmp_tmp;
        double b_J_tmp;
        double b_J_tmp_tmp;
        double c_J_tmp;
        double c_J_tmp_tmp;
        double d_J_tmp;
        double d_J_tmp_tmp;
        double e_J_tmp;
        double e_J_tmp_tmp;
        double f_J_tmp;
        double g_J_tmp;
        double h_J_tmp;
        //  calculates 6x3 manipulator Jacobian from 3x1 array of joint
        //  angles
        //  hardcoded Jacobian calculation
        J_tmp_tmp = 3.1415926535897931 * (ja[1] - std::complex<float>(90,0).0) / 180.0;
        J_tmp = std::sin(J_tmp_tmp);
        J_tmp_tmp = std::cos(J_tmp_tmp);
        b_J_tmp_tmp = 3.1415926535897931 * (ja[2] + 90.0) / 180.0;
        b_J_tmp = std::sin(b_J_tmp_tmp);
        c_J_tmp = std::cos(b_J_tmp_tmp);
        b_J_tmp_tmp = 3.1415926535897931 * ja[0] / 180.0;
        c_J_tmp_tmp = std::sin(b_J_tmp_tmp);
        b_J_tmp_tmp = std::cos(b_J_tmp_tmp);
        d_J_tmp_tmp = 15.707963267948966 * c_J_tmp_tmp;
        d_J_tmp = d_J_tmp_tmp * J_tmp_tmp;
        e_J_tmp = 15.707963267948966 * c_J_tmp_tmp * J_tmp;
        J[0] = (e_J_tmp * b_J_tmp / 9.0 - d_J_tmp * c_J_tmp / 9.0) - d_J_tmp / 9.0;
        f_J_tmp = 15.707963267948966 * b_J_tmp_tmp;
        e_J_tmp_tmp = f_J_tmp * J_tmp_tmp;
        g_J_tmp = e_J_tmp_tmp * b_J_tmp;
        f_J_tmp = f_J_tmp * c_J_tmp * J_tmp / 9.0;
        h_J_tmp = 15.707963267948966 * b_J_tmp_tmp * J_tmp;
        J[6] = (-h_J_tmp / 9.0 - g_J_tmp / 9.0) - f_J_tmp;
        J[12] = -g_J_tmp / 9.0 - f_J_tmp;
        J[1] = (e_J_tmp_tmp / 9.0 + e_J_tmp_tmp * c_J_tmp / 9.0) -
                h_J_tmp * b_J_tmp / 9.0;
        d_J_tmp *= b_J_tmp;
        f_J_tmp = d_J_tmp_tmp * c_J_tmp * J_tmp / 9.0;
        J[7] = (-e_J_tmp / 9.0 - d_J_tmp / 9.0) - f_J_tmp;
        J[13] = -d_J_tmp / 9.0 - f_J_tmp;
        J[2] = 0.0;
        J_tmp = 15.707963267948966 * J_tmp * b_J_tmp / 9.0 -
                15.707963267948966 * J_tmp_tmp * c_J_tmp / 9.0;
        J[8] = J_tmp - 15.707963267948966 * J_tmp_tmp / 9.0;
        J[14] = J_tmp;
        J[3] = 0.0;
        J[9] = -c_J_tmp_tmp;
        J[15] = -c_J_tmp_tmp;
        J[4] = 0.0;
        J[10] = b_J_tmp_tmp;
        J[16] = b_J_tmp_tmp;
        J[5] = 1.0;
        J[11] = 0.0;
        J[17] = 0.0;

        return J;
        }*/


    /**
     * @param GETPOS, GETVEL bools that determine whether to return the positions, velocity, or both
     * @return the position and/or velocity values of the motors in a 2x3 array. 
    */
    std::vector<CArray> Robot::measured_js(bool GETPOS, bool GETVEL) {
        //std::vector<std::vector<Complex>> returnArray(2, std::vector<Complex>(3, std::complex<float>(0.0, 0))); // creates a 2x3 return array
        //position
        /*ROS_INFO("measured_js");
        ROS_INFO("GETPOS, %d\n", GETPOS);
        ROS_INFO("GETVEL, %d\n", GETVEL);*/

        /**
         * This function returns a pointer to a linked list of type
				struct #hid_device_info, containing information about the HID devices
				attached to the system,
				or NULL in the case of failure or if no HID devices present in the system.
				Call hid_error(NULL) to get the failure reason.
        */
        //struct hid_device_info* info = hid_enumerate(0x239A, 0x802c);
        //ROS_INFO("vid: %d, pid: %d, serial: %ls", info->product_id, info->vendor_id, *info->serial_number);
        
        //unsigned char posPacketReportID = '1910';
        int posPacketReportID = 1910; //TODO: datatype int or unsigned char?
        std::vector<float> posPacket = read(posPacketReportID); //get positions and setpoint: number of motors, motor 1 setpoint, motor 1 position, motor 2 setpoint, motor 2 position, motor 3 setpoint, motor 3 position
        //std::vector<float> posPacket = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        CArray retPos(3);//init to right size so output size is consistent
        CArray retVel(3); 


        if (GETPOS) { // if GETPOS is true
            //ROS_INFO("getpos");
            try {
                retPos[0] = Complex((float)posPacket[2]); // gets the position of the first motor
                retPos[1] = Complex((float)posPacket[4]); // gets the position of the second motor
                retPos[2] = Complex((float)posPacket[6]); // gets the position of the third motor
                //returnArray[0] = {pos1, pos2, pos3}; // sets the top row of the return array to the position values

                //ROS_INFO("return stuff");
                //std::vector<Complex> returnArray1 = {pos1, pos2, pos3};
                //CArray retPosnew ( returnArray1.data(), returnArray1.size() ); //convert it to a CArray so it's easier to do the conjugate transpose later
                //retPos = retPosnew;
            }
            catch(const std::exception& e){
                //ROS_INFO(e);
                //ROS_ERROR("error");
                //TODO
            }
            
        }
        //velocity
        //TODO: uncomment the read
        unsigned char velPacketReportID = '1822';
        std::vector<float> velPacket = read(velPacketReportID); //get velocity data: motor 1 velocity mode setpoint, motor 1 velocity, motor 1 computed effort
                                                    //motor 2 velocity mode setpoint, motor 2 velocity, motor 2 computed effort
                                                    //motor 3 velocity mode setpoint, motor 3 velocity, motor 3 computed effort
        //std::vector<float> velPacket = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        if (GETVEL) { // if GETVEL is true
            //ROS_INFO("getvel");
            try{
            //if(velPacket.size() >= ) {
                retVel[0] = Complex((float)velPacket[2]); // gets the velocity of the first motor
                retVel[1] = Complex((float)velPacket[5]); // gets the velocity of the second motor
                retVel[2] = Complex((float)velPacket[8]); // gets the velocity of the third motor
                //returnArray[1] = {vel1, vel2, vel3}; // sets the bottom row of the return array to the velocity values
                /*std::vector<Complex> returnArray2 = {vel1, vel2, vel3};
                CArray retVelnew (returnArray2.data(), returnArray2.size() ); //convert it to a CArray so it's easier to do the conjugate transpose later
                retVel = retVelnew;*/
                //ROS_INFO("retVel.size: row %d col %d", retVel.size(), retVel[0].size());
            }
            catch(const std::exception& e){
                //ROS_ERROR("error");
            }
        }

        //ROS_INFO("ret");
        std::vector<CArray> ret = {retPos, retVel}; //the output of this is an vector of CArrays

        //check that return size is correct
        if(ret.size() < 2 || ret[0].size() < 3 || ret[1].size() < 3){
            //ROS_INFO("ret size: %ld", ret.size());
            //ROS_INFO("retPos size: %ld", ret[0].size());
            //ROS_INFO("retVel size: %ld", ret[1].size());
            throw std::runtime_error("Error: measured_js output is wrong size");
        }
        return ret; // returns the array of position and velocity values
    }

    /**
     * returns the determinant
    */
    /*Complex det(std::vector<std::vector<Complex>> input) {
        ROS_INFO("det");
        int n = input[0].size(); //TODO: not sure if always square
        ROS_INFO("n %d", n);
        
        Complex d= std::complex<float>(0,0);
        int p, h, k, i, j;
        std::vector<std::vector<Complex>> temp(n-1, std::vector<Complex>(n));//TODO: what's it trying to do here

        if(n==1) {
            return input[0][0]; 
        } else if(n==2) {
            d=(input[0][0]*input[1][1]-input[0][1]*input[1][0]);
            return d;
        } else {
            ROS_INFO("case 3 or more");
            for(p=0;p<n;p++) {
                h = 0;
                k = 0;
                for(i=1;i<n;i++) {
                    for( j=0;j<n;j++) {
                        if(j==p) {
                            continue;
                        }
                        temp[h][k] = input[i][j];
                        k++;
                        if(k==n-1) {
                            h++;
                            k = 0;
                        }
                    }
                }
                d=d+input[0][p]*std::complex<float>(pow(-1,p),0)*det(temp);
            }
            ROS_INFO("after recursion");
            return d;
        }
    }*/


    /**
     * Calculate the determinant
    */
    Complex Robot::det(std::vector<std::vector<Complex>> matrix, int size)
    {
        //det input from inside function
        //ROS_INFO("det input from inside function");
        for(int k = 0; k < matrix.size(); k++){
            for(int l=0; l < matrix[k].size(); l++){
                printf("matrix[%d][%d]: %f\n", k, l, matrix[k][l].real());
                fflush( stdout );
            }
        }
        //check if size matches
        if(matrix.size() != size){
            //ROS_ERROR("det matrix size: %ld, correct size: %ld", matrix.size(), size);
            throw std::runtime_error("Error: matrix wrong size");

        }
        Complex detr = 0;
        int sign = 1;
    
        // Base Case
        if (size == 1) {
            detr = matrix[0][0];
        }
        else if (size == 2) {
            detr = (matrix[0][0] * matrix[1][1])
                - (matrix[0][1] * matrix[1][0]);
        }
    
        // Perform the Laplace Expansion
        else {
            for (int i = 0; i < size; i++) {
    
                // Stores the cofactor matrix
                std::vector<std::vector<Complex>> cofactor(size-1, std::vector<Complex>(size-1)); //= new int*[size - 1];
                /*for (int j = 0; j < size - 1; j++) {
                    cofactor[j];
                }*/
                int sub_i = 0, sub_j = 0;
                for (int j = 1; j < size; j++) {
                    for (int k = 0; k < size; k++) {
                        if (k == i) {
                            continue;
                        }
                        cofactor[sub_i][sub_j] = matrix[j][k];
                        sub_j++;
                    }
                    sub_i++;
                    sub_j = 0;
                }
    
                // Update the determinant value
                detr += Complex(sign) * matrix[0][i]
                    * det(cofactor, size - 1);
                sign = -sign;

                //TODO: could this be the issue?
                //cofactor.clear();
                //https://stackoverflow.com/questions/10464992/c-delete-vector-objects-free-memory
                //std::vector<std::vector<Complex>>().swap(cofactor);
                /*
                for (int j = 0; j < size - 1; j++) {
                    delete[] cofactor[j];
                }*/
                //delete[] cofactor;
            }
        }
    
        // Return the final determinant value
        return detr;
    }

    /**
     * completes pick and place operation
     * @param xi, yi, zi, color - the location to navigate to
    */
    void Robot::pickAndPlace(float xi, float yi, float zi, int color) {

        //ROS_INFO("pickAndPlace");
        float traj_time = 3.0;
        float tj2 = 1.0;

        //initializing the size because otherwise there are interesting memory issues
        std::vector<float> vi{0, 0, 0};
        std::vector<float> vf{0, 0, 0};
        std::vector<float> pi{100,0,195};
        std::vector<float> pf{xi,yi,(zi+30)}; //TODO: change this

        //pi = {100,0,195};
        //pf = {xi,yi,(zi+30)};

        //vi, vf, pi, pf 
        //CArray aset = cubic_traj(traj_time, {0, 0, 0}, {0, 0, 0}, {100,0,195}, {xi,yi,(zi+30)}).apply(std::conj); 
        //CArray aset(0,12);
        CArray asetbeforeconj = cubic_traj(traj_time, vi, vf, pi, pf); 
        //ROS_INFO("after cubic_traj");
        //ROS_INFO("asetbeforeconj size: %ld", asetbeforeconj.size());
        for(int i = 0; i < asetbeforeconj.size(); i++){
            //ROS_INFO("asetbeforeconj[%ld]: %f", i, asetbeforeconj[i]);
        }

        CArray aset = asetbeforeconj.apply(std::conj); 

        //ROS_INFO("aset size: %ld", aset.size());
        for(int i = 0; i < aset.size(); i++){
            //ROS_INFO("aset[%ld]: %f", i, aset[i]);
        }

        //ROS_INFO("After cubic_traj");

        //may not need to return anything
        //std::vector<std::vector<Complex>> D1 = run_trajectory(false, aset, traj_time);
        run_trajectory(false, aset, traj_time);
        //ROS_INFO("after run_trajectory 1");

       vi = {0, 0, 0};
        vf = {0, 0, 0};
        pi = {xi,yi,zi+30};
        pf = {xi+7,yi,zi};
        CArray aset2 = cubic_traj(traj_time, vi, vf, pi, pf).apply(std::conj);
        //std::vector<std::vector<Complex>> D1_2 = run_trajectory(false, aset2, traj_time);
        run_trajectory(false, aset2, traj_time);
        //ROS_INFO("after run_trajectory 2");
        //closeGripper(); //TODO: not using gripper so I'm guessing can comment it out
        std::this_thread::sleep_for(std::chrono::seconds(1));

        vi = {0, 0, 0};
        vf = {0, 0, 0};
        pi = {xi,yi,zi};
        pf = {100,0,195};
        CArray aset3 = cubic_traj(tj2, vi, vf, pi, pf).apply(std::conj);
        //std::vector<std::vector<Complex>> D1_3 = run_trajectory(false, aset3, tj2);
        run_trajectory(false, aset3, tj2);
         //ROS_INFO("after run_trajectory 3");

        vi = {0, 0, 0};
        vf = {0, 0, 0};
        pi = {100,0,195};
        pf = {10,150,30};
        CArray aset4 = cubic_traj(traj_time, vi, vf, pi, pf).apply(std::conj);
        //std::vector<std::vector<Complex>> D1_4 = run_trajectory(false, aset4, traj_time);
        run_trajectory(false, aset4, traj_time);
         //ROS_INFO("after run_trajectory 4");
    }

    //need to exit
    void Robot::stop(){
        printf("Stopping...\n");
        fflush( stdout );
        s->disconnectDeviceImp();
    }

//};

/*
class Traj_Planner{
    public:
        Robot robot;

        Traj_Planner(Robot robot){
            robot = robot;
        }
};
*/


/**
 * Calls the procedure for moving the arm
*/
/*int mainArmCall(){
    //do the hw5 stuff here

    std::vector<std::vector<int>> cRed(3, std::vector<int>(3, 0));
    std::vector<int> desPos = cRed[0];
    SimpleComsDevice a;
    Robot robot(a);
    int x = desPos[0];
    int y = desPos[1];
    int z = desPos[2];
    robot.servo_jp({std::complex<float>(0), std::complex<float>(0), std::complex<float>(0)});
    std::this_thread::sleep_for(std::chrono::seconds(1));
    robot.pickAndPlace(x, y, z, 1);
    
    return 0;
};*/

/**
 * main ROS function
 * Starts ros, calls servo_jp and pickAndPlace to move arm
 * TODO: snap ros stuff
*/
int main(int argc, char **argv)
{
    
    //ros::init(argc, argv, "arm_code");
    //ros::NodeHandle nh;

    //ros::AsyncSpinner spinner(4);
    //spinner.start();

    //ROS_INFO("Before the cRed stuff");in
    
    //do the hw5 stuff here
    //no idea if it works like this
    printf("in main\n");
    fflush( stdout );
    std::vector<std::vector<float>> cRed(3, std::vector<float>(3, 0));
    //std::vector<float> desPos = cRed[0]; //why did I do that
    const char* path = "/dev/hidraw1";
    SimpleComsDevice s;
    //init robot
    //Robot robot(&nh, &s);
    Robot robot(&s);
    //ROS_INFO("ROS robot is now started...");

    //move arm
    
    CArray in = {std::complex<float>(0,0), std::complex<float>(0,0), std::complex<float>(0,0)};
    robot.servo_jp(in);
    printf("servo_jp done\n");
    fflush( stdout );
    //robot.stop();

    //Home position
    /*float x = 0;//desPos[0];
    float y = 1;//desPos[1];
    float z = 0;//desPos[2];*/
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    /*robot.pickAndPlace(x, y, z, (float)1.0); //test simple servo jp before doing this
    printf("pickAndPlace done\n");
    fflush( stdout );*/
    robot.stop();


    //ROS_INFO("Before spin");

    //ros::spinOnce();
    /*ros::Rate r(10); // 10 hz
    while (ros::ok())
    {
       ros::spinOnce();
       r.sleep();
    }

    
    robot.scddisconnect();
    ROS_INFO("After disconnect");
    ros::waitForShutdown();
    robot.stop();*/
    //TODO: FUTURE CASE just use ROS instead of SimplePacketComs, Hidapi
}