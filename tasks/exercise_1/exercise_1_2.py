#!/usr/bin/env python

import rospy
from trajectory_msgs.msg import JointTrajectory, JointTrajectoryPoint
from sensor_msgs.msg import JointState

#
# move robot directly with rostopics
#

rospy.init_node('exercise_1_2', anonymous=True)

# Create publisher for the correct controller topic
pub = rospy.Publisher('/effort_joint_trajectory_controller/command', JointTrajectory, queue_size=10)

# Create JointTrajectory message
joint_trajectory = JointTrajectory()
joint_trajectory.header.stamp = rospy.Time.now()
joint_trajectory.joint_names = ['panda_joint1', 'panda_joint2', 'panda_joint3', 'panda_joint4', 'panda_joint5', 'panda_joint6', 'panda_joint7']

# Create JointTrajectoryPoint for the desired joint positions
# Get current joint values
current_joint_values = rospy.wait_for_message('/joint_states', JointState, timeout=5).position

rospy.loginfo("Current joint values: %s", current_joint_values)

# Create JointTrajectoryPoint for the desired joint positions
point = JointTrajectoryPoint()
point.positions = list(current_joint_values)
point.positions[1] = 0.5  # Change joint 2 to the desired value
point.time_from_start = rospy.Duration(2.5)  # Make the movement last 2.5 seconds

# Add point to the trajectory
joint_trajectory.points.append(point)

rospy.sleep(1.0)  # Wait for initialization

rospy.loginfo("Publishing joint trajectory command.")
pub.publish(joint_trajectory)
rospy.loginfo("Joint trajectory command published.")

rospy.sleep(2.5)  # Wait for the movement to finish

rospy.signal_shutdown("Task completed")