import sys
import math

import rospy
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown


roscpp_initialize(sys.argv)
rospy.init_node('ros_intermediate_1_eval', anonymous=True)

group = MoveGroupCommander("panda_arm")

current_joint_values = group.get_current_joint_values()
expected_values = [0.0, -math.pi /4, 0.5 * math.pi, -math.pi * 3/4, math.pi * 3/4, math.pi/2, math.pi/4]
error_margin = 0.01

for i, (current, expected) in enumerate(zip(current_joint_values, expected_values)):
    if abs(current - expected) > error_margin:
        
        roscpp_shutdown()
        print("Joint {} is not in the correct position".format(i))
        print(False)
        sys.exit(0)

roscpp_shutdown()

print(True)
