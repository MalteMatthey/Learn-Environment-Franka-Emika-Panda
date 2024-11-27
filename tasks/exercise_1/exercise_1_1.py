#!/usr/bin/env python

import sys
import rospy
import math
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown


# 
# This task starts with showing the individual methods and understanding what they do. 
# And then the first movement is planned.
#

# intialize the MoveIt! commander with command-line arguments
roscpp_initialize(sys.argv)

# initialize the ros node for the communication with the robot
rospy.init_node('exercise_1_1', anonymous=True)

# create an instance of MoveGroupCommander for controlling the Panda arm
group = MoveGroupCommander("panda_arm")

# set the planner ID to 'RRTConnectkConfigDefault'
group.set_planner_id("RRTConnectkConfigDefault")

# set the number of planning attempts to 10
group.set_num_planning_attempts(10)

# define a target joint values
# change to 160 degrees
joint_goal = group.get_current_joint_values()
joint_goal[4] = math.pi / 180 * 155  # joint 5

# sets the target joint values
group.set_joint_value_target(joint_goal)

# execute the planned motion
group.go(wait=True)

# delete the ros node
rospy.signal_shutdown("Task completed")
roscpp_shutdown()