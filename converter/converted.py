##### DO NOT CHANGE #####

import sys
import math

import rospy
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown

##### DO NOT CHANGE #####

##### DO NOT CHANGE #####

roscpp_initialize(sys.argv)
rospy.init_node('exercise_1_1', anonymous=True)

##### DO NOT CHANGE #####

##### DO NOT CHANGE #####

group = MoveGroupCommander("panda_arm")
group.set_planner_id("RRTConnectkConfigDefault")
group.set_num_planning_attempts(10)

##### DO NOT CHANGE #####

joint_goal = group.get_current_joint_values()
joint_goal[4] = (155 * math.pi) / 180

group.set_joint_value_target(joint_goal)
group.go(wait=True)

##### DO NOT CHANGE #####

rospy.signal_shutdown("Task completed")
roscpp_shutdown()

##### DO NOT CHANGE #####
