# ROS-Gazebo Simulation of Differential Drive Robotic Ground Vehicle

Below are basic instructions to follow:

To run package use following command in terminal #Provided you have ros kinetic installed and catkin_ws is set up

```
cd ~/catkin_ws/src
```
```
git clone https://github.com/savint123/intel.git
```
```
cd ..
```
```
catkin_make
```

# Important
As we are going to use differential drive controller from ros and need to communicate with ros, make sure you have installed 
these plugins. If not, use the following commands in terminal to install ros controllers and gazebo plugins for ros-kinetic version.

```
sudo apt-get install ros-kinetic-gazebo*
```
```
sudo apt-get install ros-kinetic-ros-cont*
```
Now execute follwing command to visualize and control the differential drive robot

```
roslaunch intel intel_robot.launch
```
