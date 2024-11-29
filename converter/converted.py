import rospy
from moveit_commander import PlanningSceneInterface, MoveGroupCommander
from moveit_msgs.msg import Grasp, PlaceLocation, CollisionObject
from geometry_msgs.msg import PoseStamped, Pose
from tf.transformations import quaternion_from_euler
from shape_msgs.msg import SolidPrimitive
import moveit_msgs
import shape_msgs
import geometry_msgs
import trajectory_msgs

def create_collision_objects(planning_scene_interface):
    # Tabelle 1
    table1 = moveit_msgs.msg.CollisionObject()
    table1.id = "table1"
    table1.header.frame_id = "panda_link0"

    primitive1 = shape_msgs.msg.SolidPrimitive()
    primitive1.type = shape_msgs.msg.SolidPrimitive.BOX
    primitive1.dimensions = [0.2, 0.4, 0.4]  # Länge, Breite, Höhe

    table1.primitives.append(primitive1)

    pose1 = geometry_msgs.msg.Pose()
    pose1.position.x = 0.5
    pose1.position.y = 0.0
    pose1.position.z = 0.2
    pose1.orientation.w = 1.0

    table1.primitive_poses.append(pose1)
    table1.operation = moveit_msgs.msg.CollisionObject.ADD

    # Füge Tabelle 1 hinzu
    planning_scene_interface.add_object(table1)

    # Tabelle 2
    table2 = moveit_msgs.msg.CollisionObject()
    table2.id = "table2"
    table2.header.frame_id = "panda_link0"

    primitive2 = shape_msgs.msg.SolidPrimitive()
    primitive2.type = shape_msgs.msg.SolidPrimitive.BOX
    primitive2.dimensions = [0.4, 0.2, 0.4]

    table2.primitives.append(primitive2)

    pose2 = geometry_msgs.msg.Pose()
    pose2.position.x = 0.0
    pose2.position.y = 0.5
    pose2.position.z = 0.2
    pose2.orientation.w = 1.0

    table2.primitive_poses.append(pose2)
    table2.operation = moveit_msgs.msg.CollisionObject.ADD

    # Füge Tabelle 2 hinzu
    planning_scene_interface.add_object(table2)

    # Zielobjekt
    target_object = moveit_msgs.msg.CollisionObject()
    target_object.id = "object"
    target_object.header.frame_id = "panda_link0"

    primitive3 = shape_msgs.msg.SolidPrimitive()
    primitive3.type = shape_msgs.msg.SolidPrimitive.BOX
    primitive3.dimensions = [0.02, 0.02, 0.2]

    target_object.primitives.append(primitive3)

    pose3 = geometry_msgs.msg.Pose()
    pose3.position.x = 0.5
    pose3.position.y = 0.0
    pose3.position.z = 0.5
    pose3.orientation.w = 1.0

    target_object.primitive_poses.append(pose3)
    target_object.operation = moveit_msgs.msg.CollisionObject.ADD

    # Füge Zielobjekt hinzu
    planning_scene_interface.add_object(target_object)

def open_gripper():
    """Define the gripper open position."""
    posture = {"joint_names": ["panda_finger_joint1", "panda_finger_joint2"],
               "points": [{"positions": [0.04, 0.04], "time_from_start": rospy.Duration(0.5)}]}
    return posture

def close_gripper():
    """Define the gripper closed position."""
    posture = {"joint_names": ["panda_finger_joint1", "panda_finger_joint2"],
               "points": [{"positions": [0.0, 0.0], "time_from_start": rospy.Duration(0.5)}]}
    return posture

