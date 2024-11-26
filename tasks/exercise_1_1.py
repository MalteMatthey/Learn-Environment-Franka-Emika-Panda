import rospy
from trajectory_msgs.msg import JointTrajectory, JointTrajectoryPoint

def test():
    rospy.init_node('panda_joint5_control', anonymous=True)
    
    # Create publisher for the correct controller topic
    pub = rospy.Publisher('/effort_joint_trajectory_controller/command', JointTrajectory, queue_size=10)

    # Create JointTrajectory message
    joint_trajectory = JointTrajectory()
    joint_trajectory.header.stamp = rospy.Time.now()
    joint_trajectory.joint_names = ['panda_joint1', 'panda_joint2', 'panda_joint3', 'panda_joint4', 'panda_joint5', 'panda_joint6', 'panda_joint7']

    # Create JointTrajectoryPoint for the desired joint positions
    point = JointTrajectoryPoint()
    point.positions = [0, 0, 0, 0, 0, 0, 0]  # Set joint 5 to 2.8 radians (45 degrees)
    point.time_from_start = rospy.Duration(5.0)  # Make the movement last 2 seconds

    point2 = JointTrajectoryPoint()
    point2.positions = [2.7, 1.6, 2.7, 3.7525/2, 2.7, 2.7, 2.7]  # Set joint 5 to 2.8 radians (45 degrees)
    point2.time_from_start = rospy.Duration(10.0)  # Make the movement last 2 seconds

    # Add point to the trajectory
    joint_trajectory.points.append(point)
    joint_trajectory.points.append(point2)

    rospy.sleep(1)  # Wait for initialization

    rospy.loginfo("Publishing joint trajectory command.")
    pub.publish(joint_trajectory)
    rospy.loginfo("Joint trajectory command published.")

    rospy.sleep(15)  # Wait for movement to finish

if __name__ == '__main__':
    try:
        test()
    except rospy.ROSInterruptException:
        pass
