#!/usr/bin/env python

import sys
import rospy
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown


# 
# task: convert the cartesian pose to joint values and move it with joint values
#

# intialize the MoveIt! commander with command-line arguments
roscpp_initialize(sys.argv)

# initialize the ros node for the communication with the robot
rospy.init_node('exercise_2_1', anonymous=True)

# create an instance of MoveGroupCommander for controlling the Panda arm
group = MoveGroupCommander("panda_arm")

group.set_max_velocity_scaling_factor(1.0)  # Set to 1.0 for maximum speed
group.set_max_acceleration_scaling_factor(1.0)

target_pose = group.get_current_pose().pose
target_pose.position.x = 0.5
target_pose.position.y = 0.1
target_pose.position.z = 0.2
target_pose.orientation.x = 0.5
target_pose.orientation.y = 0.5
target_pose.orientation.z = 0.5
target_pose.orientation.w = 0.5

group.set_pose_target(target_pose)

plan = group.plan()

if plan:
    joint_angles = plan[1].joint_trajectory.points[-1].positions
    group.set_joint_value_target(joint_angles)
    rospy.loginfo(f"Joint angles: {joint_angles}")
    group.go(wait=True)
    rospy.loginfo("pose target reached")
else:
    rospy.loginfo("No solution found!")

# delete the ros node
rospy.signal_shutdown("Task completed")
roscpp_shutdown()