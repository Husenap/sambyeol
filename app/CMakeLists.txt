set(target_name "app")

set(src_window
    "src/window/BaseWindow.h"
    "src/window/MainWindow.h"
    "src/window/MainWindow.cpp")

set(src_files
    ${src_window}
    "sambyeol.manifest"
    "src/main.cpp")

add_executable(${target_name} ${src_files})

source_group("src\\window" FILES ${src_window})
source_group("src" FILES "src/main.cpp")

set_target_properties(${target_name} PROPERTIES FOLDER ${PROJECT_NAME})
set_target_properties(${target_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin )

cotire(${target_name})