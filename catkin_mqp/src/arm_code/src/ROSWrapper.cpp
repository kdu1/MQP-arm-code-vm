#include <ros/ros.h>
#include "Robot.h"
#include "SimpleComsDevice.h"
#include <memory>
//#include <sensor_msgs/JointState.h>
#include <iostream>
#include <std_msgs/String.h>
#include <std_srvs/Trigger.h>
typedef std::complex<float> Complex;
typedef std::valarray<Complex> CArray;

//this will now be the "arm" class like in the example code I found
class ROSWrapperArm 
{
private:
    std::unique_ptr<Robot> robot;
    ros::Subscriber read_sub("shown_personality", 1000, ROSWrapperArm::personalityCallback);
    ros::Subscriber personality_subscriber_;
    //ros::Publisher _servo_jp_publisher;
    ros::Publisher _arm_state_publisher;

    std_msgs::String state;

    //do we need this or do we only publish when it calls write
    ros::Timer current_write_timer_;
    double publish_current_write_frequency_;

//okay nvm so it has the hiddevice(easyhid) and write stuff in a seperate file (controller) from the one doing the rossing (arm)
//seems like instead of doing a wrapper it needs to be in the code
//and publish JointState messages

public:
    /*explicit ROSWrapperArm();
    ROSWrapperArm(const ROSWrapperArm&) = delete;
    ROSWrapperArm& operator=(const ROSWrapperArm&) = delete;
    ~ROSWrapperArm() = default;*/

    ROSWrapperArm(ros::NodeHandle *nh, SimpleComsDevice* s)
    {
        this->robot = std::unique_ptr<Robot>(new Robot(s));
        //pos_subscriber_ = nh->subscribe(read_sub);
        personality_subscriber_ = nh->subscribe("shown_personality");//subscribe to personality node

        
        /*


        //call write?
        read_subscriber_ = nh->subscribe(
            "read", 10, &ROSWrapperArm::callbackRead, this);*/

        //servo_jp and pickAndPlace should be publishers
        //sensor_msgs::JointState servo_jp_state;
        //_servo_jp_publisher = nh->advertise<sensor_msgs::JointState>("servo_jp", servo_jp_state);

        state.data = "Finished";
        _arm_state_publisher = nh->advertise<std_msgs::String>("shown_personality", state);//publishes whether arm is in the process of moving



    }

    //don't really need this??
    /*void publishServoJp(const ros::TimerEvent &event)
    {
        sensor_msgs::JointState servo_jp_state;
        CArray servo_jp_res = servo_jp({0, 0, 0});// TODO: hardcoded, don't know where to get those values
        for(int i = 0; i < 3; i++){
            servo_jp_state.position.push_back(reinterpret_cast<float(&)>(servo_jp_res[i]));
        }
        current_speed_publisher_.publish(servo_jp_state);
    }

    void publishArmState(const ros::TimerEvent &event)
    {
        _arm_state_publisher.publish(state);

    }*/

    /**
     * disconnects from hidapi and stops device and code
    */
    void stop()
    {
        robot->scddisconnect(); //TODO: may be redundant
        robot->stop();
    }

    void personalityCallback(const std_msgs::String mood){
        //call pose based off of message from shown_personality
        if(mood.data.compare("smirk") == 0){
            //call the "smirk" pose
            smirk();
        }
        else if(mood.data.compare("smile") == 0){
            smile();
        }
        else if(mood.data.compare("neutral")  == 0){
            neutral();
        }
        else if(mood.data.compare("confusion") == 0){
            confusion();
        }
        else if(mood.data.compare("surprised") == 0){   
            surprised();
        }
        else{
            std::runtime_error("recieved emotion invalid");
        }
    }

    void smirk(){
        //set state to In progress
        state.data = "In progress";
        //publish state
        _arm_state_publisher.publish(state);

        //run servo jp based off of angle values
        CArray in = {Complex(0), Complex(0), Complex(0)};
        robot->servo_jp(in);
        
        //set state to Finished
        state.data = "Finished";
        //publish state
        _arm_state_publisher.publish(state);
    }

