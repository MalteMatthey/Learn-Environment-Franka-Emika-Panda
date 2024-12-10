import rospy
import sys
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown
from geometry_msgs.msg import Pose

def move_through_waypoints():
    # Initialize MoveIt! commander and ROS node
    roscpp_initialize(sys.argv)
    rospy.init_node('exercise_7', anonymous=True)

    # Initialize the MoveGroup
    arm_group = MoveGroupCommander('manipulator')

    # Get the current pose of the end-effector
    current_pose = arm_group.get_current_pose().pose

    # Define the waypoints
    waypoints = []

    # Waypoint 1: Slightly move along the X-axis
    waypoint_1 = Pose()
    waypoint_1.position.x = current_pose.position.x + 0.1
    waypoint_1.position.y = current_pose.position.y
    waypoint_1.position.z = current_pose.position.z
    waypoint_1.orientation = current_pose.orientation
    waypoints.append(waypoint_1)

    # Waypoint 2: Move along the Y-axis
    waypoint_2 = Pose()
    waypoint_2.position.x = waypoint_1.position.x
    waypoint_2.position.y = waypoint_1.position.y + 0.1
    waypoint_2.position.z = waypoint_1.position.z
    waypoint_2.orientation = current_pose.orientation
    waypoints.append(waypoint_2)

    # Waypoint 3: Move upwards along the Z-axis
    waypoint_3 = Pose()
    waypoint_3.position.x = waypoint_2.position.x
    waypoint_3.position.y = waypoint_2.position.y
    waypoint_3.position.z = waypoint_2.position.z + 0.1
    waypoint_3.orientation = current_pose.orientation
    waypoints.append(waypoint_3)

    # Compute the Cartesian path
    fraction = 0.0
    max_attempts = 100
    attempts = 0

    while fraction < 1.0 and attempts < max_attempts:
        (plan, fraction) = arm_group.compute_cartesian_path(waypoints, 0.01, 0.0)  # Plan the trajectory
        attempts += 1

    if fraction == 1.0:
        rospy.loginfo("Successfully computed the Cartesian path. Executing...")
        arm_group.execute(plan, wait=True)
        rospy.loginfo("Execution completed.")
    else:
        rospy.logwarn(f"Failed to compute a complete path after {attempts} attempts.")

    # Cleanup
    rospy.signal_shutdown("Task completed")
    roscpp_shutdown()

if __name__ == '__main__':
    try:
        move_through_waypoints()
    except rospy.ROSInterruptException:
        pass