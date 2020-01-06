set(target_name "sambyeol_lib")

set(src_util
    "src/util/FPSCounter.h"
    "src/util/FPSCounter.cpp")

set(src_bitmap
    "src/bitmap/Bitmap.h"
    "src/bitmap/Bitmap.cpp")

set(src_window
    "src/window/DpiScale.h"
    "src/window/BaseWindow.h"
    "src/window/MainWindow.h"
    "src/window/MainWindow.cpp")

set(src_files
    ${src_util}
    ${src_bitmap}
    ${src_window})

add_library(${target_name} STATIC ${src_files})

target_link_libraries(${target_name}
    "compiler-warnings"
    "d2d1"
    "assimp"
    "glm_static")

source_group("src\\util" FILES ${src_util})
source_group("src\\bitmap" FILES ${src_bitmap})
source_group("src\\window" FILES ${src_window})

target_include_directories(${target_name} INTERFACE "src")

set_target_properties(${target_name} PROPERTIES
    FOLDER ${PROJECT_NAME}
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

cotire(${target_name})