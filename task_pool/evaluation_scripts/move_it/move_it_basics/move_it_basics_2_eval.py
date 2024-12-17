import sys

import rospy
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown


roscpp_initialize(sys.argv)
rospy.init_node('move_it_basics_2_eval', anonymous=True)

group = MoveGroupCommander("panda_arm")

current_pose = group.get_current_pose().pose

current_values = [
    current_pose.position.x,
    current_pose.position.y,
    current_pose.position.z,
    current_pose.orientation.x,
    current_pose.orientation.y,
    current_pose.orientation.z,
    current_pose.orientation.w
]

value_names = [
    "position.x",
    "position.y",
    "position.z",
    "orientation.x",
    "orientation.y",
    "orientation.z",
    "orientation.w"
]

expected_pose = [0.5, 0.0, 0.2, 0.5, 0.5, 0.5, 0.5]
error_margin = 0.01

for i, (current, expected) in enumerate(zip(current_values, expected_pose)):
    if abs(current - expected) > error_margin:
        
        roscpp_shutdown()

        print("Pose component \'{}\' is not in the correct position".format(value_names[i]))
        print("false")
        sys.exit(0)

roscpp_shutdown()

print("true")