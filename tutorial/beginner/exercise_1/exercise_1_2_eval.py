import sys
import math

import rospy
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown


rospy.loginfo("exercise_1_2_evaluation")

roscpp_initialize(sys.argv)
rospy.init_node('exercise_1_2_evaluation', anonymous=True)

group = MoveGroupCommander("panda_arm")

current_joint_values = group.get_current_joint_values()
expected_values = [math.pi * 150/180, math.pi/2, math.pi * 150/180, -math.pi * 6/180, math.pi * 150 / 180, math.pi * 206/180, math.pi * 150/180]
error_margin = 0.01

for i, (current, expected) in enumerate(zip(current_joint_values, expected_values)):
    if abs(current - expected) > error_margin:
        
        roscpp_shutdown()
        print("false, Joint {} is not in the correct position".format(i+1))
        sys.exit(0)

roscpp_shutdown()

print("true")
