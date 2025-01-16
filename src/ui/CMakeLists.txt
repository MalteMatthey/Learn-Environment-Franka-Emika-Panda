add_library(ui
    custom_list_widget.cpp
    execute_frame.cpp
    sidebar.cpp
    subtask_item.cpp
    task_ui.cpp
    toast.cpp
    ${CMAKE_SOURCE_DIR}/learn_environment/include/ui/custom_list_widget.hpp
    ${CMAKE_SOURCE_DIR}/learn_environment/include/ui/execute_frame.hpp
    ${CMAKE_SOURCE_DIR}/learn_environment/include/ui/sidebar.hpp
    ${CMAKE_SOURCE_DIR}/learn_environment/include/ui/subtask_item.hpp
    ${CMAKE_SOURCE_DIR}/learn_environment/include/ui/task_ui.hpp
    ${CMAKE_SOURCE_DIR}/learn_environment/include/ui/toast.hpp
)

target_include_directories(ui PUBLIC ${CMAKE_SOURCE_DIR}/include/ui)

target_link_libraries(ui
    common
    Qt5::Core
    Qt5::Widgets
    ${QT_LIBRARIES}
    ${catkin_LIBRARIES}
)