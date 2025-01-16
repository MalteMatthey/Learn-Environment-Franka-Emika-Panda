add_library(script_execution
    process_runner.cpp
    script_worker.cpp
    task_executor.cpp
    ${CMAKE_SOURCE_DIR}/learn_environment/include/script_execution/process_runner.hpp
    ${CMAKE_SOURCE_DIR}/learn_environment/include/script_execution/script_worker.hpp
    ${CMAKE_SOURCE_DIR}/learn_environment/include/script_execution/task_executor.hpp
)

target_include_directories(script_execution PUBLIC 
    ${CMAKE_SOURCE_DIR}/include/script_execution
)

target_link_libraries(script_execution
    common
    Qt5::Core
    Qt5::Widgets
    ${QT_LIBRARIES}
    ${catkin_LIBRARIES}
)