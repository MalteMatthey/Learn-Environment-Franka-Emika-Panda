import rospy
from moveit_commander import PlanningSceneInterface, roscpp_initialize, roscpp_shutdown
import sys

def evaluate_scene_empty():

    # Initialize ROS and PlanningSceneInterface
    roscpp_initialize(sys.argv)
    rospy.init_node('evaluate_scene_empty', anonymous=True)

    # Interface for PlanningScene
    scene = PlanningSceneInterface()

    # Short delay to make sure the scene is loaded
    rospy.sleep(2)

    # Query the currently existing objects in the scene
    current_objects = scene.get_known_object_names()

    if not current_objects:
        print("The scene is empty. All objects have been successfully removed.")
        print(True)
    else:
        print("The scene is not empty. The following objects are still there:")
        for obj in current_objects:
            print(f" - {obj}")
        print(False)

    # Shutdown ROS
    rospy.signal_shutdown("Task completed")
    roscpp_shutdown()

if __name__ == "__main__":
    evaluate_scene_empty()