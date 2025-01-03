import rospy
import time
import random
import rosnode
import rostopic
import re

from std_msgs.msg import String


def publishMsg(pub, msg):
    pub.publish(msg)

def check_node() -> bool:

    pattern = r'^/subscriber_node.*'

    node_names = rosnode.get_node_names()

    for node_name in node_names:
        if re.match(pattern, node_name):
            return True

    return False

def checkSubscribtion():

    topic = '/notification'
    pattern = r'^/subscriber_node.*'

    _, subs = rostopic.get_topic_list()

    subs = [x[2] for x in subs if x[0] == topic]

    if len(subs) == 0:
        return False

    for sub in subs[0]:
        if re.match(pattern, sub):
            return True
        
    return False

rospy.init_node('ros_basics_2_eval', anonymous=True)
pub = rospy.Publisher('/notification', String, queue_size=10)

t = time.time()
rate = rospy.Rate(10)
node_exist = False
sub_exist = False
evaluation_finished = False

while time.time() - t < 25:
    publishMsg(pub, str(random.randint(1, 100)))

    if not evaluation_finished:
        if check_node():
            node_exist = True

            if checkSubscribtion():
                sub_exist = True
                evaluation_finished = True

        else:
            node_exist = False

    rate.sleep()

if node_exist and sub_exist:
    print('true')

elif not node_exist:
    print('Node not found.')
    print('false')

else:
    print('Subscriber does not exist.')
    print('false')

rospy.signal_shutdown('evaluation done')