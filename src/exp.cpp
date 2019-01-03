// #include "ros/ros.h"

// #include "gazebo_msgs/SpawnModel.h"
// #include "nav_msgs/Odometry.h"
// #include "sensor_msgs/Imu.h"

// #include "geometry_msgs/Vector3.h"
// #include "geometry_msgs/Quaternion.h"
// #include "tf/transform_datatypes.h"

#include "experiment.h"




int main(int argc,char **argv){

    ros::init(argc,argv,"experiment");
    ros::NodeHandle nh;
    experiment_ robot;

    ros::Subscriber odom_sub = nh.subscribe("intel_robot/diff_drive_controller/odom",100,&experiment_::odomCallback, &robot);
    ros::Subscriber imu_sub = nh.subscribe("imu",100, &experiment_::imuCallback, &robot);
    double velocity =0.8, distance = 5, propotional = 0.4; 
    //* velocity= 0.5 and propotional = 0.2 rate =50 *//
    int rate = 50;
    int sudo=0;
    ros::Rate r(rate);
    


    while(ros::ok() || sudo<1000){    
    robot.run(velocity,distance, propotional, rate);
    sudo++;
    r.sleep();
    ros::spinOnce();
    
    }   
   
    
    return 0;
}