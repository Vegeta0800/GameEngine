
cmake_minimum_required(VERSION 3.11)

# define project
project(renderexample C CXX)
set(TARGET_NAME renderexample)

# list include files to variables
file(GLOB SRC
	"src/*.cpp"
)

# set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

# configure executable, properties and linkage
add_executable(${TARGET_NAME} ${SRC})

# set some target properties
set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX ".debug")

target_link_libraries(${TARGET_NAME} core shared rendering game ${Vulkan_LIBRARY})

# define the include directories
target_include_directories(${TARGET_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/header"
	"${CMAKE_SOURCE_DIR}/rendering/header"
)
