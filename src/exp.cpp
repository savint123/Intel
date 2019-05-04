#include "experiment.h"

int main(int argc,char **argv){

    ros::init(argc,argv,"experiment");
    ros::NodeHandle nh;
    experiment_ robot(nh);


    double desired_velocity =0.7, current_velocity, distance = 5.0, propotional = 0.8;
    static double pose =0;
    //  rate (Hz) is used for defining loop frequency and profile generation 
    //  increment used to move from var to next var in profile
    int rate = 5, increment = 0;
    // get the deccleration profile in vector form and option for choosing between 
    // straght line, exponential and logarithmic 
    int option = 1;
    // stop time (sec) is varible to contrain within which robot needs to stop
    double stop_time = 1;
    std::vector<double> profile ;
    // Boolian var for shifting from run to stop
    // bool val = true;
  
    ros::Rate r(rate);   
   

    while(ros::ok() ){   
        // if pose of robot is above 5 meters shift from run to stop mode 
        if(pose<distance){

            pose = robot.run(desired_velocity, distance, propotional, rate);
            // std::cout<<pose<<std::endl;
            current_velocity = robot.vel(pose,rate);
            std::cout<<current_velocity<<std::endl;
            r.sleep();
            ros::spinOnce();
            }

        else {
            static int i = 0;
             // Run profile generation only once by using following condition
            if (i == 0){
                profile = robot.decceleration_pattern(stop_time, rate, current_velocity, option);
                // std::cout<<"executed "<<++i<<" times"<<std::endl;
                }
            i++;            
            std::cout<<profile[increment]<<std::endl;
            robot.stop(profile[increment],propotional);
            increment++;
            r.sleep();
            ros::spinOnce();
            if(increment == rate){
                break;
                }
            }
        }     
    return 0;
}
