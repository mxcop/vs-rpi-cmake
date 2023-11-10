cmake_minimum_required(VERSION 3.15)
project(vs-rpi-cmake)

# Set C++ 17 standard
set( CMAKE_CXX_STANDARD 17 CACHE STRING "" FORCE )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE )

# Create executable target
add_executable( main )

set_target_properties( main PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME} )

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    # Post build command (copies the asset files) (!this one is required for VS)
    add_custom_command( TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
        ${PROJECT_SOURCE_DIR}/public 
        ${PROJECT_BINARY_DIR}/public
    )
else()
    # Post build command (copies the asset files)
    add_custom_command( TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
        ${PROJECT_SOURCE_DIR}/public 
        $<CONFIG>/public
    )
endif()

# Add include directories
target_include_directories( main PRIVATE "src" )

# Sub-directories
add_subdirectory( "src" )
add_subdirectory( "lib" ) 
