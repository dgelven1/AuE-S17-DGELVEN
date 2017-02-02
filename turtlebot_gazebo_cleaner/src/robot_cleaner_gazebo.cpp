// Dylan Gelven
// robot_cleaner_gazebo file
// 2/1/17

#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include <sstream>

using namespace std;


ros::Publisher velocity_publisher;

void move(double speed, double distance, bool isForward);
void rotate(double angular_speed, double angle, bool clockwise);
double degrees2radians(double angle_in_degrees);
void gridClean();

const double PI = 3.14159625259;

int main(int argc, char **argv)
{
	ros::init(argc, argv, "robot_cleaner_gazebo");
	ros::NodeHandle n;

	velocity_publisher = n.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop", 10);
	ros::Rate loop_rate(10.0);
	// double i = 1;

	// do{
	// 	move(0.5,10,true);
	// 	loop_rate.sleep();
	// }
	// while(i <10);

	// move(2,100, false);

	 gridClean();
	
	

}

void move(double speed, double distance, bool isForward)
{
	geometry_msgs::Twist vel_msg;

	if (isForward)
		vel_msg.linear.x =abs(speed);
	else

	vel_msg.linear.x =-abs(speed);
	vel_msg.linear.y = 0;
	vel_msg.linear.z = 0;

	vel_msg.angular.x = 0;
	vel_msg.angular.y = 0;
	vel_msg.angular.z = 0;

	//current time
	double t0 = ros::Time::now().toSec();
	double current_distance = 0;
	ros::Rate loop_rate(10);
	do {
		velocity_publisher.publish(vel_msg);
		double t1 = ros::Time::now().toSec();
		current_distance = speed * (t1-t0);
		ros::spinOnce();
		loop_rate.sleep();
	}
	while(current_distance < distance);
	vel_msg.linear.x = 0;
	velocity_publisher.publish(vel_msg);
}

void rotate(double angular_speed, double angle, bool clockwise)
{
	geometry_msgs::Twist vel_msg;

	vel_msg.linear.x = 0;
	vel_msg.linear.y = 0;
	vel_msg.linear.z = 0;

	vel_msg.angular.x = 0;
	vel_msg.angular.y = 0;

	if (clockwise)
	vel_msg.angular.z = -abs(angular_speed);
	else 
	vel_msg.angular.z = abs(angular_speed);
	
	double current_angle = 0;
	double t0 = ros::Time::now().toSec();
	ros::Rate loop_rate(30);
	do{
		velocity_publisher.publish(vel_msg);
		double t1 = ros::Time::now().toSec();
		current_angle = angular_speed * (t1-t0);
		ros::spinOnce();
		loop_rate.sleep();
	} while (current_angle < angle);
	vel_msg.angular.z = 0;
	velocity_publisher.publish(vel_msg);
}

double degrees2radians(double angle_in_degrees)
{
	return angle_in_degrees *PI /180.0;
}

void gridClean()
{
	geometry_msgs::Twist vel_msg;
	ros::Rate loop_rate(10.0);
	double i = 1;
	loop_rate.sleep();

	do{
	move(0.75,9, true);
	ros::Duration(1.5).sleep();
	//cout<<"end first move command"<<endl;
	rotate(degrees2radians(10), degrees2radians(90), false);
	ros::Duration(1.5).sleep();
	move(1,0.5,true);
	ros::Duration(1.5).sleep();
	rotate(degrees2radians(10), degrees2radians(90), false);	
	ros::Duration(1.5).sleep();
	move(0.75,9,true);
	ros::Duration(1.5).sleep();
	rotate(degrees2radians(10), degrees2radians(90), true);
	ros::Duration(1.5).sleep();
	move(1,0.5,true);
	ros::Duration(1.5).sleep();
	rotate(degrees2radians(10), degrees2radians(90), true);
	ros::Duration(1.5).sleep();
	//ros::spinOnce();


	i++;

	cout<<" iteration of i: ";
	cout<< i << endl;

	}while(i< 8 );
	vel_msg.linear.x =0;
	vel_msg.angular.z = 0;
	velocity_publisher.publish(vel_msg);


}