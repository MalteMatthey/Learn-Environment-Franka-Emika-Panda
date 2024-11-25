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

    # define a target pose with the x y z w values
    target_pose = Pose()
    target_pose.position.x = 0.5
    target_pose.position.y = 0.0
    target_pose.position.z = 0.5
    target_pose.orientation.w = 0.5

    # sets the target pose
    group.set_pose_target(target_pose)

    # execute the planned motion
    group.go(wait=True)

    current_position = group.get_current_pose().pose
    rospy.loginfo("Current Pose: {}".format(current_position))

    # delete the ros node
    rospy.signal_shutdown("Task completed")
    roscpp_shutdown()
    
    
if __name__ == '__main__':
    exercise0()
