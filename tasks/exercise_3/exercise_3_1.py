#!/usr/bin/env python

import sys
import rospy
import math
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown


# 
# task: add a sequence of joint values to the MoveGroupCommander and execute it
#

# intialize the MoveIt! commander from the moveit_command package
roscpp_initialize(sys.argv)

# initialize the ros node for the communication with the robot
rospy.init_node('exercise_3_1', anonymous=True)

# create an instance of MoveGroupCommander for controlling the Panda arm
group = MoveGroupCommander("panda_arm")

# set the planner ID to 'RRTConnectkConfigDefault'
group.set_planner_id("RRTConnectkConfigDefault")

# set the number of planning attempts to 10
group.set_num_planning_attempts(10)

group.set_max_velocity_scaling_factor(1.0)  # Set to 1.0 for maximum speed
group.set_max_acceleration_scaling_factor(1.0)

joint_sequence = [
        [0.0, -0.785, 0.0, -2.356, 0.0, 1.571, 0.785],  # Home position
        [0.5, -0.5, 0.5, -1.5, 0.5, 1.0, 0.5],         # Position 1
        [1.0, -1.0, 1.0, -2.0, 1.0, 0.5, 1.0],         # Position 2
        [2.7, -1.6, 2.7, -0.2, 2.7, 0.5, 2.7],       # position 3
        [1.0, -1.0, 1.0, -2.0, 1.0, 0.5, 1.0],        # Position 2
        [0.5, -0.5, 0.5, -1.5, 0.5, 1.0, 0.5],         # Position 1
        [0.0, -0.785, 0.0, -2.356, 0.0, 1.571, 0.785]  # Home position
]

for joint_goal in joint_sequence:
    group.set_joint_value_target(joint_goal)
    group.go(wait=True)
    rospy.loginfo("goal reached")
    rospy.sleep(.5)

# delete the ros node
rospy.signal_shutdown("Task completed")
roscpp_shutdown()