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

# Find Qt5 packages
find_package(Qt5 ${rviz_QT_VERSION} EXACT REQUIRED Core Widgets)

# Enable automatic MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

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
  converter/converted.py
  tutorial/beginner/exercise_1/exercise_1_1.ipynb
  tutorial/beginner/exercise_1/exercise_1_2.ipynb
  tutorial/beginner/exercise_1/exercise_1_3.ipynb
  tutorial/beginner/exercise_2/exercise_2_1.py
  tutorial/beginner/exercise_2/exercise_2_2.py
  tutorial/beginner/exercise_3/exercise_3_1.py
  tutorial/beginner/exercise_4/exercise_4_1.ipynb
  tutorial/beginner/exercise_5/exercise_5_1.ipynb
  tutorial/intermediate/pick_place.ipynb
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

set(CMAKE_AUTOMOC ON)

## Include Qt libraries, only compatible with RViz using Qt5
find_package(Qt5 ${rviz_QT_VERSION} EXACT REQUIRED Core Widgets)
set(QT_LIBRARIES Qt5::Widgets)

add_definitions(-DQT_NO_KEYWORDS)

set(${PROJECT_NAME}_HDRS
  # include/${PROJECT_NAME}/rviz_panel.hpp
  include/${PROJECT_NAME}/learning_plattform.h
  include/${PROJECT_NAME}/subtask_item.h
  include/${PROJECT_NAME}/task_manager.h
  include/${PROJECT_NAME}/task.h
  include/${PROJECT_NAME}/custom_list_widget.h
  include/${PROJECT_NAME}/sidebar.h
)

set(${PROJECT_NAME}_SRCS
  # src/rviz_panel.cpp
  src/learning_plattform.cpp
  src/subtask_item.cpp
  src/task_manager.cpp
  src/custom_list_widget.cpp
  src/sidebar.cpp
)

set(${PROJECT_NAME}_UIS
  # resource/learn_panel.ui
  resource/learning_plattform.ui
)

set(${PROJECT_NAME}_QRC
  resources.qrc
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
    qt5_add_resources(${PROJECT_NAME}_QRC_SOURCES ${${PROJECT_NAME}_QRC})
endif()

add_library(learn_environment
  ${${PROJECT_NAME}_SRCS}
  ${${PROJECT_NAME}_UIS_H}
  ${${PROJECT_NAME}_MOCS}
  ${${PROJECT_NAME}_QRC_SOURCES}
  ${${PROJECT_NAME}_UIS}
  ${${PROJECT_NAME}_QRC} 
)

target_link_libraries(learn_environment
  ${catkin_LIBRARIES}
  Qt5::Core
  Qt5::Widgets
  ${QT_LIBRARIES}
  ${topic_tools_LIBRARIES}
)
