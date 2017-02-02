// Dylan Gelven
// Turtleism move 

#include "ros/ros.h"
#include "turtlesim/Pose.h"
#include "geometry_msgs/Twist.h"

using namespace std;

ros::Publisher velocity_publisher;
ros::Subscriber pose_subscriber;
turtlesim::Pose turtlesim_pose;

void move(double speed, double distance, bool isForward);
void rotate(double angular_speed, double angle, bool clockwise);
double degrees2radians(double angle_in_degrees);
double setDesiredOrientation(double desired_angle_radians);
void poseCallback(const turtlesim::Pose::ConstPtr & pose_message);
void moveGoal(turtlesim::Pose goal_pose, double distance_tolerance);
double getDistance(double x1, double y1, double x2, double y2);
void gridClean();
void spiralClean();
const double PI = 3.14159625359;

int main(int argc, char **argv)
{
	ros::init(argc, argv, "robot_cleaner");
	ros::NodeHandle n;

	double speed;
	double distance;
	bool isForward;
	double angular_speed;
	double angle;
	bool clockwise;

	velocity_publisher = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);

	// Can't use in gazebo '
	pose_subscriber = n.subscribe("/turtle1/pose", 10, poseCallback);
	
	// cout<<"enter speed: ";
	// cin>> speed;
	// cout<<"enter distance: ";
	// cin>> distance;
	// cout<< "forward? ";
	// cin>>isForward;
	// move(speed,distance,isForward);

	// cout<<"enter angular velocity: ";
	// cin>> angular_speed;
	// cout<<"enter desired angle: ";
	// cin>> angle;
	// cout<< "clockwise? ";
	// cin>>clockwise;
 	
	// rotate(degrees2radians(angular_speed), degrees2radians(angle), clockwise);

	// setDesiredOrientation(degrees2radians(120));
	 ros::Rate loop_rate(1.0);
	// loop_rate.sleep();
	// setDesiredOrientation(degrees2radians(-60));
	// loop_rate.sleep();
	// setDesiredOrientation(degrees2radians(0.00));

	// turtlesim::Pose goal_pose;
	// goal_pose.x = 1;
	// goal_pose.y = 1;
	// goal_pose.theta = 0;
	// moveGoal(goal_pose, 0.01);
	// loop_rate.sleep();

	 gridClean();
	//spiralClean();


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

double setDesiredOrientation(double desired_angle_radians)
{
	double relative_angle_radians = desired_angle_radians - turtlesim_pose.theta;
	bool clockwise = ((relative_angle_radians<0) ?true:false);
	//cout<<desired_angle_radians <<","<<turtlesim_pose.theta<<"."relative_angle_radians<<"."
	rotate( degrees2radians(30), abs(relative_angle_radians), clockwise);

}

void poseCallback(const turtlesim::Pose::ConstPtr & pose_message)
{
	turtlesim_pose.x = pose_message->x;
	turtlesim_pose.y = pose_message->y;
	turtlesim_pose.theta = pose_message->theta;
}
double getDistance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow((x1-x2),2)+pow((y1-y2),2));
}


void moveGoal(turtlesim::Pose goal_pose, double distance_tolerance)
{
	geometry_msgs::Twist vel_msg;
	
	ros::Rate loop_rate(30);
	do{
		vel_msg.linear.x = 1.5*getDistance(turtlesim_pose.x, turtlesim_pose.y, goal_pose.x, goal_pose.y);
		vel_msg.linear.y = 0;
		vel_msg.linear.z = 0;

		vel_msg.angular.x = 0;
		vel_msg.angular.y = 0;
		vel_msg.angular.z = 4*(atan2(goal_pose.y - turtlesim_pose.y, goal_pose.x - turtlesim_pose.x) - turtlesim_pose.theta);

		velocity_publisher.publish(vel_msg);

		ros::spinOnce();
		loop_rate.sleep();
	} while(getDistance(turtlesim_pose.x, turtlesim_pose.y, goal_pose.x, goal_pose.y)> distance_tolerance);
	cout<<"end move goal"<< endl;
	vel_msg.linear.x =0;
	vel_msg.angular.z = 0;
	velocity_publisher.publish(vel_msg);
}

void gridClean()
{
	geometry_msgs::Twist vel_msg;
	ros::Rate loop(0.5);
	turtlesim::Pose pose;
	double i = 1;
	pose.x = 0.75;
	pose.y=0.75;
	pose.theta =0;
	moveGoal(pose, 0.01);
	setDesiredOrientation(0);


	do{
		i++;
	move(2,9, true);
	rotate(degrees2radians(30), degrees2radians(90), false);
	move(2,0.5,true);
	rotate(degrees2radians(30), degrees2radians(90), false);
	move(2,9,true);
	rotate(degrees2radians(30), degrees2radians(90), true);
	move(2,0.5,true);
	rotate(degrees2radians(30), degrees2radians(90), true);
	ros::spinOnce();
	loop.sleep();


	}while(i< 9 );
	vel_msg.linear.x =0;
	vel_msg.angular.z = 0;
	velocity_publisher.publish(vel_msg);


}

void spiralClean()
{
	geometry_msgs::Twist vel_msg;

	double count = 0;
	double constant_speed = 4;
	double vk =1;
	double wk =2;
	double rk = 0.5;
	ros::Rate loop(1);

	do {
		rk = rk + 0.5;
		vel_msg.linear.x = rk;
		vel_msg.linear.y = 0;
		vel_msg.linear.z = 0;

		vel_msg.angular.x = 0;
		vel_msg.angular.y = 0;
		vel_msg.angular.z = constant_speed;

		cout<<"vel_msg.linear.x = "<<vel_msg.linear.x<<endl;
		cout<<"vel_msg.angular.z = "<<vel_msg.angular.z<<endl;
		velocity_publisher.publish(vel_msg);
		ros::spinOnce();

		loop.sleep();
	} while( (turtlesim_pose.x<10.5) && (turtlesim_pose.y<10.5));
	vel_msg.linear.x = 0;
	velocity_publisher.publish(vel_msg);
}
