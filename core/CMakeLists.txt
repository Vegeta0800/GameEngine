
cmake_minimum_required(VERSION 3.11)

# define project
project(core C CXX)
set(TARGET_NAME core)

# list include files to variables
file(GLOB SRC
	"src/*.cpp"
)

# set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)

# configure executable, properties and linkage
add_library(${TARGET_NAME} STATIC ${SRC})

# set some target properties
set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX ".debug")

# define the include directories
target_include_directories(${TARGET_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/header"
	"${CMAKE_SOURCE_DIR}/shared/header"
)
