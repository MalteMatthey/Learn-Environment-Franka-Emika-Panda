# Introduction

### Goal of the tutorial
This tutorial introduces the Franka Emika Panda robot arm and provides a hands-on guide to understanding its systems. It focuses on learning how to control the robot using the Python API provided by its software. In the end, you'll gain theoretical and practical skills in working with robotic applications.

### Target group of the tutorial
This tutorial is designed for students and assistants working on projects involving the Panda robot arm, as well as anyone curious about robotic manipulation, regardless of prior experience.

### General structure of the tutorial
The tutorial progresses through various topics and difficulty levels, so you can start exactly where you want to. There is a Jupyter Notebook for each task you must complete. When you start your script, there is always an automatic evaluation at the end to see if and where you went wrong. In addition, we always provide a solution Jupyter Notebook if you get stuck on a task.

# Systems

There are several systems working together in our project. Here is a brief overview of what they are and what they do.

### ROS
ROS (Robot Operating System) is a set of software libraries and tools that can be used to create robot applications. Through this tutorial, you’ll explore the main features of ROS and its integration with the Panda robot arm.

### Rviz
Rviz (ROS Visualization) is a 3D visualization tool for ROS that is used for debugging and understanding robot data and states. It allows the display of sensor data, such as camera images, and the visualization of robot models and trajectories. In our case, Rviz will show us the robot arm and the trajectory of its movement. Rviz itself also offers its own plugins that can help when working with the robot. Our tutorial is also a Rviz plugin.

### MoveIt
MoveIt is an advanced robotics manipulation framework in ROS designed for motion planning, control and simulation of robotic arms. It provides tools for kinematics, collision detection, trajectory generation and task execution, making it indispensable for creating complex robotic applications. MoveIt will be an important part of the tutorial. Therefore, we will introduce you to MoveIt within the tasks.

### Gazebo
Gazebo is a simulation environment in which robot motions and systems can be tested virtually. In our case, the Franka Emika Panda robot arm will be simulated within Gazebo. Since not everyone always has the Panda robot arm available, we have created our tutorial for the Gazebo simulator. Nevertheless, you can of course also follow the tutorial with the real robot.

### Docker
Docker is a containerization platform that allows you to package the tutorial’s applications and dependencies into lightweight, portable containers. It enables cross-platform accessibility, although certain features may require Linux.

# Getting Started

If you have built and run the Docker environment correctly, you should end up in a folder with a [GETTING_STARTED.md](GETTING_STARTED.md) file. This file describes exactly how to start the tutorial from there. There is also a FAQ below with the most frequently asked questions about the project. In case you want to further develop the tutorial yourself, for example by adding new tasks, we have a `developer_docs` folder where you can find all the important information.