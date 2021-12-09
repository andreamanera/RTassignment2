//declaration of needed libraries

#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "geometry_msgs/Twist.h"
#include "second_assignment/accelerate.h"
#include "std_srvs/Empty.h"

// declaration of publisher pub
ros::Publisher pub;

float acceleration = 0.0;

//service Empty is used to reset robot position
std_srvs::Empty res_pos;

//Function used to find the closest obstacle in each section of the ranges array
float Distance(float obs_dist[], int val_min, int val_max){

	//as the minimum distance is set a large value that will then be compared
	//with all the values between val_min and val_max in the ranges array
	
	float dist_min = 30.0;
	
	//thanks to the for loop and the if statement all values in the ranges
	//array are compared so as to find the smallest
	
	for(int i = val_min; i <= val_max; i++){
	
		if(obs_dist[i] <= dist_min){
		
			dist_min = obs_dist[i];
		}
	}
	
	return dist_min;
}

//Declaration of the callback function 

void robotCallback(const sensor_msgs::LaserScan::ConstPtr& msg){
	
	geometry_msgs::Twist my_vel;
	
	//variables for the closest obstacle on robot right, left and front
	float dist_min_right, dist_min_left, dist_min_front;
	
	// array wich will be filled with the elements of ranges
	float scanner[721];
	
	// thanks to this loop scanner array is filled with the element of ranges
	for(int i = 0; i < 722; i++){
		scanner[i] = msg->ranges[i];
	}
	
	// Call at the Distance function to calculate the minimum
	// distance from the obstacles on robot front, right and left
	dist_min_right = Distance(scanner, 0, 100);
	dist_min_left = Distance(scanner, 620, 720);
	dist_min_front = Distance(scanner, 300, 420);
	
	// As in the first assignment here is checked if we are close
	// to an obstacle on robot front and if so, we check if we are close
	// on the right or on the left, in order to turn the robot in the right
	// way
	if(dist_min_front < 1.5){
		if(dist_min_left > dist_min_right){
			my_vel.angular.z = 1.0;
			my_vel.linear.x = 0.3;
		}
		else if(dist_min_right > dist_min_left){
			my_vel.angular.z = -1.0;
			my_vel.linear.x = 0.3;
		}
	}
	
	// If there are no obstacles on robot front it continues straight
	else{
		my_vel.linear.x = 1.0+acceleration;
		my_vel.angular.z = 0.0;
	}
	if(my_vel.linear.x <= 0){
		my_vel.linear.x = 0.0;
	}
		
	// here the velocity is published on 
		pub.publish(my_vel);
}

// service callback funcion used to manage velocity variations
bool serviceCallback (second_assignment::accelerate::Request &req, second_assignment::accelerate::Response &res){
	// if the request sent from the user is 'a' the velocity is incremented
	if(req.input == 'a'){
		acceleration += 0.5;
		req.input = 'x';
	}
	//if the request sent from the user is 'd' the velocity is incremented
	if(req.input == 'd'){
			acceleration -= 0.5;
			req.input = 'x';
	}
	//if the request sent from the user is 'r' robot goes back to the initial position
	if(req.input == 'r'){
		ros::service::call("/reset_positions", res_pos);
	}
	if(req.input == 'x'){
		return false;
	}
	if(req.input != 'a' && req.input != 'd' && req.input != 'r' && req.input != 'x'){
		std::cout<<"wrong key"<<std::endl;
		fflush(stdout);
	}
	 
	res.acc = acceleration;
	return true;
}

int main (int argc, char **argv)
{
	// Initialize the node, setup the NodeHandle for handling the communication 
	// with the ROS system
	ros::init(argc, argv, "controller");
	ros::NodeHandle nh;
	
	//publishing on the topic /cmd_vel
	pub = nh.advertise<geometry_msgs::Twist> ("/cmd_vel", 1);
	//subsciption to the topic /base_scan 
	ros::Subscriber sub = nh.subscribe("/base_scan", 1, robotCallback);
	
	ros::ServiceServer service =  nh.advertiseService("/accelerate", serviceCallback);
	
	ros::spin();
	
	return 0;
}
	

