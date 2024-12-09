import rospy
import math
import sys

from moveit_commander import MoveGroupCommander, PlanningSceneInterface, roscpp_initialize, roscpp_shutdown


def reset_robot():
    group = MoveGroupCommander("panda_arm")
    group.set_max_velocity_scaling_factor(1.0)
    group.set_max_acceleration_scaling_factor(1.0)

    joint_goal = group.get_current_joint_values()

    joint_goal[0] = 0.0
    joint_goal[1] = -math.pi / 4
    joint_goal[2] = 0.0
    joint_goal[3] = -math.pi * 3 / 4
    joint_goal[4] = 0.0
    joint_goal[5] = math.pi / 2
    joint_goal[6] = math.pi / 4

    group.set_joint_value_target(joint_goal)
    group.go(wait=True)


def remove_objects():
    scene = PlanningSceneInterface(synchronous=True)
    rospy.sleep(2)  # Allow some time for the scene to initialize
    objects = scene.get_known_object_names()

    for object_name in objects:
        scene.remove_world_object(object_name)

    rospy.sleep(2)  # Allow some time for the objects to be removed


rospy.init_node('reset', anonymous=True)
roscpp_initialize(sys.argv)

reset_robot()
remove_objects()

roscpp_shutdown()

print("resetting finised")