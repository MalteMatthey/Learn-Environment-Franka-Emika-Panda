import rospy
import sys
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown
from trajectory_msgs.msg import JointTrajectory, JointTrajectoryPoint

def check_joint_positions_executed(target_positions):
    # Initialize the MoveIt Commander
    roscpp_initialize(sys.argv)
    
    # Initialize the ROS-Node
    rospy.init_node('position_controller_eval', anonymous=True)
    
    # Initialize the MoveGroup
    group = MoveGroupCommander('panda_arm')
    
    # Check each target position
    for i, target_position in enumerate(target_positions):
        group.set_joint_value_target(target_position)
        plan = group.go(wait=True)
        group.stop()
        group.clear_pose_targets()
        
        current_joint_values = group.get_current_joint_values()
        if not are_joint_positions_close(current_joint_values, target_position):
            rospy.logwarn(f"Target position {i+1} not reached. Expected: {target_position}, but got: {current_joint_values}")
            return False
        rospy.sleep(1)
    
    return True

def are_joint_positions_close(current, target, tolerance=0.1):
    return all(abs(c - t) < tolerance for c, t in zip(current, target))

if __name__ == "__main__":
    target_positions = [
        [0.5, -0.5, 0.0, 0.0, 0.5, -0.5, 0.0],
        [0.6, -0.6, 0.2, 0.2, 0.5, -0.5, 0.2],
        [0.7, -0.2, -0.5, -0.2, -0.5, 0.7, -0.2]
    ]
    check_joint_positions_executed(target_positions)
    rospy.signal_shutdown("Evaluation completed")
    roscpp_shutdown()
