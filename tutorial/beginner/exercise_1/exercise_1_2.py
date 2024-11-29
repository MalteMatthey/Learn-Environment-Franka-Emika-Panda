#!/usr/bin/env python

import rospy
from trajectory_msgs.msg import JointTrajectory, JointTrajectoryPoint
from sensor_msgs.msg import JointState

#
# task: move the robot arm to a specific joint configuration while using rostopics
#

# Initialize the ROS node
rospy.init_node('exercise_1_2', anonymous=True)

# Create publisher for the correct controller topic
pub = rospy.Publisher('/effort_joint_trajectory_controller/command', JointTrajectory, queue_size=10)

# Create JointTrajectory message
joint_trajectory = JointTrajectory()
joint_trajectory.header.stamp = rospy.Time.now()
joint_trajectory.joint_names = ['panda_joint1', 'panda_joint2', 'panda_joint3', 'panda_joint4', 'panda_joint5', 'panda_joint6', 'panda_joint7']

# Get current joint values
current_joint_values = rospy.wait_for_message('/joint_states', JointState, timeout=5).position

# Create JointTrajectoryPoint for the desired joint positions
point = JointTrajectoryPoint()

# set the position to the current joint values
point.positions = list(current_joint_values)

# change joint 2 to the desired value
point.positions[1] = 0.5

# Make the movement last 2.5 seconds
point.time_from_start = rospy.Duration(2.5)  

# Add point to the trajectory
joint_trajectory.points.append(point)

rospy.sleep(1.0)  # Wait for initialization

pub.publish(joint_trajectory)

rospy.loginfo("Joint trajectory command published.")

# wait for the movement to finish
rospy.sleep(2.5)

rospy.signal_shutdown("Task completed")