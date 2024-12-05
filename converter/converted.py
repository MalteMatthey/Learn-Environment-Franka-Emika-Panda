##### DO NOT CHANGE #####

import sys
import rospy
from moveit_commander import roscpp_initialize, roscpp_shutdown, PlanningSceneInterface
from moveit_msgs.msg import CollisionObject
from shape_msgs.msg import SolidPrimitive
from geometry_msgs.msg import Pose

##### DO NOT CHANGE #####

##### DO NOT CHANGE #####

# Initialize the ROS-Node
rospy.init_node('moveit_commander_example', anonymous=True)

# Initialize the MoveIt Commander
roscpp_initialize(sys.argv)

# Initialize the PlanningSceneInterface
planning_scene_interface = PlanningSceneInterface()

# Wait for the planning scene interface to initialize
rospy.sleep(2)

##### DO NOT CHANGE #####

# rectangle_1
rectangle_1 = CollisionObject()
rectangle_1.id = "rectangle_1"
rectangle_1.header.frame_id = "panda_link0"

primitive1 = SolidPrimitive()
primitive1.type = SolidPrimitive.BOX
primitive1.dimensions = [0.2, 0.4, 0.4]

rectangle_1.primitives.append(primitive1)

pose1 = Pose()
pose1.position.x = 0.5
pose1.position.y = 0.0
pose1.position.z = 0.2
pose1.orientation.w = 1.0

rectangle_1.primitive_poses.append(pose1)
rectangle_1.operation = CollisionObject.ADD

# add rectangle_1 to the planning scene
planning_scene_interface.add_object(rectangle_1)

# rectangle_2
rectangle_2 = CollisionObject()
rectangle_2.id = "rectangle_2"
rectangle_2.header.frame_id = "panda_link0"

primitive2 = SolidPrimitive()
primitive2.type = SolidPrimitive.BOX
primitive2.dimensions = [0.4, 0.2, 0.4]

rectangle_2.primitives.append(primitive2)

pose2 = Pose()
pose2.position.x = 0.0
pose2.position.y = 0.5
pose2.position.z = 0.2
pose2.orientation.w = 1.0

rectangle_2.primitive_poses.append(pose2)
rectangle_2.operation = CollisionObject.ADD

# add rectangle1 to the planning scene
planning_scene_interface.add_object(rectangle_2)

# Zielobjekt
target_object = CollisionObject()
target_object.id = "object"
target_object.header.frame_id = "panda_link0"

primitive3 = SolidPrimitive()
primitive3.type = SolidPrimitive.BOX
primitive3.dimensions = [0.02, 0.02, 0.2]

target_object.primitives.append(primitive3)

pose3 = Pose()
pose3.position.x = 0.5
pose3.position.y = 0.0
pose3.position.z = 0.5
pose3.orientation.w = 1.0

target_object.primitive_poses.append(pose3)
target_object.operation = CollisionObject.ADD

# Füge Zielobjekt hinzu
planning_scene_interface.add_object(target_object)

##### DO NOT CHANGE #####

# Shutdown the ROS node
rospy.signal_shutdown("Task completed")
roscpp_shutdown()

##### DO NOT CHANGE #####
