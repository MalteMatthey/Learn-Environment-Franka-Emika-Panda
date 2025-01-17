import sys
import math

import rospy
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown

roscpp_initialize(sys.argv)
rospy.init_node('exercise_4_1_evaluation', anonymous=True)

group = MoveGroupCommander("panda_arm")

current_pose = group.get_current_pose().pose
expected_pose = [0.5, 0, 0.6, 1, 0.0, 0.0, 0.0]
error_margin = 0.1

current_values = [
    current_pose.position.x,
    current_pose.position.y,
    current_pose.position.z,
    current_pose.orientation.w,
    current_pose.orientation.x,
    current_pose.orientation.y,
    current_pose.orientation.z
]

for i, (current, expected) in enumerate(zip(current_values, expected_pose)):
    if abs(current - expected) > error_margin:
        roscpp_shutdown()
        print("Orientation")
        print(current_pose.orientation.x,
              current_pose.orientation.y,
              current_pose.orientation.z,
              current_pose.orientation.w)
        print("false, Pose component {} is not in the correct position".format(i+1))
        print(False)

roscpp_shutdown()

print(True)