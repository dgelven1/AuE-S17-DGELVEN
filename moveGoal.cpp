#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>

using namespace std;

nav_msgs::Odometry odom;
sensor_msgs::LaserScan entries;

void moveGoal(nav_msgs::Odometry goal_odom, double distance_tolerance);
double getDistance(double x1, double y1, double x2, double y2);
void odomCallBack(const nav_msgs::Odometry::ConstPtr& odom_message);
void for_callback(const sensor_msgs::LaserScan::ConstPtr& scan_msg);

  double MIN_SCAN_ANGLE_RAD = -30.0/180*M_PI;
 double MAX_SCAN_ANGLE_RAD = +30.0/180*M_PI;


ros::Publisher vel_pub;
ros::Subscriber odom_sub;
ros::Subscriber laser_sub;

int main(int argc, char** argv){
    ros::init(argc, argv, "turtle_moveGoal");

    ros::NodeHandle n;
    vel_pub = n.advertise<geometry_msgs::Twist>("cmd_vel_mux/input/navi",100);
    odom_sub = n.subscribe("odom",100, odomCallBack);
    laser_sub = n.subscribe("/scan", 100, for_callback);

  
    ros::Rate r(1.0);

    nav_msgs::Odometry goal_odom;

    goal_odom.pose.pose.position.x = 4;
    goal_odom.pose.pose.position.y = 4; 
    moveGoal(goal_odom, 0.5);

    ros::spinOnce();

    r.sleep();

}

// void sort(scan) {
//   entries = length(scan);

//   for(i=1; i>entries; i= I+1;){
//     if 0.4 < 
//   }

//}

void for_callback(const sensor_msgs::LaserScan::ConstPtr& scan) {

  int minIndex = ceil((MIN_SCAN_ANGLE_RAD - scan->angle_min) / scan->angle_increment);
  int maxIndex = floor((MAX_SCAN_ANGLE_RAD - scan->angle_min) / scan->angle_increment);
  float scanLaser = scan->ranges[minIndex];
    //   for (int currIndex = minIndex + 1; currIndex <= maxIndex; currIndex++) {
    //     if (scan->ranges[currIndex] < closestRange) {
    //         closestRange = scan->ranges[currIndex];
    //     }
    // }
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
    ROS_INFO("yaw: %f", yaw);
    double error = getDistance( odom.pose.pose.position.x,  odom.pose.pose.position.y, goal_odom.pose.pose.position.x, goal_odom.pose.pose.position.y);

    vel_msg.linear.x = Kp * error;
		vel_msg.linear.y = 0;
		vel_msg.linear.z = 0;

		vel_msg.angular.x = 0;
		vel_msg.angular.y = 0;
    vel_msg.angular.z = 4*(atan2(goal_odom.pose.pose.position.y-odom.pose.pose.position.y, goal_odom.pose.pose.position.x- odom.pose.pose.position.x) - yaw);

    if (vel_msg.linear.x > 1.0 )
      vel_msg.linear.x = 0.5; 
    else
      vel_msg.linear.x = vel_msg.linear.x;

    if (vel_msg.angular.z > 0.5)
      vel_msg.angular.z = 0.5;

    else
      vel_msg.angular.z = vel_msg.angular.z;

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

