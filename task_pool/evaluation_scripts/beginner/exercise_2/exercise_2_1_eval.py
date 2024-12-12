import sys
import math

import rospy
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown


rospy.loginfo("exercise_2_1_evaluation")

roscpp_initialize(sys.argv)
rospy.init_node('exercise_2_1_evaluation', anonymous=True)

group = MoveGroupCommander("panda_arm")

current_pose = group.get_current_pose().pose
expected_pose = [0.5, 0, 0.2, 0.5, 0.5, 0.5, 0.5]
error_margin = 0.01

current_values = [
    current_pose.position.x,
    current_pose.position.y,
    current_pose.position.z,
    current_pose.orientation.x,
    current_pose.orientation.y,
    current_pose.orientation.z,
    current_pose.orientation.w
]

for i, (current, expected) in enumerate(zip(current_values, expected_pose)):
    if abs(current - expected) > error_margin:
        
        roscpp_shutdown()

        print("false, Pose component {} is not in the correct position".format(i+1))
        sys.exit(0)

roscpp_shutdown()

print("true")