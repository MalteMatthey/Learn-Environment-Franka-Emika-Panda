#!/usr/bin/env python

import sys
import rospy
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown
from moveit_msgs.msg import DisplayTrajectory
from geometry_msgs.msg import Pose
import time


# 
# This task starts with showing the individual methods and understanding what they do. 
# And then the first movement is planned.
#


def exercise0():

    # intialize the MoveIt! commander with command-line arguments
    roscpp_initialize(sys.argv)

    # initialize the ros node for the communication with the robot
    rospy.init_node('learn_env', anonymous=True)

    # create an instance of MoveGroupCommander for controlling the Panda arm
    group = MoveGroupCommander("panda_arm")

    # create a ros publisher for sending the planned trajecoey to the rviz
    display_trajectory_publisher = rospy.Publisher(
        "/move_group/display_planned_path", DisplayTrajectory, queue_size=10
    )

    # set the planner ID to 'RRTConnectkConfigDefault'
    group.set_planner_id("RRTConnectkConfigDefault")

    # set the number of planning attempts to 10
    group.set_num_planning_attempts(10)

    # define a target joint values
    joint_goal = group.get_current_joint_values()
    joint_goal[0] = -1.38089706712302  # joint 1
    joint_goal[1] = 1.7626998758091972  # joint 2
    joint_goal[2] = 1.774931698337621  # joint 3
    joint_goal[3] = -2.262299837416762  # joint 4
    joint_goal[4] = 1.5498787547996722  # joint 5
    joint_goal[5] = 1.8493953485899208  # joint 6
    joint_goal[6] = -0.8594281783760449  # joint 7

    # sets the target joint values
    group.set_joint_value_target(joint_goal)

    # execute the planned motion
    group.go(wait=True)

    current_joints = group.get_current_joint_values()
    rospy.loginfo("Current Joint Values: {}".format(current_joints))

    # delete the ros node
    rospy.signal_shutdown("Task completed")
    roscpp_shutdown()
    
    
if __name__ == '__main__':
    exercise0()
