import rospy
import math
import sys

from moveit_commander import MoveGroupCommander, PlanningSceneInterface, roscpp_initialize, roscpp_shutdown


def robot_already_in_default_position(group, robot_default_position):
    current_joint_values = group.get_current_joint_values()

    for i in range(7):
        if abs(current_joint_values[i] - robot_default_position[i]) > 0.01:
            return False

    return True

def reset_robot(group):
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


def remove_objects(scene):
    objects = scene.get_known_object_names()

    for object_name in objects:
        scene.remove_world_object(object_name)

    rospy.sleep(2)


rospy.init_node('reset', anonymous=True)
roscpp_initialize(sys.argv)
group = MoveGroupCommander("panda_arm")

robot_default_position = [0.0, -math.pi / 4, 0.0, -math.pi * 3 / 4, 0.0, math.pi / 2, math.pi / 4]

if not robot_already_in_default_position(group, robot_default_position):
    reset_robot(group)

scene = PlanningSceneInterface(synchronous=True)

if len(scene.get_known_object_names()) > 0:
    remove_objects(scene)

roscpp_shutdown()
rospy.signal_shutdown("Task completed.")

print("Successfully reset the robot and removed all objects.")