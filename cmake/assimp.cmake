message("-- External Project: assimp")
include(FetchContent)

FetchContent_Declare(
	assimp
	GIT_REPOSITORY  https://github.com/assimp/assimp.git
	GIT_TAG         v5.0.0
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT FALSE CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER TRUE CACHE BOOL "" FORCE)

FetchContent_GetProperties(assimp)
if(NOT assimp_POPULATED)
    FetchContent_Populate(assimp)
    add_subdirectory(${assimp_SOURCE_DIR} ${assimp_BINARY_DIR})
endif()

set(assimp_libraries
    "assimp"
    "zlib"
    "zlibstatic"
    "IrrXML"
    "uninstall"
    "UpdateAssimpLibsDebugSymbolsAndDLLs")

foreach(library ${assimp_libraries})
    set_target_properties(${library} PROPERTIES FOLDER "thirdparty//assimp")
endforeach()

set_target_properties("assimp" PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")