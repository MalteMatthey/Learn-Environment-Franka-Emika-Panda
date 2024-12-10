import rospy
import time
import random

from std_msgs.msg import String


rospy.init_node('exercise_1_2_eval', anonymous=True)

pub = rospy.Publisher('/notification', String, queue_size=10)

t = time.time()
rate = rospy.Rate(10)

while time.time() - t < 60:

    msg = 'HI'
    pub.publish(msg)
    rate.sleep()

rospy.signal_shutdown('evaluation done')