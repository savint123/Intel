cmake_minimum_required(VERSION 2.8.3)
project(intel)

find_package (catkin REQUIRED COMPONENTS 
  roscpp
  rospy
  std_msgs
  geometry_msgs
  nav_msgs
  tf
  message_generation)

catkin_package()

include_directories( ${catkin_INCLUDE_DIRS})

install(DIRECTORY config launch urdf
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})


add_executable(intel_vel_node src/vel.cpp)
add_dependencies(intel_vel_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(intel_vel_node ${catkin_LIBRARIES})