    void smile(){
        //set state to In progress
        state.data = "In progress";
        _arm_state_publisher.publish(state);

        //run servo jp based off of angle values
        CArray in = {Complex(0), Complex(0), Complex(0)};
        robot->servo_jp(in);
        //call pickAndPlace and/or servo_jp to go to the appropriate place

        //set state to Finished
        state.data = "Finished";
        //publish state
        _arm_state_publisher.publish(state);
    }

    void neutral(){
        //set state to In progress
        state.data = "In progress";
        _arm_state_publisher.publish(state);

        //run servo jp based off of angle values
        CArray in = {Complex(0), Complex(0), Complex(0)};
        robot->servo_jp(in);
        //call pickAndPlace and/or servo_jp to go to the appropriate place

        //set state to Finished
        state.data = "Finished";
        //publish state
        _arm_state_publisher.publish(state);
    }

    void confusion(){
        //set state to In progress
        state.data = "In progress";
        _arm_state_publisher.publish(state);

        //run servo jp based off of angle values
        CArray in = {Complex(0), Complex(0), Complex(0)};
        robot->servo_jp(in);
        //call pickAndPlace and/or servo_jp to go to the appropriate place

        //set state to Finished
        state.data = "Finished";
        //publish state
        _arm_state_publisher.publish(state);
    }

    void surprised(){
        //set state to In progress
        state.data = "In progress";
        _arm_state_publisher.publish(state);

        //run servo jp based off of angle values
        CArray in = {Complex(0), Complex(0), Complex(0)};
        robot->servo_jp(in);
        //call pickAndPlace and/or servo_jp to go to the appropriate place

        //set state to Finished
        state.data = "Finished";
        //publish state
        _arm_state_publisher.publish(state);
    }

    //void readCallback(const sensor_msgs::JointState& servo_states){
        //the problem: the hidapi read is something I'm not sure how to translate since I'm not totally sure how it works
        //so usually they subscribe to a topic and call the callback when the topic is updated
        //but not sure how to translate the hidapi call to the topic

        //ok so there needs to be a way to read + write from the device. And I'm not sure if the hardware of the arm boards supports that. So maybe we do need hidapi
        //my question now is: do we even need to have a ros subscriber and publisher for the servo positions if they are taken care of by hid read and write?
        //maybe if we don't want each pose to start and end at the same place, and want it so sometimes it can end with the arm in a different position, which means the following pose needs to account
        //for the previous position

        //but what is subscribing to that will return any useful information?

        //ok. So from the wrapper we can call servo_jp and pickAndPlace
    

        //read is called inside of measured_js
        //could I change the function?
        //actually not totally sure how to do this
        //have the position be a global var, have it be read before measured_js, then have measured_js change based off of that?

        //call servo_jp
        //servo1pos, servo2pos, servo3pos
        //servo_jp takes in a carray, you just don't need it
        //robot->servo_jp(servo_states.position[0], servo_states.position[1], servo_states.position[2]);
        //robot->write(servo_states.position[0], servo_states.position[1], servo_states.position[2]);

        //servoRadians = servo_des_states.position;
        //sendControlInput(RadToPWM(servoRadians), controlSpeed);
        //digitalWrite(13, HIGH-digitalRead(13));
        //write here?? but 
    //}

   /*void callbackRead(const std_msgs::Int32 &msg)
    {
        robot->read()//int id, std::vector<Complex> values idk how send this
        //this also doesn't return the actual readed thing, just the success
        //maybe I could modify it so it returns the readed thing or returns it from somewhere
    }*/

    bool callbackStop(std_srvs::Trigger::Request &req, std_srvs::Trigger::Response &res)
    {
        stop();
        res.success = true;
        res.message = "Successfully stopped motor.";
        return true;
    }
};

int main(int argc, char **argv)
{
    ros::init(argc, argv, "arm_code");
    ros::NodeHandle nh;

    ros::AsyncSpinner spinner(4);
    spinner.start();

    SimpleComsDevice s;
    ROSWrapperArm ros_driver_wrapper(&nh, &s);
    ROS_INFO("ROS driver is now started");
    
    ros::waitForShutdown();
    ros_driver_wrapper.stop();
}