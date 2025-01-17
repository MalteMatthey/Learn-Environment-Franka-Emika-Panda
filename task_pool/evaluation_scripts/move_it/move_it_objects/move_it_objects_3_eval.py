import rospy
from moveit_commander import PlanningSceneInterface, roscpp_initialize, roscpp_shutdown

def evaluate_scene_empty():
    """
    Checks whether all objects have been removed from the planning environment.
    Issues a success message if there are no objects in the scene,
    Otherwise, a warning.
    """
    # Initialize ROS and PlanningSceneInterface
    roscpp_initialize([])
    rospy.init_node('evaluate_scene_empty', anonymous=True)

    # Interface for PlanningScene
    scene = PlanningSceneInterface()

    # Short delay to make sure the scene is loaded
    rospy.sleep(1)

    # Query the currently existing objects in the scene
    current_objects = scene.get_known_object_names()

    if not current_objects:
        rospy.loginfo("The scene is empty. All objects have been successfully removed.")
        print("The scene is empty. All objects have been successfully removed.")
        print(True)
    else:
        rospy.logwarn("The scene is not empty. The following objects are still there:")
        print("The scene is not empty. The following objects are still there:")
        for obj in current_objects:
            rospy.logwarn(f" - {obj}")
            print(f" - {obj}")
        print(False)

    # Shutdown ROS
    roscpp_shutdown()

if __name__ == "__main__":
    evaluate_scene_empty()