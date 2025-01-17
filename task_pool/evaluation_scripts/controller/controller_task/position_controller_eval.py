import rospy
import math
import sys
from moveit_commander import roscpp_shutdown, roscpp_initialize, MoveGroupCommander
# Target positions
target_positions = [math.pi / 2, -math.pi / 4, 0.0, -math.pi * 3 / 4, 0.0, math.pi / 2, math.pi / 4]

# Tolerance for position verification
tolerance = 0.1

# Initialize ROS node
rospy.init_node('position_controller_evaluation', anonymous=True)

# Initialize MoveIt Commander
roscpp_initialize(sys.argv)
group = MoveGroupCommander("panda_arm")

rospy.sleep(5)

# Get current joint states
current_positions = group.get_current_joint_values()

rospy.sleep(5)

# Verify joint positions
for i in range(len(target_positions)):
    if abs(current_positions[i] - target_positions[i]) > tolerance:
        print(f"Joint {i+1} is not in the target position.")
        print(current_positions)
        print(False)
        break
else:
    print("All joints are in the correct positions.")
    print(True)

# Shutdown the ROS node
rospy.signal_shutdown("Evaluation completed")
roscpp_shutdown()