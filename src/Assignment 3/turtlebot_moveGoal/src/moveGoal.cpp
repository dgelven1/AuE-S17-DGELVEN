#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <iostream>
#include <sstream>

using namespace std;

nav_msgs::Odometry odom;
sensor_msgs::LaserScan entries;

void moveGoal(nav_msgs::Odometry goal_odom, double distance_tolerance);
double getDistance(double x1, double y1, double x2, double y2);
void odomCallBack(const nav_msgs::Odometry::ConstPtr& odom_message);
void for_callback(const sensor_msgs::LaserScan::ConstPtr& scan_msg);
void sort(sensor_msgs::LaserScan entries);

int sect_1;
int sect_2;
int sect_3;

ros::Publisher vel_pub;
ros::Subscriber odom_sub;
ros::Subscriber laser_sub;

int main(int argc, char** argv){
    ros::init(argc, argv, "turtle_moveGoal");

    ros::NodeHandle n;
    // for mybot switched the topic to publish to from cmd_vel_mux/input/navi to just cmd_vel
    vel_pub = n.advertise<geometry_msgs::Twist>("cmd_vel",100);
    odom_sub = n.subscribe("odom",100, odomCallBack);
    laser_sub = n.subscribe("/scan", 100, for_callback);

    ros::Rate r(1.0);
//    ROS_INFO(" sect_1 = %i; sect_2 = %i; sect_3 = %i", sect_1,sect_2,sect_3);

   nav_msgs::Odometry goal_odom;

    goal_odom.pose.pose.position.x = 3;
    goal_odom.pose.pose.position.y = 3; 

   moveGoal(goal_odom, 0.5);


}

// void sort(sensor_msgs::LaserScan entries) {
  
//   vector<float> vector_ranges = entries.ranges;
//   int size_entries = vector_ranges.size();

//     sect_1 = 0;
//     sect_2 = 0;
//     sect_3 = 0;

//   for(float i=1; i>size_entries; i= i+1){
//     if(0.4 < vector_ranges[i] < 0.75){
//       if(0 < i < size_entries/3){
//         sect_1 = 1;
//       }
//       else if(size_entries/3 < i < size_entries/2){
//         sect_2 = 1;
//       }

//       else if( size_entries/2< i  < size_entries){
//         sect_3 = 1;
//       }

//     ROS_INFO(" sect_1 = %i; sect_2 = %i; sect_3 = %i", sect_1,sect_2,sect_3);
//     }
//   }
// }






void for_callback(const sensor_msgs::LaserScan::ConstPtr& scan) {
  ROS_INFO("current first value: %f", scan->ranges[0]);


}

void odomCallBack(const nav_msgs::Odometry::ConstPtr& odom_message) {
  odom.pose.pose.position.x = odom_message->pose.pose.position.x;
  odom.pose.pose.position.y =  odom_message->pose.pose.position.y;
  odom.pose.pose.position.z = odom_message->pose.pose.position.z;

  odom.pose.pose.orientation.w = odom_message->pose.pose.orientation.w;
  odom.pose.pose.orientation.x = odom_message->pose.pose.orientation.x;
  odom.pose.pose.orientation.y = odom_message->pose.pose.orientation.y;
  odom.pose.pose.orientation.z = odom_message->pose.pose.orientation.z;

}

double getDistance(double x1, double y1, double x2, double y2)
{
  return sqrt(pow((x1-x2),2) + pow((y1-y2),2));
}


void moveGoal(nav_msgs::Odometry goal_odom, double distance_tolerance )
{
  geometry_msgs::Twist vel_msg;

  ros::Rate loop_rate(3.0);

  double  Kp = 0.3;
 
  do{

    double q = odom.pose.pose.orientation.z;
    double yaw = 2*asin(q); 
    double error = getDistance( odom.pose.pose.position.x,  odom.pose.pose.position.y, goal_odom.pose.pose.position.x, goal_odom.pose.pose.position.y);

    vel_msg.linear.x = Kp * error;
		vel_msg.linear.y = 0;
		vel_msg.linear.z = 0;

		vel_msg.angular.x = 0;
		vel_msg.angular.y = 0;
    vel_msg.angular.z = 4*(atan2(goal_odom.pose.pose.position.y-odom.pose.pose.position.y, goal_odom.pose.pose.position.x- odom.pose.pose.position.x) - yaw);

    if (vel_msg.linear.x > .2 )
      vel_msg.linear.x = 0.2; 
    else
      vel_msg.linear.x = vel_msg.linear.x;

    if (vel_msg.angular.z > 0.5)
      vel_msg.angular.z = 0.5;

    else
      vel_msg.angular.z = vel_msg.angular.z;

    ROS_INFO("error = %f", error);
    ROS_INFO("velocity = %f", vel_msg.linear.x);

    vel_pub.publish(vel_msg);



    ros::spinOnce();
    loop_rate.sleep();
  } while(getDistance( odom.pose.pose.position.x,  odom.pose.pose.position.y, goal_odom.pose.pose.position.x, goal_odom.pose.pose.position.y)> distance_tolerance);
  cout<<"end move goal"<< endl;
  vel_msg.linear.x = 0;
  vel_msg.angular.z = 0;
  vel_pub.publish(vel_msg);
  loop_rate.sleep();

}

