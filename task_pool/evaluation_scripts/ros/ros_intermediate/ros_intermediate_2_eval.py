import rospy
from sensor_msgs.msg import JointState
from trajectory_msgs.msg import JointTrajectory
from sensor_msgs.msg import JointState
import time
import math


class Eval():
    def __init__(self):
        self.current_joint_values = None
        self.received_msg = False

        rospy.init_node('ros_intermediate_2_eval', anonymous=True)

        rospy.Subscriber('/effort_joint_trajectory_controller/command', JointTrajectory, self.joint_trajectory_callback)

        rospy.Subscriber('/joint_states', JointState, self.joint_state_callback)

    def joint_state_callback(self, msg):
        self.current_joint_values = msg

    def joint_trajectory_callback(self):
        self.received_msg = True

    def evaluate_position(self, target_position):


        if self.current_joint_values is None or target_position is None:
            return False
        
        tolerance = 0.05

        if all(
            abs(current - target) < tolerance
            for current, target in zip(self.current_joint_values.position, target_position)
        ):
            return True
        
        return False
    

x = Eval()
t = time.time()
sucessful_positions = 0

target_positions_sequence = [[0, -math.pi/4, 0, -math.pi *3/4, 0, math.pi/2, math.pi* 3/4],
                             [-1.5, -0.5, 0.0, -1.5, 0.0, 1.5, 0.0],
                             [0, -math.pi/4, 0, -math.pi *3/4, 0, math.pi/2, math.pi/4]]

for target_position in target_positions_sequence:

    rate = rospy.Rate(10)
    while not rospy.is_shutdown():

        if x.evaluate_position(target_position):

            sucessful_positions += 1
            break

        if time.time() - t > 30:
            break

        rate.sleep()

    if time.time() - t > 30:
        break

if sucessful_positions == 3:
    print(True)

elif not x.received_msg:
    print('No message received on the topic.')
    print(False)

else:
    print('Position {} not reached.'.format(sucessful_positions + 1))
    print(False)