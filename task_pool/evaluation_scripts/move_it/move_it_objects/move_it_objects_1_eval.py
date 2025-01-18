import rospy
from moveit_commander import PlanningSceneInterface, roscpp_initialize, roscpp_shutdown
from geometry_msgs.msg import PoseStamped
import sys

# Initialize the ROS node for verifying objects in the scene
roscpp_initialize(sys.argv)
rospy.init_node('verify_objects', anonymous=True)

# Create an interface for interacting with the planning scene
scene = PlanningSceneInterface()
rospy.sleep(2)


# Define the expected pose of the cylinder object
def expected_cylinder_pose():
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


# Verify if an object in the scene matches the expected pose and size
def verify_object(object_name, expected_pose, expected_size, tolerance=0.1):
    current_objects = scene.get_objects()

    if object_name not in current_objects:
        print(f"The object '{object_name}' was not found.")
        return False

    # Retrieve the current pose and size of the object from the scene
    current_object = current_objects[object_name]
    current_pose = current_object.pose
    current_size = current_object.primitives[0].dimensions

    # Compare the object's position with the expected position
    position_match = (abs(current_pose.position.x - expected_pose.pose.position.x) < tolerance and
                      abs(current_pose.position.y - expected_pose.pose.position.y) < tolerance and
                      abs(current_pose.position.z - expected_pose.pose.position.z) < tolerance)

    # Compare the object's orientation with the expected orientation
    orientation_match = (abs(current_pose.orientation.x - expected_pose.pose.orientation.x) < tolerance and
                         abs(current_pose.orientation.y - expected_pose.pose.orientation.y) < tolerance and
                         abs(current_pose.orientation.z - expected_pose.pose.orientation.z) < tolerance and
                         abs(current_pose.orientation.w - expected_pose.pose.orientation.w) < tolerance)

    # Compare the object's size with the expected size
    size_match = all(abs(current_size[i] - expected_size[i]) < tolerance for i in range(len(expected_size)))

    # Log results and return whether both position, orientation, and size match
    if position_match and orientation_match and size_match:
        print(f"The object '{object_name}' is in the correct position, orientation, and size.")
        return True
    elif not position_match:
        print(f"The object '{object_name}' is NOT in the correct position.")
        return False
    elif not orientation_match:
        print(f"The object '{object_name}' is NOT in the correct orientation.")
        return False
    else:
        print(f"The object '{object_name}' is NOT the correct size.")
        return False

# Define the expected size of the objects
expected_cylinder_size = [1.0, 0.2]
expected_box_1_size = [0.05, 0.05, 0.05]
expected_box_2_size = [0.1, 0.1, 0.1]

# Verify the positions, orientations, and sizes of the expected objects
cylinder_pose = expected_cylinder_pose()
box_1_pose = expected_box_1_pose()
box_2_pose = expected_box_2_pose()

# Check if the cylinder is correctly placed in the scene
cylinder_result = verify_object("cylinder", cylinder_pose, expected_cylinder_size)

# Check if box_1 is correctly placed in the scene
box_1_result = verify_object("box_1", box_1_pose, expected_box_1_size)

# Check if box_2 is correctly placed in the scene
box_2_result = verify_object("box_2", box_2_pose, expected_box_2_size)

if cylinder_result and box_1_result and box_2_result:
    print(True)
else:
    print(False)

rospy.signal_shutdown("Task completed")
roscpp_shutdown()

# Additional objects can be added for verification as needed
# Example:
# verify_object("another_object", expected_other_object_pose())