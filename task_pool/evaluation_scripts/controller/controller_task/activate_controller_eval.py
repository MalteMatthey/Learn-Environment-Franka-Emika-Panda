import rospy
from controller_manager_msgs.srv import ListControllers

def evaluate_controller_activation():
    rospy.wait_for_service('/controller_manager/list_controllers')
    # Create a service proxy for '/controller_manager/list_controllers'
    list_controllers = rospy.ServiceProxy('/controller_manager/list_controllers', ListControllers)
    # Retrieve the list of controllers
    controllers = list_controllers().controller

    # Filter the running controllers
    running_controllers = [controller.name for controller in controllers if controller.state == 'running']
    
    # Check if exactly one controller is running
    assert len(running_controllers) == 1, f"Expected 1 running controller, but found {len(running_controllers)}"
    # Check if the running controller is 'position_joint_trajectory_controller'
    assert running_controllers[0] == 'position_joint_trajectory_controller', \
        f"Expected 'position_joint_trajectory_controller' to be running, but found {running_controllers[0]}"

if __name__ == "__main__":
    evaluate_controller_activation()
