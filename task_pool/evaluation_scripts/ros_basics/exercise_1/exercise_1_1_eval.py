import rosnode
import rospy
import time
import re

from std_msgs.msg import String


class Eval():

    def __init__(self):
        self.received_msg = None

        rospy.init_node('exercise_1_1_eval', anonymous=True)

        rospy.Subscriber("/chatter", String, self.callback)

    def callback(self, msg):
        self.received_msg = msg.data
        
    def check_node(self):

        pattern = r'^/publisher_node.*'

        node_names = rosnode.get_node_names()

        for node_name in node_names:
            if re.match(pattern, node_name):
                return True

        return False
    
    def checkPublished(self):

        t = time.time()

        while time.time() - t < 3:
            if not self.received_msg == 'Hello, ROS!':
                return False
            
        return True

x = Eval()

node_exists = False
msg_received = False
t = time.time()

while time.time() - t < 25:

    if not node_exists:
        if x.check_node():
            node_exists = True

    else:

        if x.checkPublished():

            msg_received = True
            break

if node_exists and msg_received:
    print('true: Evaluation Success')

elif not node_exists:
    print('false: Node not found')

else:
    print('false: Message not received')


rospy.signal_shutdown('evaluation done')