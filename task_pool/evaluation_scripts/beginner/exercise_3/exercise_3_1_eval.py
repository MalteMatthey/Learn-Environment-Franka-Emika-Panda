#!/usr/bin/env python

import rospy
from sensor_msgs.msg import JointState
import time


class JointSequenceEvaluator:
    def __init__(self, tolerance=0.01):
        """Initialize the evaluator."""
        self.current_joint_states = None
        self.target_joint_positions = None
        self.start_time = None
        self.tolerance = tolerance

        # Initialize the ROS node
        rospy.init_node('exercise_3_1_evaluation', anonymous=True)

        # Subscribe to the /joint_states topic
        rospy.Subscriber('/joint_states', JointState, self.joint_state_callback)

        rospy.loginfo("JointSequenceEvaluator initialized and listening to /joint_states.")

    def joint_state_callback(self, msg):
        """Callback for joint states topic."""
        self.current_joint_states = msg

    def set_target_position(self, target_position):
        """Set the target joint positions to evaluate."""
        self.target_joint_positions = target_position
        self.start_time = None  # Reset timer for new target

    def evaluate_position_hold(self, required_duration=0.5):
        """Evaluate if the robot holds a position for the required duration."""
        if self.current_joint_states is None or self.target_joint_positions is None:
            return False  # Cannot evaluate without data

        # Check if all joints are within the tolerance of the target position
        within_tolerance = all(
            abs(current - target) < self.tolerance
            for current, target in zip(self.current_joint_states.position, self.target_joint_positions)
        )

        if within_tolerance:
            # Start or update the timer
            if self.start_time is None:
                self.start_time = time.time()
                
            elif time.time() - self.start_time >= required_duration:
                rospy.loginfo("Position held for {} seconds. Marking as positive.".format(required_duration))
                return True
        else:
            # Reset the timer if the robot moves out of tolerance
            self.start_time = None

        return False


if __name__ == '__main__':
    try:
        evaluator = JointSequenceEvaluator()

        target_positions_sequence = [
            [0.5, -0.5, 0.5, -1.5, 0.5, 1.0, 0.5],         # Position 1
            [1.0, -1.0, 1.0, -2.0, 1.0, 0.5, 1.0],         # Position 2
            [1.5, -0.5, 1.5, -1.0, 1.5, 0.5, 1.5],       # position 3
            [1.0, -1.0, 1.0, -2.0, 1.0, 0.5, 1.0],        # Position 2
            [0.5, -0.5, 0.5, -1.5, 0.5, 1.0, 0.5],         # position 1
            [0.0, -0.785, 0.0, -2.356, 0.0, 1.571, 0.785]  # home position
        ]

        for idx, target_position in enumerate(target_positions_sequence):
            rospy.loginfo(f"Setting target position: {target_position}")
            evaluator.set_target_position(target_position)

            # Wait until the position is evaluated as positive
            rate = rospy.Rate(10)  # 10 Hz
            duration = 0.5
            while not rospy.is_shutdown():
                if idx == len(target_positions_sequence) - 1:
                    duration = 2.0

                if evaluator.evaluate_position_hold(duration):
                    rospy.loginfo("Target position successfully evaluated.")
                    break
                rate.sleep()

        print("All target positions successfully evaluated.")

    except rospy.ROSInterruptException:
        pass