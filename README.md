Research Track 1 second assignment
================================

The second assignment of research track 1 course is about the use of a robot simulator in ROS.
The tasks for this assignment are to move the robot in the environment (Monza circuit) without hitting the wall,
and to give the user the possibility to increase or decrease robot velocity.


* # **The robot moving towards the environment**
the small blue dot that you see at the bottom of the image, running in the circuit, is the robot.
   
<p align="center">
<img src="https://github.com/andreamanera/RTassignment2/blob/main/images/tacciato%2Brobot.jpg" width="450" height="350">
</p>

Installing and running
----------------------

The simulator works on [ROS noetic](http://wiki.ros.org/noetic), but probably also in other ROS version.
you can check  your ros version using the following command on your terminal:

``` bash
$ rosversion -d
```

Once you have installed ROS and created your workspace you have to download the package `second_assignment` in
the `src` folder of your workspace.
Then you have to run:

``` bash
$ catkin_make
```
**IMPORTANT:** be sure to run the command above in the root folder of your workspace

After doing this you are ready to run your program, thanks to the launch file you can simply run all the nodes
using the command:

``` bash
$ roslaunch second_assignment run.launch
```

Code development
----------------

To complete the requests I decided to implement two nodes, in addition to the one regarding the stage_ros simulator.
Furthermore to manage the change in speed I have implemented a custom service.

### stageros node
The stageros node wraps the Stage 2-D multi-robot simulator, via libstage. Stage simulates a world as defined in .world file.
This file tells stage everything about the world, from obstacles (usually represented via a bitmap to be used as a kind of background),
to robots and other objects. In this project the world is passed to Stage via a `.png`

* Subscription:
   - `cmd_vel (geometry_msgs/Twist)` topic, to manage the velocity of the robot
* Publishing:
   - `odom (nav_msgs/Odometry)`: odometry data of the model 
   - `base_scan (sensor_msgs/LaserScan)`: scans from the laser model
   - `base_pose_ground_truth (nav_msgs/Odometry)`: ground truth pose
   - `image (sensor_msgs/Image)`: visual camera image
   - `depth (sensor_msgs/Image)`: depth camera image
   - `camera_info (sensor_msgs/CameraInfo)`: info for camera calibration 

### controller node
The controller node has the task of moving the robot in the circuit, but it is also the handler of the inputs coming from the UI node.
* Subscription:
   - `base_scan (sensor_msgs/LaserScan)`: scans from the laser model
* Publishing:
   - `cmd_vel (geometry_msgs/Twist)` topic, here is published robot velocity

In this node I have created some function:

* float Distance(float obs_dist[], int val_min, int val_max)

__arguments:__

- `obs_dist[]`: array wich contains 721 elements wich are the distance of the obstacles from the robot
- `val_min`: minimum index of the subsection of the array that we want to analyze
- `val_max`: maximum index of the subsection of the array that we want to analyze

__return:__

- `dist_min`: minimum distance from an obstacle in the subsection of the array

Thanks to the subscription to the `base_scan` topic is possible to use the ranges vector to see robot distance from walls, 
the robot can see with a field of view of 180° in front of him, this field (given in radiants) is divided in 721 section.
Thanks a for loop and an if statement is possible to find the smallest value in each range.

* void robotCallback(const sensor_msgs::LaserScan::ConstPtr& msg)

This callback function is called whenever a message is posted on the topic `base_scan`.
Here the velocity is published on the topic `cmd_vel` and, after the division of the array ranges in subsections wich represent robot right,
left and front, is called the function `Distance(...)`.

```cpp
dist_min_right = Distance(scanner, 0, 100);
dist_min_left = Distance(scanner, 620, 720);
dist_min_front = Distance(scanner, 300, 420);
```
Then using a logic similar to that used in [assignment 1](https://github.com/andreamanera/RTassignment1) we decide if the
robot has to go stright, turn left or turn right.

```cpp
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
	else{
		my_vel.linear.x = 1.0+acceleration;
		my_vel.angular.z = 0.0;
	}
```

* bool serviceCallback (second_assignment::accelerate::Request &req, second_assignment::accelerate::Response &res)

Server to receive the client request from the UI node.

Here is where the user's keyboard input is received. some if statement. `a` key allows to accelerate, the `d` to decelerate and
the `r` to call the standard 'reset_position' service from the 'std_srvs' package: this tool made it very easy to reset the robot to its initial position.
When the user decides to accelerate, a global variable is incremented and added to the speed of th robot. The opposite happens for deceleration.
`x` is used just to avoid the gloabal variable to increment non-stop. 

```cpp
if(req.input == 'a'){
		acceleration += 0.5;
		req.input = 'x';
	}
	if(req.input == 'd'){
			acceleration -= 0.5;
			req.input = 'x';
	}
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
```
This function also creates the server's response to the client's request; in particular, the response consists of the float containing the value of acceleration (the value of
the global variable wich increment velocity).

### UI node

The UI node represent the user interface of the project. Thanks to this node is possible to modify the velocity of the robot and also to reset the position to the
initial state.

* Subscription:
   - `base_scan (sensor_msgs/LaserScan)`: scans from the laser model
* Publishing:
   - //
 
To change the speed and reset the position use the commands below:

<center>

| Commands | Result |
|:--------:|:----------:|
|__a__   |__Accelerate__|
|__d__   |__Decelerate__|
|__r__   |__Reset the position__|

</center>

you can manage the speed and reset the position thanks to the custom service `accelerate.srv`, located in the folder `srv`, the request of the service `char input` is sent
to the server, located in controller node, by the client (wich in this case is the UI node), in the server the request is received and the velocity is modified consequently.
Then the value of acceleration is assigned to service response (wich is float32 `acc`).

In UI node char input from keyboard is handled by the function:

* char GetCh()

```cpp
char GetCh(){
	char ch;
	std::cout<<"press 'a' to accelerate and 'd' to decelerate, 'r' to reset robot position   ";
	std::cin>>ch;
	return ch;
}
```
This function simply print a character request message and return the carachter given in input by the user.

Flow-chart
----------

running the command `rosrun rqt_graph rqt_graph` is possible to see relationship beetween all nodes:

<p align="center">
<img src="https://github.com/andreamanera/RTassignment2/blob/main/images/nodi%2Btopic.jpg" width="400" height="200">
</p>

I have also decided to to insert a graph wich explain the custom service behavior:

<p align="center">
<img src="https://github.com/andreamanera/RTassignment2/blob/main/images/service.jpg" width="400" height="200">
</p>

Theoretically, as you can see in the image above, I could have sent the value off acceleration back to the UI, pratically i decided
to not implement this feature in this case.

Conclusion and possible improvements
------------------------------------

I'm quite satisfied with the final result, I think that the robot behavior is correct and usually it makes more than one lap of the circuit.
The development of the code has been useful to learn how ROS works, and the logic behind it.
A possible improvement could concern the aesthetic part of the UI that maybe could make the user experience more enjoyable. Another improvement
concerns the behavior of the robot at high speed in fact it often happens that, if the speed is greatly increased, the robot does not works well
(even if this represents a real behavior).

**NOTE:** if you want to see a commented version of the code open code files in any code editor





