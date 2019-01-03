#ifndef _EXPERIMENT_H_INCLUDED_
#define _EXPERIMENT_H_INCLUDED_

#include <vector>


#include "ros/ros.h"

#include "gazebo_msgs/SpawnModel.h"
#include "nav_msgs/Odometry.h"
#include "sensor_msgs/Imu.h"

#include "geometry_msgs/Vector3.h"
#include "geometry_msgs/Quaternion.h"
#include "geometry_msgs/Twist.h"
#include "tf/transform_datatypes.h"



class experiment_{

public:
// double roll, pitch, yaw;
// double pose_x,pose_y, vel_x, vel_y, ang_z, acc_x, acc_y;
// double error, temp_ang_z =0;

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

void run(double &velocity, double &distance, double &propotional, int &rate){
    
    geometry_msgs::Twist msg;
    ref_yaw= 0;
    std::vector<double> pattern = decceleration_pattern(rate, velocity);
    // int sudo = 0;
    
    if(pose_x< distance){
    msg.linear.x = velocity;
    //* ###################################################################### *//
    //* additing propotional controller for straight line cruise control       *//
    error = yaw - ref_yaw;
    if (error != 0){
        error = propotional * error;
        msg.angular.z = -(error*5);
    }
    else{ msg.angular.z = 0;}
    //* ###################################################################### *//  
    vel_pub.publish(msg);   
    }
    else{
        msg.linear.x = 0 ;
        vel_pub.publish(msg);
    }
    // else if(pose_x >distance && pose_x < (distance+0.5)){
        
    //     for (int i = 0;  i< rate; i++){
    //     msg.linear.x = -pattern[i+2];
    //     vel_pub.publish(msg);
    //     ROS_INFO("decceleration : [%f]", -pattern[i+2]);
    //     break;
    //     }}



    // ROS_INFO("pose_x: [%f], pose_y: [%f], pitch: [%f], yaw: [%f]",pose_x,pose_y, pitch, yaw);
    // temp_ang_z = ang_z;
    // ROS_INFO("yaw: [%f] and error_yaw: [%f]", yaw, error);
   
    // std::cout<<"\n";
    
}

std::vector<double> decceleration_pattern(int &number, double &velocity){
     std::vector<double> decceleration ;
     for(int i=0;i<number; i++){

         decceleration.push_back(-(velocity/number)* i +velocity);


     }
     return decceleration;


}


private:

ros::NodeHandle n;
ros::Publisher vel_pub = n.advertise<geometry_msgs::Twist>("intel_robot/diff_drive_controller/cmd_vel",100);
double ref_z, ref_yaw;
double roll, pitch, yaw;
double pose_x,pose_y, vel_x, vel_y, ang_z, acc_x, acc_y;
double error, temp_ang_z =0;
};












#endif
