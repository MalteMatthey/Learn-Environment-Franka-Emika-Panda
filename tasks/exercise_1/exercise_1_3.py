#!/usr/bin/env python


import sys
import rospy
import math
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown
from trajectory_msgs.msg import JointTrajectory, JointTrajectoryPoint

#
# task: reset the robot arm to its initial joint configuration while using moveit commander or rostopics
# let the students decide
#

roscpp_initialize(sys.argv)

rospy.init_node('exercise_1_3', anonymous=True)

# create an instance of MoveGroupCommander for controlling the Panda arm
group = MoveGroupCommander("panda_arm")

# set the planner ID to 'RRTConnectkConfigDefault'
group.set_planner_id("RRTConnectkConfigDefault")

# set the number of planning attempts to 10
group.set_num_planning_attempts(10)

# define a target joint values
joint_goal = group.get_current_joint_values()
joint_goal[0] = 0.0  # joint 1
joint_goal[1] = -math.pi / 4  # joint 2
joint_goal[2] = 0.0  # joint 3
joint_goal[3] = -math.pi * 3 / 4  # joint 4
joint_goal[4] = 0.0  # joint 5
joint_goal[5] = math.pi / 2  # joint 6
joint_goal[6] = math.pi / 4  # joint 7

# sets the target joint values
group.set_joint_value_target(joint_goal)

# execute the planned motion
group.go(wait=True)

rospy.signal_shutdown("Task completed")
roscpp_shutdown()