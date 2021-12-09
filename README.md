Research Track 1 second assignment
================================

The second assignment of research track 1 course is about the use of a robot simulator in ROS.
The tasks for this assignment are to move the robot in the environment (Monza circuit) without hitting the wall,
and to give the user the possibility to increase or decrease robot velocity.

* # **The environment (Monza circuit)**
<img src="https://github.com/andreamanera/RTassignment2/blob/main/images/tracciato.jpg" width="600" height="500">


* # **The robot moving towards the environment**

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

