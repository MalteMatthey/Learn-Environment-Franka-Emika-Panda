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

## Include Qt libraries, only compatible with RViz using Qt5
find_package(Qt5 ${rviz_QT_VERSION} EXACT REQUIRED Core Widgets)
set(QT_LIBRARIES Qt5::Widgets)

add_definitions(-DQT_NO_KEYWORDS)

add_subdirectory(src/task_management)
add_subdirectory(src/ui)
add_subdirectory(src/script_execution)
add_subdirectory(src/common)

set(${PROJECT_NAME}_HDRS
  include/learn_environment.hpp
)

set(${PROJECT_NAME}_SRCS
  src/learn_environment.cpp
)

set(${PROJECT_NAME}_UIS
  resource/learn_environment.ui
)

set(${PROJECT_NAME}_QRC
  resource/resources.qrc
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
  task_management
  ui
  script_execution
  common
)
