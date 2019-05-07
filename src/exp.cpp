


#include "experiment.h"

int main(int argc,char **argv){

    ros::init(argc,argv,"experiment");
    ros::NodeHandle nh;
    experiment_ robot(nh);

    bool flag = true;
    std::string file_;
    if (argc>1 ){
        flag = false;
        // std::cout<<flag<<std::endl;
        file_ = argv[2];
        std::cout<<file_<<std::endl;
    }
    // else{
    //     flag = true;
    // }
      
    // std::cout<<argv[1];
    double desired_velocity = 1, current_velocity, distance = 5.0, propotional = 0.8;
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
    int current_sim = 0, sims = 1;
    // create profile if true else read from given files
    


    ros::Rate r(100);   
    

    while(ros::ok() && current_sim<sims){   
        // if pose of robot is above 5 meters shift from run to stop mode 

        if(robot.run_){
            if(count == 0){
                robot.init_pose();
                count = 1;
            }

            robot.run(desired_velocity, distance, propotional, rate);
            // std::cout<<"Going to goal with simulation number "<<current_sim<<std::endl;
            // current_velocity = robot.vel(pose,rate);
            // std::cout<<current_velocity<<std::endl;
            r.sleep();
            ros::spinOnce();
            }

        else{
                // static int i = 0;
                // Run profile generation only once by using following condition
                if (count == 1){
                    
                    std::cout<<flag<<std::endl;
                    ros::Duration(2).sleep();
                    if(flag){
                    std::string file_name =  "simulation"+ std::to_string(current_sim) + ".csv";
                    robot.stats.open(file_name);
                    profile = robot.decceleration_pattern(stop_time, rate, option);
                    
                    }
                    else {
                        // if flag is false simulation will take argv as file names in csv 
                        // and use that as profile
                        // file name should be like "string0.csv" 
                        // input argument should be false string

                        profile =  robot.read_file(file_);
                        // std::cout<<profile[]
                    }
                    
                    count = 0;
                    option++;
                    // std::cout<<"executed "<<++i<<" times"<<std::endl;
                    }
                            
                std::cout<<profile[increment]<<std::endl;
                robot.stop(profile[increment],propotional);
                increment++;
                
                r.sleep();
                ros::spinOnce();

                if(increment == rate)
                    {
                    robot.stats.close();
                    robot.full_stop();                  
                    robot.client.call(robot.reset_world);
                    // count = 0;
                    robot.reset_robot();
                    increment = 0;
                    robot.run_ = true;
                    ++current_sim;
                    // sleep for 2 seconds
                    ros::Duration(1).sleep();
                    robot.gazebo_robot_state();
                    
                    }
            }
            
        }     
    return 0;
}
