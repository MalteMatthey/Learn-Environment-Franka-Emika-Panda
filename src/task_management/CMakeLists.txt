add_library(task_management
    task_manager.cpp
    task_parser.cpp
    ${CMAKE_SOURCE_DIR}/learn_environment/include/task_management/task_manager.hpp
    ${CMAKE_SOURCE_DIR}/learn_environment/include/task_management/task_parser.hpp
)

target_include_directories(task_management PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(task_management
    common
    script_execution
    ui
    Qt5::Core
    Qt5::Widgets
    ${QT_LIBRARIES}
    ${catkin_LIBRARIES}
)