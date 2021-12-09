Research Track 1 second assignment
================================

The second assignment of research track 1 course is about the use of a robot simulator in ROS.
The tasks for this assignment are to move the robot in the environment (Monza circuit) without hitting the wall,
and to give the user the possibility to increase or decrease robot velocity.

* # **The environment (Monza circuit)**
<p align="center">
<img src="https://github.com/andreamanera/RTassignment2/blob/main/images/tracciato.jpg" width="600" height="500">
</p>

* # **The robot moving towards the environment**
the small blue dot that you see at the bottom of the image, running in the circuit, is the robot.
   
<p align="center">
<img src="https://github.com/andreamanera/RTassignment2/blob/main/images/tacciato%2Brobot.jpg" width="600" height="500">
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
to robots and other objects.  This file tells Stage everything about the environment, in this project the world is passed to Stage via a `.png`

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
   -`base_scan (sensor_msgs/LaserScan)`: scans from the laser model
* Publishing:
   - `cmd_vel (geometry_msgs/Twist)` topic, here is published robot velocity

In this node I have created some function:

* float Distance(float obs_dist[], int val_min, int val_max)

