#!/usr/bin/env python

import sys
import rospy
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown


# 
# task: move the robot arm to a specific cartesian pose while using moveit commander
#

# intialize the MoveIt! commander with command-line arguments
roscpp_initialize(sys.argv)

# initialize the ros node for the communication with the robot
rospy.init_node('exercise_2_1', anonymous=True)

# create an instance of MoveGroupCommander for controlling the Panda arm
group = MoveGroupCommander("panda_arm")

# speed up the robot
group.set_max_velocity_scaling_factor(1.0) 
group.set_max_acceleration_scaling_factor(1.0)

# get the current pose of the end effector
target_pose = group.get_current_pose().pose

# important!! orientation must be: x^2 + y^2 + z^2 + w^2 = 1
# cause its a quanternion normalisation

target_pose.position.x = 0.5
target_pose.position.y = 0.5
target_pose.position.z = 0.5
target_pose.orientation.x = 0.5
target_pose.orientation.y = 0.5
target_pose.orientation.z = 0.5
target_pose.orientation.w = 0.5

group.set_pose_target(target_pose)

# execute the planned motion
group.go(wait=True)

rospy.loginfo("pose target reached")

# delete the ros node
rospy.signal_shutdown("Task completed")
roscpp_shutdown()