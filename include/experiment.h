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

experiment_(ros::NodeHandle &nh){
    
    odom_sub = nh.subscribe("intel_robot/diff_drive_controller/odom",100,&experiment_::odomCallback, this);
    imu_sub = nh.subscribe("imu",100, &experiment_::imuCallback, this);
    vel_pub = nh.advertise<geometry_msgs::Twist>("intel_robot/diff_drive_controller/cmd_vel",100);
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

double run(double &velocity, double &distance, double &propotional, int &rate){
    
    geometry_msgs::Twist msg;
    ref_yaw= 0;
    
    if(pose_x< distance){     
        msg.linear.x = velocity;
//******** adding propotional controller for straight line cruise control*******// 
        error = yaw - ref_yaw;
        if (error != 0){
            error = propotional * error;
            msg.angular.z = -(error*5);               
        }
        else{
            msg.angular.z = 0;
        }
        vel_pub.publish(msg);   
    }

    // else if(pose_x >distance){
    //      std::vector<double> pattern = decceleration_pattern(rate, velocity);        
    //     for (int i = 0;  i< rate; i++){
    //         deccl = velocity* pattern[i];
    //         msg.linear.x = deccl;
    //         vel_pub.publish(msg);
    //         ROS_INFO("decceleration : [%f]", deccl);
    //         break;

    //     }
    // }
    temp_ang_z = ang_z;
    // ROS_INFO("yaw: [%f] and error_yaw: [%f]", yaw, error);   
    // std::cout<<"\n";
    return pose_x;
       
}

void stop(double value,double &propotional){
    geometry_msgs::Twist msg;
    ref_yaw= 0;
    error = yaw - ref_yaw;
    if (error != 0){
        error = propotional * error;
        msg.angular.z = -(error*5);               
    }
    else{
        msg.angular.z = 0;
    }
    msg.linear.x = value; 
    std::cout<<value<<std::endl;
    vel_pub.publish(msg);
}

std::vector<double> decceleration_pattern(int &number, double &velocity, int &opt){
     std::vector<double> decceleration ;
     switch(opt){
        case 1:
            for(int i=0;i<number; i++){
                // straight line decceleration profiles
                decceleration.push_back(-(velocity/number)* i +velocity);
            }
            break;
        case 2:
            for(int i=0;i<number; i++){
                // exponential deccelration profile
                decceleration.push_back(velocity * std::exp(-i));
            }
            break;
        case 3:
            for(int i=0;i<number; i++){
                // logarithmic decceleration profile
                decceleration.push_back(- velocity * std::log(i));
            }
            break;
     }
     
     return decceleration;
}


private:
ros::Publisher vel_pub ;//= n.advertise<geometry_msgs::Twist>("intel_robot/diff_drive_controller/cmd_vel",100);
ros::Subscriber odom_sub;// = nh.subscribe("intel_robot/diff_drive_controller/odom",100,&experiment_::odomCallback, &robot);
ros::Subscriber imu_sub;// = nh.subscribe("imu",100, &experiment_::imuCallback, &robot);
double ref_z, ref_yaw;
double roll, pitch, yaw;
double pose_x,pose_y, vel_x, vel_y, ang_z, acc_x, acc_y;
double error, temp_ang_z =0;
double deccl;
};












#endif
