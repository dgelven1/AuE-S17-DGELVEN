READ ME File:


fix for: NO file or directory found for ros/ros.h
Comment out the catkin_package() and replace with: 
include_directories(${catkin_INCLUDE_DIRS})

fix for not finding turtlesim_cleaner_node
System cannot find file in /catkin_ws/src/turtlesim_cleaner
Go to /catkin_ws/build/turtlesim_cleaner and copy the turtlesim_cleaner_node
and paste in the /catkin_ws/src/turtlesim_cleaner directory

