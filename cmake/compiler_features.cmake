if (MSVC)
	# Explicitly specify flags in order to enable VS2017 C++17 features.
	# In VS2017 15.9.3 this is needed to correctly identify nested namespace definitions
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
	add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX -DWIN_LEAN_AND_MEAN)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
endif()

message("-- CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
