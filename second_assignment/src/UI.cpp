//declaration of needed libraries

#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "geometry_msgs/Twist.h"
#include "second_assignment/accelerate.h"

//declaration of the client client1
ros::ServiceClient client1;

//function GetCh is used to manage the input given by the user to increment or decrement the velocity, or
//to reset the robot position
char GetCh(){
	char ch;
	std::cout<<"press 'a' to accelerate and 'd' to decelerate, 'r' to reset robot position   ";
	std::cin>>ch;
	return ch;
}


//callback function for char used to send the request to change the velocity and reset the position
void ChCallback(const sensor_msgs::LaserScan::ConstPtr& msg){
	//declaration of an object accelerate wich is the custom service made 
	//to manage acceleration
	second_assignment::accelerate a_srv;
	//call at the function GetCh() wich returns the char given by the user
	char in = GetCh();
	//assignment at the member input of the request
	//the variable char in wich is the input given by the user
	a_srv.request.input = in;
	client1.waitForExistence();
	client1.call(a_srv);
}



int main(int argc, char **argv)
{
	ros::init(argc, argv, "UI");
	ros::NodeHandle nh;
	
	//assignment of client client1
	client1 = nh.serviceClient<second_assignment::accelerate>("/accelerate");
	
	//here is made the subscription to the topic /base_scan
	ros::Subscriber sub = nh.subscribe("/base_scan", 1, ChCallback);
	
	ros::spin();

	return 0;
}
