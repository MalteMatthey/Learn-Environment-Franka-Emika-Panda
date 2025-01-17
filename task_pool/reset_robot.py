import rospy
import math
import sys

from moveit_commander import MoveGroupCommander, PlanningSceneInterface, roscpp_initialize, roscpp_shutdown
from controller_manager_msgs.srv import ListControllers, LoadController, SwitchController, SwitchControllerRequest

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

def ensure_controllers_active(controllers):
    # Ensure that the given controllers are loaded and running and that all other controllers are stopped.
    rospy.wait_for_service('/controller_manager/list_controllers')
    list_controllers = rospy.ServiceProxy('/controller_manager/list_controllers', ListControllers)
    loaded_controllers = [controller.name for controller in list_controllers().controller]
    running_controllers = [controller.name for controller in list_controllers().controller if controller.state == 'running']

    rospy.wait_for_service('/controller_manager/load_controller')
    load_service = rospy.ServiceProxy('/controller_manager/load_controller', LoadController)

    for controller in controllers:
        if controller not in loaded_controllers:
            response = load_service(controller)
            assert response.ok, f"Controller {controller} could not be loaded."

    rospy.wait_for_service('/controller_manager/switch_controller')
    switch_service = rospy.ServiceProxy('/controller_manager/switch_controller', SwitchController)
    switch_req = SwitchControllerRequest()
    switch_req.start_controllers = controllers
    switch_req.stop_controllers = running_controllers
    switch_req.strictness = SwitchControllerRequest.BEST_EFFORT
    response = switch_service(switch_req)
    assert response.ok, "Controllers could not be started."

    rospy.sleep(2)

rospy.init_node('reset', anonymous=True)
roscpp_initialize(sys.argv)

# remove all objects from the scene
scene = PlanningSceneInterface(synchronous=True)
scene.remove_world_object()

print("Successfully reset the robot and removed all objects.")

rospy.sleep(2)

group = MoveGroupCommander("panda_arm")

robot_default_position = [0.0, -math.pi / 4, 0.0, -math.pi * 3 / 4, 0.0, math.pi / 2, math.pi / 4]

if not robot_already_in_default_position(group, robot_default_position):
    reset_robot(group)

rospy.sleep(2)

controllers_to_activate = ['franka_gripper', 'franka_state_controller', 'effort_joint_trajectory_controller']
ensure_controllers_active(controllers_to_activate)

rospy.sleep(2)

roscpp_shutdown()
rospy.signal_shutdown("Task completed.")