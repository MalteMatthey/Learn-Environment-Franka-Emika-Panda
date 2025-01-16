cmake_minimum_required(VERSION 3.0.2)

add_library(common
    notebook_converter.cpp
    ${CMAKE_SOURCE_DIR}/learn_environment/include/common/folder_structure_constants.hpp
    ${CMAKE_SOURCE_DIR}/learn_environment/include/common/notebook_converter.hpp
    ${CMAKE_SOURCE_DIR}/learn_environment/include/common/task.hpp
)

target_include_directories(common PUBLIC ${CMAKE_SOURCE_DIR}/include/common)

target_link_libraries(common
    Qt5::Core
    Qt5::Widgets
    ${QT_LIBRARIES}
    ${catkin_LIBRARIES}
)