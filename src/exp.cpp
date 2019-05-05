#include "experiment.h"


int main(int argc,char **argv){

    ros::init(argc,argv,"experiment");
    ros::NodeHandle nh;
    experiment_ robot(nh);

    

    double desired_velocity = 0.7, current_velocity, distance = 5.0, propotional = 0.2;
    static double pose =0;
    //  rate (Hz) is used for defining loop frequency and profile generation 
    //  increment used to move from var to next var in profile
    int rate = 100, increment = 0, count = 0;
    // get the deccleration profile in vector form and option for choosing between 
    // straght line, exponential and logarithmic 
    int option = 1;
    // stop time (sec) is varible to contrain within which robot needs to stop
    double stop_time = 2;
    std::vector<double> profile ;
    // Boolian var for shifting from run to stop
    // bool run = true;
    int current_sim = 0, sims = 2;
    

    ros::Rate r(100);   
    

    while(ros::ok() && current_sim<sims){   
        // if pose of robot is above 5 meters shift from run to stop mode 
        if(robot.run_){
            if(count == 0){
                robot.init_pose();
                count = 1;
            }

            robot.run(desired_velocity, distance, propotional, rate);
            std::cout<<"Going to goal with simlation number "<<current_sim<<std::endl;
            
            // current_velocity = robot.vel(pose,rate);
            // std::cout<<current_velocity<<std::endl;
            r.sleep();
            ros::spinOnce();
            }

        else{
                // static int i = 0;
                // Run profile generation only once by using following condition
                if (count == 1){
                    profile = robot.decceleration_pattern(stop_time, rate, option);
                    count = 0;
                    // std::cout<<"executed "<<++i<<" times"<<std::endl;
                    }
                            
                std::cout<<profile[increment]<<std::endl;
                robot.stop(profile[increment],propotional);
                increment++;
                
                r.sleep();
                ros::spinOnce();

                if(increment == rate)
                    {
                    robot.full_stop();                  
                    robot.client.call(robot.reset_world);
                    // count = 0;
                    robot.reset_robot();
                    increment = 0;
                    robot.run_ = true;
                    current_sim++;
                    // sleep for 2 seconds
                    ros::Duration(1).sleep();
                    robot.gazebo_robot_state();
                    
                    }
            }
            
        }     
    return 0;
}
