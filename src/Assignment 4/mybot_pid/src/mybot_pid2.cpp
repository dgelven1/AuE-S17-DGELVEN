#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <iostream>
#include <sstream>

using namespace std;

sensor_msgs::LaserScan laser;

ros::Publisher vpub;
ros::Subscriber lsub;

double error;
double previous_error;
double a;
double b;
double theta;
double alpha;
double AB;
double AC;
double CD;
double goal_dist;

bool straight = true ;



void laserCallBack(const sensor_msgs::LaserScan::ConstPtr& scan);
void pidControl();

int main(int argc, char** argv){
    ros::init(argc, argv, "mybot_pid");
    
    ros::NodeHandle n;

    vpub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1000);
    lsub = n.subscribe("mybot/laser/scan", 1000, &laserCallBack);

    ros::Rate rate(1.0);
    while(ros::ok()){
        pidControl();
        ros::spinOnce();
        rate.sleep();

        return 0;
    }
    return 0;


}

void laserCallBack(const sensor_msgs::LaserScan::ConstPtr& scan){
    goal_dist = 2.0;
    // try to set ranges array equal to laser variable
     laser.ranges = scan->ranges;

     a = scan->ranges[119];
     b = scan->ranges[0];
    // theta is in degrees
     theta = 30*(3.14/180); 

    //Logic from F1tenth website 
     alpha = atan((a*cos(theta) - b)/a*sin(theta));
    //Original distance robot to wall
     AB = b*cos(alpha);

     AC = 0.1;
     CD = AB + AC*sin(alpha);
    error = goal_dist - CD;
    //ROS_INFO("Error = %f", error);

    if(scan->ranges[359] < 5.0){ // && (scan->ranges[719] < 100)){
        // ROS_INFO("set left turn to true");
        // ROS_INFO("719 value = %f", scan->ranges[719]);
        straight = false;
        
    }

}
void pidControl(){
    geometry_msgs::Twist vel;
    double kp = 2.0;
    double kd = 0.5;
    previous_error = 0;

    ros::Rate r(5.0);

    while(ros::ok()){
        vel.linear.x = 0.2;

        if(straight){

            double PID = kp *error + kd*(previous_error - error);
            previous_error = error;
            // vel.linear.x = 0.2;
            // System does not like when this if is entered
            if(PID > 1.0){
                vel.angular.z = 1.0;
            }
            else{
                vel.angular.z = PID;
            }
            ROS_INFO("vel.z command = %f", vel.angular.z);
//            ROS_INFO("previous_error = %f", previous_error);
            vpub.publish(vel);

            ros::spinOnce();
            r.sleep();

        }
         else{
            //  ROS_INFO("entered left turn");
        //     cout<<straight<<endl;
             ros::Rate lrate(3.0);
            do{
                vel.linear.x = 0.5;
                vel.angular.z = 1.0;
                //lrate.sleep();
                vpub.publish(vel);
                //ROS_INFO("laser range 719 = %f",laser.ranges[719]);
                ROS_INFO("laser range 359 = %f",laser.ranges[359]);                
                ros::spinOnce();
                lrate.sleep();
            }while(laser.ranges[359] < 5.0);
            cout<<"end turn"<< endl;
            straight = true;
            lrate.sleep();
            
            
         }

// ORIGINAL CODE THAT WORKS WITH RATE OF 5.0
        // double PID = kp *error + kd*(previous_error - error);
        // previous_error = error;
        // vel.linear.x = 0.2;
        // vel.angular.z = PID;
        // ROS_INFO("PID Command = %f", PID);
        // ROS_INFO("previous_error = %f", previous_error);
        // vpub.publish(vel);
        

        // ros::spinOnce();
        // r.sleep();
    }
    cout<<"ended by user"<< endl;
    vel.linear.x = 0;
    vel.angular.z =0;
    vpub.publish(vel);
    r.sleep();
    


}

// extra comments
        // ROS_INFO("a = %f", a);
        // ROS_INFO("b ( actual distance from wall) = %f", b);
        // ROS_INFO("alpha = %f", alpha);
        // ROS_INFO("AB = %f", AB);
        // ROS_INFO("CD = %f", CD);
        // ROS_INFO("error = %f", error);