#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>


double fwd_speed = 0.5;
double min_scale_angle = -30/(180*3.1426);
double max_scale_angle = 30/(180*3.1426);
float min_dist_from_obstacle = 1.5;


ros::Publisher pub;
ros::Subscriber sub;

bool keepMoving;

void stopper();
void moveForward();
void scanCallback( const sensor_msgs::LaserScan::ConstPtr& scan);
void startMoving();

int main(int argc, char**argv){
    ros::init(argc, argv, "turtlebot_sort_laserscan");
    stopper();
    startMoving();


    return 0;
}



void stopper() {
    keepMoving = true;
    ros::NodeHandle node;
    pub = node.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop",10);
    sub = node.subscribe("scan", 10, &scanCallback);
}


void moveForward(){
    geometry_msgs::Twist msg;

    msg.linear.x = fwd_speed;
    pub.publish(msg);
}

void scanCallback( const sensor_msgs::LaserScan::ConstPtr& scan){
    bool isObstacleInFront = false;

   int  minIndex = ceil((max_scale_angle - scan->angle_min)/scan->angle_increment);
    int  maxIndex = floor((max_scale_angle - scan->angle_min)/scan->angle_increment);

    int size_ranges = scan->ranges.size();

    for(int currIndex = 1; currIndex < size_ranges; currIndex++){
        if(scan->ranges[currIndex] < min_dist_from_obstacle){
            ROS_INFO("obstacle");
            isObstacleInFront = true;
            break;
        }
    }

    if(isObstacleInFront){
        ROS_INFO("Stop");
        keepMoving = false;
    }

}

void startMoving(){
    ros::Rate rate(10);
    ROS_INFO("start moving");

    while(ros::ok() && keepMoving){
        moveForward();
        ros::spinOnce();
        rate.sleep();
    }
}


