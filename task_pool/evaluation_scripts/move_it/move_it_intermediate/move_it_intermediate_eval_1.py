import math
import os
import importlib.util
from geometry_msgs.msg import Pose, Point, Quaternion

# Dynamically import waypoints module
waypoints_path = None
for root, dirs, files in os.walk('/'):
    if 'converted.py' in files:
        waypoints_path = os.path.join(root, 'converted.py')
        break

if waypoints_path:
    spec = importlib.util.spec_from_file_location('waypoints', waypoints_path)
    waypoints_module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(waypoints_module)
    waypoints = waypoints_module.waypoints
else:
    raise FileNotFoundError("The converted.py file was not found in the file system")

print("Waypoints:")
print(waypoints)

sol = [0.4, 0.2, 0.7, -1.0, 0.4, 0.0, 0.0]

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

target_waypoints = [pose1, pose2, pose3]

def is_close(p1, p2, tolerance=0.2):
    return abs(p1.x - p2.x) <= tolerance and abs(p1.y - p2.y) <= tolerance and abs(p1.z - p2.z) <= tolerance

def quaternion_is_close(q1, q2, tolerance=0.1):
    return (abs(q1.x - q2.x) <= tolerance and abs(q1.y - q2.y) <= tolerance and
            abs(q1.z - q2.z) <= tolerance and abs(q1.w - q2.w) <= tolerance)

for i in range(3):
    if (is_close(target_waypoints[i].position, waypoints[i].position) and
            quaternion_is_close(target_waypoints[i].orientation, waypoints[i].orientation)):
        print(f"Pose {i+1} matches.")
    else:
        print(f"Pose {i+1} does not match.")