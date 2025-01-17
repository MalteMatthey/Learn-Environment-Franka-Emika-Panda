import rospy
from controller_manager_msgs.srv import ListControllers

def evaluate_stop_controllers():
    rospy.wait_for_service('/controller_manager/list_controllers')
    try:
        list_controllers = rospy.ServiceProxy('/controller_manager/list_controllers', ListControllers)
        controllers = list_controllers().controller
        active_controllers = [controller.name for controller in controllers if controller.state == 'running']
        
        if not active_controllers:
            print("Evaluation passed: No active controllers.")
        else:
            print("Evaluation failed: There are still active controllers:", active_controllers)
    except rospy.ServiceException as e:
        rospy.logerr("Service call failed: %s" % e)

if __name__ == "__main__":
    evaluate_stop_controllers()
