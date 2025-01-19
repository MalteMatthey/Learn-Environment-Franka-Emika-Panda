# Start your own project

After you have worked through the tutorial, you will probably want to start your own project. 

### Launch file
Whether with Docker or without, first you need a launch file to start the Gazebo Simulator, Rviz etc.. . A good start is this launch file:

<launch>
  <!-- specify the planning pipeline -->
  <arg name="pipeline" default="ompl" />

  <!-- Panda specific options -->
  <arg name="load_gripper" default="true" />
  <arg name="transmission" default="effort" />

  <!-- Gazebo specific options -->
  <arg name="gazebo_gui" default="true" />
  <arg name="paused" default="false" />

  <!-- Launch the gazebo simulator and spawn the robot -->
  <include file="$(find franka_gazebo)/launch/panda.launch" pass_all_args="true">
    <arg name="headless" value="$(eval not arg('gazebo_gui'))" />
    <arg name="use_gripper" default="$(arg load_gripper)" />
    <arg name="controller" default="$(arg transmission)_joint_trajectory_controller" />
  </include>

  <include file="$(dirname)/demo.launch" pass_all_args="true">
    <!-- robot description is loaded by gazebo.launch, to enable Gazebo features -->
    <arg name="load_robot_description" value="false" />
    <!-- MoveItSimpleControllerManager provides ros_control's JointTrajectory controllers
         as well as GripperCommand actions -->
    <arg name="moveit_controller_manager" value="simple" />
  </include>
</launch>

This launch file should already be available from the installed systems (/root/ws_moveit/src/panda_moveit_config/launch/demo_gazebo.launch). It can be started with 
"roslaunch panda_moveit_config demo_gazebo.launch".
If it is not available, just copy the above file and run it. This launch file starts other launch files, such as “/launch/panda.launch” or “/demo.launch”.

### Python Scripts

In order for a Python script to communicate with the entire system, it is essential to create a ros node in that script. However, if you are using the MoveIt Python interface, it is possible to do this without a node, but this is not recommended as it may lead to unexpected errors. Creating and executing a Python script is basically the same as any other Python script.