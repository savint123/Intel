#ifndef _EXPERIMENT_H_INCLUDED_
#define _EXPERIMENT_H_INCLUDED_

#include <vector>
#include <iostream>
#include <fstream>

#include "ros/ros.h"

#include "gazebo_msgs/SpawnModel.h"
#include "nav_msgs/Odometry.h"
#include "sensor_msgs/Imu.h"

#include "geometry_msgs/Vector3.h"
#include "geometry_msgs/Quaternion.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/PoseStamped.h"
#include "tf/transform_datatypes.h"

#include "std_srvs/Empty.h"
#include "gazebo_msgs/SetModelState.h"
#include "gazebo_msgs/GetModelState.h"

#include "CSVreader.h"

class experiment_{

public:

experiment_(ros::NodeHandle &nh){
    
    odom_sub = nh.subscribe("intel_robot/diff_drive_controller/odom",100,&experiment_::odomCallback, this);
    imu_sub = nh.subscribe("imu",100, &experiment_::imuCallback, this);
    vel_pub = nh.advertise<geometry_msgs::Twist>("intel_robot/diff_drive_controller/cmd_vel",100);
    client = nh.serviceClient<std_srvs::Empty>("gazebo/reset_world");
    client_set = nh.serviceClient<gazebo_msgs::SetModelState>("gazebo/set_model_state");
    client_get = nh.serviceClient<gazebo_msgs::GetModelState>("gazebo/Get_model_state");
    
}

void odomCallback(const nav_msgs::Odometry::ConstPtr msg){

    pose_x = msg->pose.pose.position.x;
    pose_y =msg->pose.pose.position.y;

    vel_x = msg->twist.twist.linear.x;
    vel_y = msg->twist.twist.linear.y;
    
    ang_z = msg->twist.twist.angular.z;

}
void imuCallback(const sensor_msgs::Imu::ConstPtr msg){
    tf::Quaternion quat;
    tf::quaternionMsgToTF(msg->orientation, quat);
    tf::Matrix3x3(quat).getRPY(roll, pitch, yaw);
}

void init_pose(){
    *((double *)(&sim_current_pose)) = pose_x;
}

void reset_robot(){
    set.request.model_state.model_name = "intel";
    set.request.model_state.pose.position.x = 0;
    set.request.model_state.pose.position.y = 0;
    set.request.model_state.pose.position.z = 0;
    set.request.model_state.pose.orientation.x = 0;
    set.request.model_state.pose.orientation.y = 0;
    set.request.model_state.pose.orientation.z= 0;
    set.request.model_state.pose.orientation.w = 1;
    set.request.model_state.twist.linear.x  = 0;
    set.request.model_state.twist.linear.y  = 0;
    set.request.model_state.twist.linear.z  = 0;
    set.request.model_state.twist.angular.x  = 0;
    set.request.model_state.twist.angular.y  = 0;
    set.request.model_state.twist.angular.z  = 0;
    set.request.model_state.reference_frame = "world";
    client_set.call(set);
    if(set.response.success){
        ROS_INFO(" Model state is reinitialized at origin");
    }
}


void gazebo_robot_state(){
    geometry_msgs::PoseStamped grPose;
   
    get.request.model_name = "intel";
    get.request.relative_entity_name = "base_link";

    grPose.pose = get.response.pose;
    if(get.response.success){
    ROS_INFO("Model state is changed");
    ROS_INFO("pose x: [%f], y: [%f], z: [%f]",grPose.pose.position.x,grPose.pose.position.y,grPose.pose.position.z);
  }
}


void run(double &velocity, double &distance, double &propotional, int &rate){
    
    geometry_msgs::Twist msg;
    ref_yaw= 0;
    double goal_pose = sim_current_pose + distance;
    // std::cout<<"initial_pose: "<<sim_current_pose<<std::endl;
    // std::cout<<"current_pose: "<<pose_x<<std::endl;
    if(pose_x < sim_current_pose){
        throw std::invalid_argument("gazebo internal fault occured");
    }
    if(pose_x< goal_pose){     
        msg.linear.x = velocity;
//******** adding propotional controller for straight line cruise control*******// 
        error = yaw - ref_yaw;
        if (error != 0){
            error = - propotional * error ;
            msg.angular.z = (error);               
        }
        else{
            msg.angular.z = 0;
        }
        vel_pub.publish(msg);   
    }
    else {
        run_ = false;
    }
    // ROS_INFO("yaw: [%f] and error_yaw: [%f]", yaw, error);   
       
}
void full_stop(){
    geometry_msgs::Twist msg;
    msg.linear.x = 0;
    msg.angular.z = 0;
    vel_pub.publish(msg);
    ros::Duration(1).sleep();
}

void stop(double &value,double &propotional){
    geometry_msgs::Twist msg;    
    
    ref_yaw= 0;
    error = yaw - ref_yaw;
    if (error != 0){
        error = - propotional * error ;
        msg.angular.z = (error );               
    }
    else{
        msg.angular.z = 0;
    }
    msg.linear.x = value; 
    stats<<value<<","<<roll<<","<<pitch<<","<<yaw<<"\n";
    std::cout<<"current pitch is "<<pitch<<std::endl;
    // std::cout<<value<<std::endl;

    vel_pub.publish(msg);
}

// double vel(double &pose, int &rate){
//     static double current_pose = 0;
//     static double old_pose = 0;
//     static double velocity = 0;
//     current_pose = pose;    
//     velocity = (current_pose - old_pose) * rate;
//     old_pose = current_pose;
//     return velocity;    
// }

std::vector<double> read_file(std::string &file){
        static int value = 0;
        std::string velocity;
        std::string::size_type sz;
        std::vector<double> profile;        
        // try{
        //     input.open(file+(std::to_string(value))+".csv");
        //     }
        // catch(ros::Exception &e)
        // {
        //     throw std::invalid_argument("No file found with given name ");
        //     // ROS_ERROR("ROS Exception: %s", e.what());
        //     // return -1;
        //     }

        std::string filename = file+(std::to_string(value))+".csv";
        std::cout<<"this function is called!!!"<<std::endl;
        CSVreader reader(filename,","); 
        std::vector<std::vector<std::string> > dataList = reader.getData();

        for(std::vector<std::string> vec : dataList)
	        {
		        for(std::string data : vec)
		            {
			            profile.push_back(std::stod (data, &sz));
		            }
		        std::cout<<std::endl;
	        }


        // while(input.good()){
        //     getline(input,velocity,',');
        //     profile.push_back(std::stod (velocity,&sz));
        // }
        // value ++;
        return profile;
}

std::vector<double> decceleration_pattern(double &stop_time, int &number, int &opt){
     std::vector<double> decceleration ;
    //  nomralized_count devide the stop_time in required number of profiles
     double nomralized_count = stop_time /number;
    //  decceleration value given current velocity and stop time requirement
     double decceleration_ =  vel_x / stop_time;
     std::cout<<"current_velocity"<<std::endl<<vel_x<<std::endl;
    //  Twist msgs take velocity as input so we have to give all decceleration values in terms of velocities
    //  Considering current velocity and stop time we calculate decresing velocities in different time steps
     switch(opt){
        case 1:
            for(int i=1;i <= number; i++){
                // straight line decceleration profiles (y = - m * x + c )
                decceleration.push_back((-decceleration_) * (nomralized_count * i) + vel_x);
                // std::cout<<velocity/stop_time<<" : getting slope"<<std::endl;
                // std::cout<<decceleration[i]<<std::endl;
            }
            break;
        case 2:
            for(int i=1;i <= number; i++){
                // exponential deccelration profile (y = - m * exp(x))
                decceleration.push_back((-decceleration_) * std::exp(nomralized_count * i));
            }
            break;
        case 3:
            for(int i=1;i <= number; i++){
                // logarithmic decceleration profile (y = m * log(x))
                decceleration.push_back((-decceleration_) * std::log(nomralized_count * i));
            }
            break;
     }

     return decceleration;
}


public:
const double sim_current_pose = 0;
bool run_ = true;
ros::ServiceClient client;
ros::ServiceClient client_set;
ros::ServiceClient client_get;

std_srvs::Empty reset_world;
gazebo_msgs::SetModelState set;
gazebo_msgs::GetModelState get;

std::ofstream stats;
std::ifstream input;

private:

ros::Publisher vel_pub ;
ros::Subscriber odom_sub;
ros::Subscriber imu_sub;

double ref_z, ref_yaw;
double roll, pitch, yaw;
double pose_x,pose_y, vel_x, vel_y, ang_z, acc_x, acc_y;
double error, temp_ang_z =0;
double initial_pose;
};

#endif
