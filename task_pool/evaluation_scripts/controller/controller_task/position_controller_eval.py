import rospy
from sensor_msgs.msg import JointState
import math
import moveit_commander
import sys

# Target positions
target_positions = [math.pi / 2
, -math.pi / 4, 0.0, -math.pi * 3 / 4, 0.0, math.pi / 2, math.pi / 4]

# Tolerance for position verification
tolerance = 0.1

# Initialize ROS node
rospy.init_node('position_controller_evaluation', anonymous=True)

# Initialize MoveIt Commander
moveit_commander.roscpp_initialize(sys.argv)
group = moveit_commander.MoveGroupCommander("panda_arm")

# Wait until joint states are received
rospy.sleep(5)

# Get current joint states
current_positions = group.get_current_joint_values()

# Wait until joint states are received
rospy.sleep(5)

# Verify joint positions
if 'current_positions' in globals() and len(current_positions) == len(target_positions):
    for i in range(len(target_positions)):
        if abs(current_positions[i] - target_positions[i]) > tolerance:
            rospy.loginfo(f"Joint {i+1} is not in the target position.")
            rospy.loginfo(current_positions)
            break
    else:
        rospy.loginfo("All joints are in the target positions.")
else:
    rospy.loginfo("Joint states not received or length mismatch.")

# Shutdown the ROS node
rospy.signal_shutdown("Evaluation completed")