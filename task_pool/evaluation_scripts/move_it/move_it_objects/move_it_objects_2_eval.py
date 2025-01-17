import rospy
from moveit_commander import PlanningSceneInterface, roscpp_shutdown, roscpp_initialize
from geometry_msgs.msg import PoseStamped
import sys

# Initialize the ROS node for verifying objects in the scene
roscpp_initialize(sys.argv)
rospy.init_node('verify_objects', anonymous=True)

# Create an interface for interacting with the planning scene
scene = PlanningSceneInterface()
rospy.sleep(2) 


# Define the expected pose of the target object
def expected_target_pose():
    pose = PoseStamped()
    pose.header.frame_id = "panda_link0"
    pose.pose.position.x = 0.5
    pose.pose.position.y = 0.0
    pose.pose.position.z = 0.5
    pose.pose.orientation.x = 0.0
    pose.pose.orientation.y = 0.0
    pose.pose.orientation.z = 0.0
    pose.pose.orientation.w = 1.0
    return pose


# Define the expected pose of rectangle_1
def expected_rectangle_1_pose():
    pose = PoseStamped()
    pose.header.frame_id = "panda_link0"
    pose.pose.position.x = 0.5
    pose.pose.position.y = 0.0
    pose.pose.position.z = 0.2
    pose.pose.orientation.x = 0.0
    pose.pose.orientation.y = 0.0
    pose.pose.orientation.z = 0.0
    pose.pose.orientation.w = 1.0
    return pose


# Define the expected pose of rectangle_2
def expected_rectangle_2_pose():
    pose = PoseStamped()
    pose.header.frame_id = "panda_link0"
    pose.pose.position.x = 0.0
    pose.pose.position.y = 0.5
    pose.pose.position.z = 0.2
    pose.pose.orientation.x = 0.0
    pose.pose.orientation.y = 0.0
    pose.pose.orientation.z = 0.0
    pose.pose.orientation.w = 1.0
    return pose


# Verify if an object in the scene matches the expected pose
def verify_object(object_name, expected_pose, tolerance=0.1):

    current_objects = scene.get_objects()

    if object_name not in current_objects:
        rospy.logerr(f"The object '{object_name}' was not found.")
        print(f"The object '{object_name}' was not found.")
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
        print(f"The object '{object_name}' is in the correct position and orientation.")
        return True
    elif not position_match:
        rospy.logerr(f"The object '{object_name}' is NOT in the correct position.")
        print(f"The object '{object_name}' is NOT in the correct position.")
        return False
    else:
        rospy.logerr(f"The object '{object_name}' is NOT in the correct orientation.")
        print(f"The object '{object_name}' is NOT in the correct orientation.")
        return False


# Verify the positions and orientations of the expected objects
target_pose = expected_target_pose()
rectangle_1_pose = expected_rectangle_1_pose()
rectangle_2_pose = expected_rectangle_2_pose()

# Check if the target is correctly placed in the scene
target_result = verify_object("target", target_pose)

# Check if rectangle_1 is correctly placed in the scene
rectangle_1_result = verify_object("rectangle_1", rectangle_1_pose)

# Check if rectangle_2 is correctly placed in the scene
rectangle_2_result = verify_object("rectangle_2", rectangle_2_pose)

# Print the final result
if target_result and rectangle_1_result and rectangle_2_result:
    print(True)
else:
    print(False)

rospy.signal_shutdown("Task completed")
roscpp_shutdown()

# Additional objects can be added for verification as needed
# Example:
# verify_object("another_object", expected_other_object_pose())