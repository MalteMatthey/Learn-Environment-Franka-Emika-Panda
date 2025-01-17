import math
from geometry_msgs.msg import Pose, Point, Quaternion
import os
import importlib.util

# Search for the file 'waypoints.py' in the file system
waypoints_path = None
for root, dirs, files in os.walk('/'):
    if 'waypoints.py' in files:
        waypoints_path = os.path.join(root, 'waypoints.py')
        break

# Load the 'waypoints' module dynamically if the file is found
if waypoints_path:
    spec = importlib.util.spec_from_file_location('waypoints', waypoints_path)
    waypoints_module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(waypoints_module)
    waypoints_user = waypoints_module.waypoints
else:
    raise FileNotFoundError('waypoints.py not found')

# Example solution values
sol = [0.4, 0.2, 0.7, -1.0, 0.4, 0.0, 0.0]

# Define target pose waypoints based on the solution values
pose1 = Pose(
    position=Point(sol[0] + 0.1, sol[1], sol[2]),
    orientation=Quaternion(sol[3], sol[4], sol[5], sol[6])
)

pose2 = Pose(
    position=Point(sol[0] + 0.1, sol[1] + 0.2, sol[2]),
    orientation=Quaternion(sol[3], sol[4], sol[5], sol[6])
)

pose3 = Pose(
    position=Point(sol[0] + 0.1, sol[1] + 0.2, sol[2] + 0.1),
    orientation=Quaternion(sol[3], sol[4], sol[5], sol[6])
)

# List of target pose waypoints
target_waypoints = [pose1, pose2, pose3]

# Predefined waypoints
waypoints = [
    Pose(
        position=Point(0.4, 0.0, 0.6),
        orientation=Quaternion(-1.0, 0.4, 0.0, 0.0)
    ),
    Pose(
        position=Point(0.4, 0.2, 0.6),
        orientation=Quaternion(-1.0, 0.4, 0.0, 0.0)
    ),
    Pose(
        position=Point(0.4, 0.2, 0.7),
        orientation=Quaternion(-1.0, 0.4, 0.0, 0.0)
    )
]

# Function to check if two points are close to each other
def is_close(p1, p2, tolerance=0.2):
    return abs(p1.x - p2.x) <= tolerance and abs(p1.y - p2.y) <= tolerance and abs(p1.z - p2.z) <= tolerance

# Function to check if two quaternions are close to each other
def quaternion_is_close(q1, q2, tolerance=0.1):
    return (abs(q1.x - q2.x) <= tolerance and abs(q1.y - q2.y) <= tolerance and
            abs(q1.z - q2.z) <= tolerance and abs(q1.w - q2.w) <= tolerance)

success = True

# Check if the target pose waypoints match the predefined waypoints
for i in range(3):
    if (is_close(target_waypoints[i].position, waypoints[i].position) and
            quaternion_is_close(target_waypoints[i].orientation, waypoints[i].orientation)):
        print(f"Pose {i+1} matches.")
    else:
        print(f"Pose {i+1} does not match.")
        success = False

print(success)