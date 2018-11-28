
#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "std_msgs/Float64.h"
#include "nav_msgs/Odometry.h"
#include "tf/tf.h"
#include <math.h>



class intel{

public:
void callbackOdom(const nav_msgs::Odometry::ConstPtr& odom ){
pose_x = odom->pose.pose.position.x;
pose_y = odom->pose.pose.position.y;
vel_odom_lx = odom->twist.twist.linear.x;
vel_odom_ly = odom->twist.twist.linear.y;
vel_odom_az = odom->twist.twist.angular.z;
quat_x = odom->pose.pose.orientation.x;
quat_y = odom->pose.pose.orientation.y;
quat_z = odom->pose.pose.orientation.z;
quat_w = odom->pose.pose.orientation.w;

tf::Quaternion q(quat_x, quat_y, quat_z, quat_w);
tf::Matrix3x3 m(q);
m.getRPY(roll, pitch, yaw);
ROS_INFO("roll: [%f] pitch: [%f] yaw: [%f]",roll, pitch, yaw);
}


void publish()
{geometry_msgs::Twist vel;
if(pose_x<5){
vel_lx= 1;
vel_az= 0;
vel.linear.x = vel_lx;
vel.angular.z= vel_az;
vel_pub.publish(vel);}
else{
vel_lx= 0;
vel_az= 0;
vel.linear.x = vel_lx;
vel.angular.z= vel_az;
vel_pub.publish(vel);}
}


private:
double vel_lx, vel_az;
double pose_x, pose_y, vel_odom_lx, vel_odom_ly,vel_odom_az, quat_x, quat_y, quat_z,quat_w;
ros::NodeHandle n;
ros::Publisher vel_pub = n.advertise<geometry_msgs::Twist>("intel_robot/diff_drive_controller/cmd_vel",1000);
ros::Subscriber odom = n.subscribe("intel_robot/diff_drive_controller/odom",1000,&intel::callbackOdom,this);
double roll, pitch, yaw;

};


int main(int argc, char **argv){
ros::init(argc,argv,"robot_vel_publisher");
intel obj;
ros::Rate loop_rate(50);
while(ros::ok()){
obj.publish();
ros::spinOnce();
loop_rate.sleep();
}
return 0;
}
