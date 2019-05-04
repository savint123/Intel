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
            error = -1.2 * propotional * error ;
            msg.angular.z = (error);               
        }
        else{
            msg.angular.z = 0;
        }
        vel_pub.publish(msg);   
    }

    // ROS_INFO("yaw: [%f] and error_yaw: [%f]", yaw, error);   
    return pose_x;
       
}

void stop(double &value,double &propotional){
    geometry_msgs::Twist msg;
    ref_yaw= 0;
    error = yaw - ref_yaw;
    if (error != 0){
        error = -1.2 * propotional * error ;
        msg.angular.z = (error );               
    }
    else{
        msg.angular.z = 0;
    }
    msg.linear.x = value; 
    // std::cout<<value<<std::endl;
    vel_pub.publish(msg);
}

double vel(double &pose, int &rate){
    static double current_pose = 0;
    static double old_pose = 0;
    static double velocity = 0;

    current_pose = pose;    
    velocity = (current_pose - old_pose) * rate;
    old_pose = current_pose;

    return velocity;
    
}

std::vector<double> decceleration_pattern(double &stop_time, int &number, double &velocity, int &opt){
     std::vector<double> decceleration ;
    //  nomralized_count devide the stop_time in required number of profiles
     double nomralized_count = stop_time /number;
    //  decceleration value given current velocity and stop time requirement
     double decceleration_ = velocity / stop_time;
    //  std::cout<<"current_velocity"<<std::endl<<velocity<<std::endl;
    //  Twist msgs take velocity as input so we have to give all decceleration values in terms of velocities
    //  Considering current velocity and stop time we calculate decresing velocities in different time steps
     switch(opt){
        case 1:
            for(int i=1;i <= number; i++){
                // straight line decceleration profiles (y = - m * x + c )
                decceleration.push_back((-decceleration_) * (nomralized_count * i) + velocity);
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


private:

ros::Publisher vel_pub ;
ros::Subscriber odom_sub;
ros::Subscriber imu_sub;

double ref_z, ref_yaw;
double roll, pitch, yaw;
double pose_x,pose_y, vel_x, vel_y, ang_z, acc_x, acc_y;
double error, temp_ang_z =0;
double deccl;



};












#endif
