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

    ros::Rate rate(2.0);
    while(ros::ok()){
        pidControl();
        ros::spinOnce();
        rate.sleep();

        return 0;
    }
    return 0;


}

void laserCallBack(const sensor_msgs::LaserScan::ConstPtr& scan){
    goal_dist = 4.0;
    // try to set ranges array equal to laser variable
     laser.ranges = scan->ranges;

     a = scan->ranges[119];
     b = scan->ranges[0];
    // theta is in radians
     theta = 30*(3.14/180); 

    //Logic from F1tenth website 
     alpha = atan((a*cos(theta) - b)/a*sin(theta));
    //Original distance robot to wall
     AB = b*cos(alpha);

     AC = 0.1;
     CD = AB + AC*sin(alpha);
    error = goal_dist - CD; 


    if((scan->ranges[359] < 8.0) && (scan->ranges[719] > 12.0) && (scan->ranges[719]<30)){ 

         ROS_INFO(" scan 719 value = %f", scan->ranges[719]);
         ROS_INFO(" scan 359 value = %f",laser.ranges[359]);  
        straight = false;
        
    }

}
void pidControl(){
    geometry_msgs::Twist vel;
    double kp = 3.0;
    double kd = 0.75;
    previous_error = 0;

    ros::Rate r(6.0);

    while(ros::ok()){
        ROS_INFO("is straight = %s",(straight)?"true":"false");

        if(straight){
            double PID = kp *error + kd*(previous_error - error);
            previous_error = error;
            if(PID > 0.5){
                PID = 0.5;
            }
            else if( PID < -0.5 ){
                PID = -0.5;
            }
             ROS_INFO("angular command = %f", vel.angular.z);
            
            vel.angular.z = PID;

            vel.linear.x = 0.2;
            vel.linear.y = 0.0;
            vel.linear.z = 0.0;

            vpub.publish(vel);  
            ros::spinOnce();
            r.sleep();

        }
         else{
              ROS_INFO("entered left turn");
             ros::Rate lrate(5.0);
            do{
                vel.angular.z = 0.5;
                vpub.publish(vel);              
                ros::spinOnce();
                lrate.sleep();
            }while((laser.ranges[359] < 8.0) && (laser.ranges[359] > 2.0));
            cout<<"end turn"<< endl;
            straight = true;
            lrate.sleep();
            r.sleep();
            
         }

    }
    cout<<"ended by user"<< endl;
    vel.linear.x = 0;
    vel.angular.z =0;
    vpub.publish(vel);
    r.sleep();
    


}