def pick(move_group):
    # Erstelle ein Grasp-Objekt
    grasp = moveit_msgs.msg.Grasp()

    # Grasp Pose: Wo der Roboter das Objekt greifen soll
    grasp.grasp_pose.header.frame_id = "panda_link0"
    grasp.grasp_pose.pose.position.x = 0.5
    grasp.grasp_pose.pose.position.y = 0.0
    grasp.grasp_pose.pose.position.z = 0.5
    grasp.grasp_pose.pose.orientation.w = 1.0

    # Pre-Grasp Approach: Bewegung zum Greifen
    grasp.pre_grasp_approach.direction.header.frame_id = "panda_link0"
    grasp.pre_grasp_approach.direction.vector.z = -1.0  # Bewegung entlang der -z-Achse
    grasp.pre_grasp_approach.min_distance = 0.095
    grasp.pre_grasp_approach.desired_distance = 0.115

    # Post-Grasp Retreat: Bewegung nach dem Greifen
    grasp.post_grasp_retreat.direction.header.frame_id = "panda_link0"
    grasp.post_grasp_retreat.direction.vector.z = 1.0  # Bewegung entlang der +z-Achse
    grasp.post_grasp_retreat.min_distance = 0.1
    grasp.post_grasp_retreat.desired_distance = 0.25

    # Pre-Grasp Posture: Öffne den Greifer
    grasp.pre_grasp_posture.joint_names = ["panda_finger_joint1", "panda_finger_joint2"]
    grasp.pre_grasp_posture.points = [
        trajectory_msgs.msg.JointTrajectoryPoint(
            positions=[0.04, 0.04], time_from_start=rospy.Duration(0.5)
        )
    ]

    # Grasp Posture: Schließe den Greifer
    grasp.grasp_posture.joint_names = ["panda_finger_joint1", "panda_finger_joint2"]
    grasp.grasp_posture.points = [
        trajectory_msgs.msg.JointTrajectoryPoint(
            positions=[0.0, 0.0], time_from_start=rospy.Duration(0.5)
        )
    ]

    # Führe den Greifvorgang aus
    move_group.pick("object", [grasp])

def place(move_group):
    # Erstelle ein PlaceLocation-Objekt
    place_location = moveit_msgs.msg.PlaceLocation()

    # Setze die Zielpose für das Platzieren
    place_location.place_pose.header.frame_id = "panda_link0"
    place_location.place_pose.pose.position.x = 0.0
    place_location.place_pose.pose.position.y = 0.5
    place_location.place_pose.pose.position.z = 0.5
    place_location.place_pose.pose.orientation.w = 1.0

    # Pre-Place Approach: Bewegung vor dem Platzieren
    place_location.pre_place_approach.direction.header.frame_id = "panda_link0"
    place_location.pre_place_approach.direction.vector.z = -1.0  # Bewegung entlang der -z-Achse
    place_location.pre_place_approach.min_distance = 0.095
    place_location.pre_place_approach.desired_distance = 0.115

    # Post-Place Retreat: Bewegung nach dem Platzieren
    place_location.post_place_retreat.direction.header.frame_id = "panda_link0"
    place_location.post_place_retreat.direction.vector.y = -1.0  # Bewegung entlang der -y-Achse
    place_location.post_place_retreat.min_distance = 0.1
    place_location.post_place_retreat.desired_distance = 0.25

    # Post-Place Posture: Öffne den Greifer
    place_location.post_place_posture.joint_names = ["panda_finger_joint1", "panda_finger_joint2"]
    place_location.post_place_posture.points = [
        trajectory_msgs.msg.JointTrajectoryPoint(
            positions=[0.04, 0.04], time_from_start=rospy.Duration(0.5)
        )
    ]

    # Führe den Platzierungsvorgang aus
    move_group.place("object", [place_location])
    
if __name__ == "__main__":
    rospy.init_node("pick_place_tutorial", anonymous=True)
    planning_scene_interface = PlanningSceneInterface()
    move_group = MoveGroupCommander("panda_arm")
    move_group.set_planning_time(45.0)

    create_collision_objects(planning_scene_interface)
    rospy.sleep(2)

    pick(move_group)
    rospy.sleep(2)

    place(move_group)





