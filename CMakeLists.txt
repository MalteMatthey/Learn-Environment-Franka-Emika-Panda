cmake_minimum_required(VERSION 3.0.2)
project(learn_environment)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  rviz
  moveit_commander
  moveit_msgs
  geometry_msgs
  pluginlib
  std_msgs
  topic_tools
)

catkin_package(
  INCLUDE_DIRS include
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  /usr/include/nlohmann
)

catkin_install_python(PROGRAMS
  tasks/exercise_1/exercise_1_1.py
  tasks/exercise_1/exercise_1_2.py
  tasks/exercise_1/exercise_1_3.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

set(CMAKE_AUTOMOC ON)

## Include Qt libraries, only compatible with RViz using Qt5
find_package(Qt5 ${rviz_QT_VERSION} EXACT REQUIRED Core Widgets)
set(QT_LIBRARIES Qt5::Widgets)

add_definitions(-DQT_NO_KEYWORDS)

set(${PROJECT_NAME}_HDRS
  include/${PROJECT_NAME}/rviz_panel.hpp
)

set(${PROJECT_NAME}_SRCS
  src/rviz_panel.cpp
)


set(${PROJECT_NAME}_UIS
  resource/learn_panel.ui
)

# Create header from ui file (uic)
if(rviz_QT_VERSION VERSION_LESS "5")
    message(STATUS "Generate header for ui with rviz_QT_VERSION: ${rviz_QT_VERSION}")
    qt4_wrap_ui(${PROJECT_NAME}_UIS_H ${${PROJECT_NAME}_UIS})
    qt4_wrap_cpp(${PROJECT_NAME}_MOCS ${${PROJECT_NAME}_HDRS})
else()
    message(STATUS "Generate header for ui with rviz_QT_VERSION: ${rviz_QT_VERSION}")
    qt5_wrap_ui(${PROJECT_NAME}_UIS_H ${${PROJECT_NAME}_UIS})
    qt5_wrap_cpp(${PROJECT_NAME}_MOCS ${${PROJECT_NAME}_HDRS})
endif()


add_library(learn_panel
  ${${PROJECT_NAME}_SRCS}
  ${${PROJECT_NAME}_UIS_H}
  ${${PROJECT_NAME}_MOCS} 
)

target_link_libraries(learn_panel
  ${catkin_LIBRARIES}
  ${QT_LIBRARIES}
  ${topic_tools_LIBRARIES}
)