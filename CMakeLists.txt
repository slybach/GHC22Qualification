cmake_minimum_required(VERSION 3.13)

set(CMAKE_DEBUG_POSTFIX d)

# set the project name
project(MentorshipAndTeamwork)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# defines a target called main with an empty file list, the file list is populated by the CMakeLists.txt in the src subdirectory
add_executable(main "")

set_target_properties(main
	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
		DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

add_subdirectory(src)
target_include_directories(main PRIVATE include)
