Launch gazebo empty world:

roslaunch turtlebot_gazebo turtlebot_world.launch world_file:=/opt/ros/kinetic/share/turtlebot_gazebo/worlds/empty.world

To run rosbag in gazebo environment with turtlebot:

rosbag play <rosbag file> /turtle1/cmd_vel:=cmd_vel_mux/input/teleop
