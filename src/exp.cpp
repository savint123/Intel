#include "experiment.h"

int main(int argc,char **argv){

    ros::init(argc,argv,"experiment");
    ros::NodeHandle nh;
    experiment_ robot(nh);


    double velocity =0.5, distance = 5, propotional = 0.4, pose; 
    int rate = 10, increment = 0;
    // get the deccleration profile in vector form and option for choosing between 
    // straght line and exponential 
    int option = 3;
    std::vector<double> profile = robot.decceleration_pattern(rate, velocity,option);
    // Boolian var for shifting from run to stop
    bool val = true;
    // Loop rate
    ros::Rate r(10);   


    while(ros::ok() ){    
        if(val){
            pose = robot.run(velocity,distance, propotional, rate);
            }

        else {
            robot.stop(profile[increment],propotional);
            increment++;
            r.sleep();
            ros::spinOnce();
            }

    // if pose of robot is above 5 meters shift from run to stop mode 
        if (pose<5){
            r.sleep();
            ros::spinOnce();
            }
        else{
            val = false;
            }
   
    }   
    
    
    return 0;
}