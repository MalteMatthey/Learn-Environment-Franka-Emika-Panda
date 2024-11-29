import rospy
import sys
from moveit_commander import MoveGroupCommander, roscpp_initialize, roscpp_shutdown

"""
Ziel :
Den Status des Roboters abfragen und interpretieren.
Diese Aufgabe soll helfen, den aktuellen Zustand des
Roboters (Positionen, Orientierungen, Gelenkwinkel) zu überprüfen
und zu verstehen, wie er während oder nach einer Bewegung auf Feedback zugreifen kann.
"""

# intialize the MoveIt! commander from the moveit_command package
roscpp_initialize(sys.argv)

# Initialisiere den ROS-Knoten
rospy.init_node('exercise_4_1', anonymous=True)

# Initialisiere die MoveGroup
group = MoveGroupCommander('panda_arm')

# set the planner ID to 'RRTConnectkConfigDefault'
group.set_planner_id("RRTConnectkConfigDefault")

# set the number of planning attempts to 10
group.set_num_planning_attempts(10)

# Abfrage der aktuellen Gelenkwinkel
joint_values = group.get_current_joint_values()
rospy.loginfo("Current Joint Values: ")
for i, joint in enumerate(joint_values):
    rospy.loginfo(f"Joint {i+1}: {joint:.2f} rad")

# Abfrage der aktuellen Endeffektor-Position
pose = group.get_current_pose().pose
rospy.loginfo(f"Endeffector Position: x={pose.position.x:.3f}, y={pose.position.y:.3f}, z={pose.position.z:.3f}")
rospy.loginfo(f"Endeffector Orientation: x={pose.orientation.x:.3f}, y={pose.orientation.y:.3f}, z={pose.orientation.z:.3f}, w={pose.orientation.w:.3f}")

# delete the ros node
rospy.signal_shutdown("Task completed")
roscpp_shutdown()
