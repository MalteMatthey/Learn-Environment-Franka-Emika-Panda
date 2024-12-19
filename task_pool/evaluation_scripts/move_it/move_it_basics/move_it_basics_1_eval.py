import rospy
import math
import sys

from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown


roscpp_initialize(sys.argv)
rospy.init_node('move_it_basics_1_eval', anonymous=True)

group = MoveGroupCommander("panda_arm")
group.set_planner_id("RRTConnectkConfigDefault")
group.set_num_planning_attempts(10)

current_joint_values = group.get_current_joint_values()

expected_values = [0, -math.pi/4, 0, -math.pi *3/4, math.pi * 50/180, math.pi/2, math.pi/4]

error_margin = 0.01

for i, (current, expected) in enumerate(zip(current_joint_values, expected_values)):
    if abs(current - expected) > error_margin:
        
        roscpp_shutdown()
        print('Joint {} is not in the correct position'.format(i))
        print('false')
        sys.exit(0)

roscpp_shutdown()

print("true")
