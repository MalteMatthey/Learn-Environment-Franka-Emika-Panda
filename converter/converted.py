##### DO NOT CHANGE #####

import rospy
import sys
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown
from geometry_msgs.msg import Pose

# Initialize the MoveIt! commander
roscpp_initialize(sys.argv)

# Initialize the ROS node
rospy.init_node('exercise_4_2', anonymous=True)

# Initialize MoveGroupCommander
group = MoveGroupCommander('panda_arm')

##### DO NOT CHANGE #####

# Select the desired planning algorithm
group.set_planner_id("RRTConnectkConfigDefault")

target_pose = Pose()
target_pose.position.x = 0.1
target_pose.position.y = 0.1
target_pose.position.z = 0.1
target_pose.orientation.w = 1.0

##### DO NOT CHANGE #####

plan = group.go(wait=True)
group.stop()
group.clear_pose_targets()

if plan:
    rospy.loginfo("Bewegung erfolgreich geplant und ausgeführt!")
else:
    rospy.logwarn("Planung fehlgeschlagen. Überprüfe die Konfiguration.")

##### DO NOT CHANGE #####

##### DO NOT CHANGE #####

# Shutdown the ROS node
rospy.signal_shutdown("Task completed")
roscpp_shutdown()


##### DO NOT CHANGE #####
