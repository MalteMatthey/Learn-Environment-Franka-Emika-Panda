#!/usr/bin/env python

import sys
import rospy
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown


# 
# This task starts with showing the individual methods and understanding what they do. 
# And then the first movement is planned.
#

# intialize the MoveIt! commander with command-line arguments
roscpp_initialize(sys.argv)

# initialize the ros node for the communication with the robot
rospy.init_node('exercise_2_1', anonymous=True)

# create an instance of MoveGroupCommander for controlling the Panda arm
group = MoveGroupCommander("panda_arm")

current_pose = group.get_current_pose().pose

rospy.loginfo(f"Current End-Effector Pose: \nPosition: x={current_pose.position.x}, "
                  f"y={current_pose.position.y}, z={current_pose.position.z} \n"
                  f"Orientation: x={current_pose.orientation.x}, y={current_pose.orientation.y}, "
                  f"z={current_pose.orientation.z}, w={current_pose.orientation.w}")

target_pose = current_pose.copy()

target_pose.position.x = 0.7
target_pose.position.y += 0.4
target_pose.position.z += 0.9

group.set_pose_target(target_pose)

# execute the planned motion
group.go(wait=True)

# delete the ros node
rospy.signal_shutdown("Task completed")
roscpp_shutdown()