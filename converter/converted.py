from moveit_commander import MoveGroupCommander, PlanningSceneInterface, roscpp_initialize, roscpp_shutdown
from geometry_msgs.msg import Pose, PoseStamped
import rospy
import sys

"""
Ziel :
Einen Roboterarm programmieren, um ein Objekt zu greifen.
Ziel ist es, die grundlegenden Konzepte des Greifens
mit einem Roboterarm zu erlernen, einschließlich der Positionierung
des Endeffektors und der Steuerung des Greifers.
"""

# ROS und MoveIt! initialisieren
roscpp_initialize(sys.argv)
rospy.init_node('exercise_5_2_with_scene', anonymous=True)

# MoveGroup und SceneInterface initialisieren
arm_group = MoveGroupCommander('panda_arm')
gripper_group = MoveGroupCommander('panda_hand')  # Greifer-Gruppe hinzufügen
scene = PlanningSceneInterface()

# Planungszeit erhöhen und Replanning aktivieren
arm_group.set_planning_time(60)  # Erhöhen Sie die Planungszeit auf 15 Sekunden
arm_group.allow_replanning(True)  # Erlaubt alternative Wege bei Planungsfehlern
arm_group.set_max_velocity_scaling_factor(0.5)  # Geschwindigkeit begrenzen
arm_group.set_max_acceleration_scaling_factor(0.5)  # Beschleunigung begrenzen

# Planer wechseln (falls erforderlich)
arm_group.set_planner_id("RRTConnect")  # Alternativer Planer (z. B. PRM, RRTConnect, RRTstar)

# Zielobjekt direkt vor dem Roboter positionieren (näher)
box_pose = PoseStamped()
box_pose.header.frame_id = "panda_link0"
box_pose.pose.position.x = 0.2  # Näher am Roboter
box_pose.pose.position.y = 0.0
box_pose.pose.position.z = 0.1
box_pose.pose.orientation.w = 1.0

scene.add_box("target_object", box_pose, size=(0.05, 0.05, 0.05))
rospy.loginfo("Objekt zur Szene hinzugefügt.")
rospy.sleep(2)

# Öffne den Greifer
gripper_group.set_named_target("open")
gripper_group.go(wait=True)
rospy.loginfo("Gripper geöffnet.")

# Setze den Arm in eine Startposition, die näher am Objekt ist
start_pose = Pose()
start_pose.position.x = 0.2  # Gleicher X-Wert wie das Zielobjekt
start_pose.position.y = 0.0
start_pose.position.z = 0.3  # Höher als das Zielobjekt
start_pose.orientation.w = 1.0

arm_group.set_pose_target(start_pose)
start_plan = arm_group.go(wait=True)
arm_group.stop()
arm_group.clear_pose_targets()
rospy.loginfo("Arm in Startposition.")

# Positioniere den Arm über dem Zielobjekt
target_pose = Pose()
target_pose.position.x = 0.2  # Gleicher X-Wert wie das Zielobjekt
target_pose.position.y = 0.0
target_pose.position.z = 0.2  # Direkt über dem Zielobjekt
target_pose.orientation.w = 1.0

arm_group.set_pose_target(target_pose)
plan = arm_group.go(wait=True)
arm_group.stop()
arm_group.clear_pose_targets()
rospy.loginfo("Arm positioniert über dem Objekt.")

# Greifer schließen (über MoveGroupCommander)
gripper_group.set_named_target("close")  # "close" für geschlossene Position
gripper_group.go(wait=True)
rospy.loginfo("Gripper geschlossen, Objekt gegriffen.")

# Cleanup
rospy.signal_shutdown("Task completed")
roscpp_shutdown()


