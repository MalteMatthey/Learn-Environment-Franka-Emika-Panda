import rospy
from moveit_commander import PlanningSceneInterface
from geometry_msgs.msg import PoseStamped

# Initialize the ROS node for verifying objects in the scene
rospy.init_node('verify_objects', anonymous=True)

# Create an interface for interacting with the planning scene
scene = PlanningSceneInterface()
rospy.sleep(2)  # Allow time for the planning scene interface to initialize


# Define the expected pose of the cylinder object
def expected_cylinder_pose():
    """
    Creates and returns the expected pose of the cylinder object.
    The pose is defined relative to the "panda_link0" frame with
    specific position and orientation values.

    Returns:
        PoseStamped: The expected pose of the cylinder.
    """
    pose = PoseStamped()
    pose.header.frame_id = "panda_link0"
    pose.pose.position.x = 2.0
    pose.pose.position.y = 0.0
    pose.pose.position.z = 4.0
    pose.pose.orientation.x = 0.0
    pose.pose.orientation.y = 0.7071
    pose.pose.orientation.z = 0.0
    pose.pose.orientation.w = 0.7071
    return pose


# Define the expected pose of box_1
def expected_box_1_pose():
    """
    Creates and returns the expected pose of box_1.
    The pose is defined relative to the "panda_link0" frame with
    specific position and orientation values.

    Returns:
        PoseStamped: The expected pose of box_1.
    """
    pose = PoseStamped()
    pose.header.frame_id = "panda_link0"
    pose.pose.position.x = 0.4
    pose.pose.position.y = 0.0
    pose.pose.position.z = 0.1
    pose.pose.orientation.x = 0.0
    pose.pose.orientation.y = 0.0
    pose.pose.orientation.z = 0.0
    pose.pose.orientation.w = 1.0
    return pose


# Define the expected pose of box_2
def expected_box_2_pose():
    """
    Creates and returns the expected pose of box_2.
    The pose is defined relative to the "panda_link0" frame with
    specific position and orientation values.

    Returns:
        PoseStamped: The expected pose of box_2.
    """
    pose = PoseStamped()
    pose.header.frame_id = "panda_link0"
    pose.pose.position.x = 1.0
    pose.pose.position.y = 1.0
    pose.pose.position.z = 1.0
    pose.pose.orientation.x = 0.0
    pose.pose.orientation.y = 0.0
    pose.pose.orientation.z = 0.0
    pose.pose.orientation.w = 1.0
    return pose


# Verify if an object in the scene matches the expected pose
def verify_object(object_name, expected_pose, tolerance=0.1):
    """
    Verifies whether an object exists in the scene and matches the
    expected pose within a specified tolerance.

    Args:
        object_name (str): The name of the object to verify.
        expected_pose (PoseStamped): The expected pose of the object.
        tolerance (float): The maximum allowed difference between the current
                           and expected pose values for a match.

    Returns:
        bool: True if the object exists and matches the expected pose; otherwise, False.
    """
    current_objects = scene.get_objects()  # Retrieve all objects currently in the scene

    if object_name not in current_objects:
        rospy.logerr(f"The object '{object_name}' was not found.")  # Log an error if the object is missing
        return False

    # Retrieve the current pose of the object from the scene
    current_pose = current_objects[object_name].pose

    # Compare the object's position with the expected position
    position_match = (abs(current_pose.position.x - expected_pose.pose.position.x) < tolerance and
                      abs(current_pose.position.y - expected_pose.pose.position.y) < tolerance and
                      abs(current_pose.position.z - expected_pose.pose.position.z) < tolerance)

    # Compare the object's orientation with the expected orientation
    orientation_match = (abs(current_pose.orientation.x - expected_pose.pose.orientation.x) < tolerance and
                         abs(current_pose.orientation.y - expected_pose.pose.orientation.y) < tolerance and
                         abs(current_pose.orientation.z - expected_pose.pose.orientation.z) < tolerance and
                         abs(current_pose.orientation.w - expected_pose.pose.orientation.w) < tolerance)

    # Log results and return whether both position and orientation match
    if position_match and orientation_match:
        rospy.loginfo(f"The object '{object_name}' is in the correct position and orientation.")
        return True
    elif not position_match:
        rospy.logerr(f"The object '{object_name}' is NOT in the correct position.")
        return False
    else:
        rospy.logerr(f"The object '{object_name}' is NOT in the correct orientation.")
        return False


# Verify the positions and orientations of the expected objects
cylinder_pose = expected_cylinder_pose()
box_1_pose = expected_box_1_pose()
box_2_pose = expected_box_2_pose()

# Check if the cylinder is correctly placed in the scene
verify_object("cylinder", cylinder_pose)

# Check if box_1 is correctly placed in the scene
verify_object("box_1", box_1_pose)

# Check if box_2 is correctly placed in the scene
verify_object("box_2", box_2_pose)

# Additional objects can be added for verification as needed
# Example:
# verify_object("another_object", expected_other_object_pose())