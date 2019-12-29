set(target_name "sambyeol_lib")

set(src_window
    "src/window/DpiScale.h"
    "src/window/BaseWindow.h"
    "src/window/MainWindow.h"
    "src/window/MainWindow.cpp")

set(src_files
    ${src_window})

add_library(${target_name} STATIC ${src_files})

target_include_directories(${target_name} INTERFACE "src")

set_target_properties(${target_name} PROPERTIES
    FOLDER ${PROJECT_NAME}
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

cotire(${arget_name})