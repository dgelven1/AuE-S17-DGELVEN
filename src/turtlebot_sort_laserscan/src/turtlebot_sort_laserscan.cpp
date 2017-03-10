#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>

using namespace std;

double fwd_speed = 0.5;
double min_scale_angle = -30/(180*3.1426);
double max_scale_angle = 30/(180*3.1426);
float min_dist_from_obstacle = 1.5;


ros::Publisher pub;
ros::Subscriber odom_sub;
ros::Subscriber sub;

nav_msgs::Odometry odom;

bool keepMoving;

void stopper();
void moveForward();
void scanCallback( const sensor_msgs::LaserScan::ConstPtr& scan);
void odomCallback(const nav_msgs::Odometry::ConstPtr& odom_msg);
void startMoving();
double getDistance(double x1, double y1, double x2, double y2);

int main(int argc, char**argv){
    ros::init(argc, argv, "turtlebot_sort_laserscan");



    stopper();
    startMoving();


    return 0;
}



void stopper() {
    keepMoving = true;
    ros::NodeHandle node;
    pub = node.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/navi",10);
     sub = node.subscribe("scan", 10, &scanCallback);
    odom_sub = node.subscribe("odom",10, &odomCallback);
}

double getDistance(double x1, double y1, double x2, double y2)
{
  return sqrt(pow((x1-x2),2) + pow((y1-y2),2));
}

// void moveForward(){
//     geometry_msgs::Twist msg;

//     msg.linear.x = fwd_speed;
//     pub.publish(msg);
// }

void moveGoal(double distance_tolerance){

    keepMoving = true;
    geometry_msgs::Twist vel_msg;

    //Inputting goal position here
    nav_msgs::Odometry goal_odom;
    goal_odom.pose.pose.position.x = 6;
    goal_odom.pose.pose.position.y = 0; 

    double kp = 0.3;
    ros::Rate rate(3.0);
    do{
        double q = (odom.pose.pose.orientation.z);
        double yaw = 2*asin(q);
        double error = getDistance( odom.pose.pose.position.x,  odom.pose.pose.position.y, goal_odom.pose.pose.position.x, goal_odom.pose.pose.position.y);
        ROS_INFO("error = %f", error);


        vel_msg.linear.x = kp *error;
        vel_msg.linear.y = 0;
        vel_msg.linear.z = 0;

        vel_msg.angular.x = 0;
        vel_msg.angular.y = 0;
        vel_msg.angular.z = 4*(atan2(goal_odom.pose.pose.position.y-odom.pose.pose.position.y, goal_odom.pose.pose.position.x- odom.pose.pose.position.x) - yaw);

        if(vel_msg.linear.x > 1.0){
            vel_msg.linear.x = 0.5;
        }

        if (abs(vel_msg.angular.z) > 1.0){
            vel_msg.angular.z = 0.3;

        }

        // ROS_INFO("Speed = %f" , vel_msg.angular.z);

        pub.publish(vel_msg);

        ros::spinOnce();
        rate.sleep();
    } while((getDistance( odom.pose.pose.position.x,  odom.pose.pose.position.y, goal_odom.pose.pose.position.x, goal_odom.pose.pose.position.y)> distance_tolerance) && (keepMoving));
    cout<<"end move goal"<< endl;
    vel_msg.linear.x = 0;
    vel_msg.angular.z = 0;
    pub.publish(vel_msg);  
    keepMoving = false;
    rate.sleep();


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

void odomCallback(const nav_msgs::Odometry::ConstPtr& odom_msg){
    odom.pose.pose.position.x = odom_msg->pose.pose.position.x;
    odom.pose.pose.position.y = odom_msg->pose.pose.position.y;
    odom.pose.pose.position.z = odom_msg->pose.pose.position.z;

    odom.pose.pose.orientation.x = odom_msg->pose.pose.orientation.x;
    odom.pose.pose.orientation.y = odom_msg->pose.pose.orientation.y;
    odom.pose.pose.orientation.z = odom_msg->pose.pose.orientation.z;
    odom.pose.pose.orientation.w = odom_msg->pose.pose.orientation.w;

}


void startMoving(){
    ros::Rate rate(1.0);
    ROS_INFO("start moving");

    while(ros::ok() && keepMoving){
        moveGoal(1.0);
        ros::spinOnce();
        rate.sleep();
    }


}

