#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <iostream>
#include <sstream>

using namespace std;

sensor_msgs::LaserScan laser;
nav_msgs::Odometry odom;


ros::Publisher vpub;
ros::Subscriber lsub;
ros::Subscriber osub;

bool stopMoving = true;
bool turnleft = false;

void laserCallBack(const sensor_msgs::LaserScan::ConstPtr& scan);
void odomCallBack(const nav_msgs::Odometry::ConstPtr& init_odom);
double getDistance(double x1, double y1, double x2, double y2);
void pidGoal(double distance_tolerance);

int main(int argc, char** argv){
    ros::init(argc, argv, "mybot_pid");

    ros::NodeHandle nh;

    vpub = nh.advertise<geometry_msgs::Twist>("cmd_vel",1000);
    lsub = nh.subscribe("mybot/laser/scan", 1000, &laserCallBack);
    osub = nh.subscribe("odom",1000,&odomCallBack);

    ros::Rate rate(1.0);
    while(ros::ok()){
        pidGoal(0.5);
        return 0;
    }

    return 0;

}

void laserCallBack(const sensor_msgs::LaserScan::ConstPtr& scan){
    // call back for subscriber, and where we will also run a sort command
    // of incoming scan data
     ROS_INFO("value = %f", scan->ranges[0]);
    // geometry_msgs::Twist msg;
    // double desired_distance = 0.6;
    // double yaw;

    

    //  ROS_INFO("range = %f", scan->ranges[719]);   
    

    // if((scan->ranges[719] > 10.0) &&  (scan->ranges[719] < 100)){
    //     //Need to turn left

    //     //stopMoving = false; 
    //      turnleft = true;
    // }
    
    // if(turnleft){

    //     ROS_INFO("left turn");
    //     int i = 1;
    //     ros::Rate r(0.5);
    //     while(i < 5){
    //         // ros::spinOnce();
    //         // ROS_INFO("im here");
    //         msg.angular.z = 0.1;
    //         ROS_INFO("orient z = %f", odom.pose.pose.orientation.z);
    //         i = i + 1;  
    //         vpub.publish(msg);
    //          ROS_INFO("i = %i", i);  
    //         // ROS_INFO("i should update");     
    //         // ros::spinOnce(); 
    //         r.sleep();
    //     }
    //     turnleft = false;
    //     msg.angular.z = 0;
    //     vpub.publish(msg);
    //     ROS_INFO("finished turn");
    //     ROS_INFO("left? = %b", turnleft);
        
        
    // }
        // ROS_INFO("left turn");
        // ROS_INFO("value = %f", scan->ranges[719]);
        // Set a yaw goal for a PID down at the bottom. 
    
    //ROS_INFO("the middle = %f", middle);
    // int size = scan->ranges.size();
    // ROS_INFO("size of ranges = %i", size);

}
void odomCallBack(const nav_msgs::Odometry::ConstPtr& init_odom){
  odom.pose.pose.position.x = init_odom->pose.pose.position.x;
  odom.pose.pose.position.y =  init_odom->pose.pose.position.y;
  odom.pose.pose.position.z = init_odom->pose.pose.position.z;

  odom.pose.pose.orientation.w = init_odom->pose.pose.orientation.w;
  odom.pose.pose.orientation.x = init_odom->pose.pose.orientation.x;
  odom.pose.pose.orientation.y = init_odom->pose.pose.orientation.y;
  odom.pose.pose.orientation.z = init_odom->pose.pose.orientation.z;

}
double getDistance(double x1, double y1, double x2, double y2)
{
  return sqrt(pow((x1-x2),2) + pow((y1-y2),2));
}

void pidGoal(double distance_tolerance){
    
    nav_msgs::Odometry goal_odom;
    geometry_msgs::Twist vel_msg;

    ros::Rate r(3.0);

    double Kp = 2.0;
    double Ki = 0.2;
    double Kd = 0.5;
    double integral = 0;
    double dt = 3.0;
    double derivative = 0;
    double last_error = 0;
    

    do{    
        goal_odom.pose.pose.position.x =  odom.pose.pose.position.x +5;
        goal_odom.pose.pose.position.y = 0;

        double q = odom.pose.pose.orientation.z;
        double yawcalc = asin(q);
        double yaw = 2*asin(q); 
        ROS_INFO("z = %f", q);
        ROS_INFO("yawcalc = %f", yawcalc);
        double error = getDistance( odom.pose.pose.position.x,  odom.pose.pose.position.y, goal_odom.pose.pose.position.x, goal_odom.pose.pose.position.y);
        integral += error * -dt;
        derivative = (error-last_error)/dt;
        vel_msg.linear.x = (Kp * error); + (Ki * integral) + (Kd*derivative);
		vel_msg.linear.y = 0;
		vel_msg.linear.z = 0;

		vel_msg.angular.x = 0;
		vel_msg.angular.y = 0;
         //vel_msg.angular.z = 0;
        vel_msg.angular.z = 4*(atan2(goal_odom.pose.pose.position.y-odom.pose.pose.position.y, goal_odom.pose.pose.position.x- odom.pose.pose.position.x) - yaw);
       
        


        if (vel_msg.linear.x > 0.5)
            vel_msg.linear.x = 0.5; 
        else
            vel_msg.linear.x = vel_msg.linear.x;

        if (vel_msg.angular.z > 0.5)
            vel_msg.angular.z = 0.5;

        else
            vel_msg.angular.z = vel_msg.angular.z;

        ROS_INFO("error = %f", error);


        vpub.publish(vel_msg);

        last_error = error;
        ROS_INFO("angular = %f", vel_msg.angular.z);


    ros::spinOnce();
    r.sleep();
  } while(getDistance( odom.pose.pose.position.x,  odom.pose.pose.position.y, goal_odom.pose.pose.position.x, goal_odom.pose.pose.position.y)> distance_tolerance && stopMoving);
  cout<<"end move goal"<< endl;
  vel_msg.linear.x = 0;
  vel_msg.angular.z = 0;
  vpub.publish(vel_msg);
  r.sleep(); 
}