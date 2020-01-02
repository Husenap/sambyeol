set(target_name "app")

set(src_sambyeol
    "src/sambyeol/SambyeolWindow.h"
    "src/sambyeol/SambyeolWindow.cpp")

set(src_files
    ${src_sambyeol}
    ${src_window}
    "sambyeol.manifest"
    "src/main.cpp")

add_executable(${target_name} ${src_files})

source_group("src\\sambyeol" FILES ${src_sambyeol})
source_group("src" FILES "src/main.cpp")

target_link_libraries(${target_name}
    "sambyeol_lib"
    "d2d1")

set_target_properties(${target_name} PROPERTIES
    FOLDER ${PROJECT_NAME}
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin 
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

cotire(${target_name})