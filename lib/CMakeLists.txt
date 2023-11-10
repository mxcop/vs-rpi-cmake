set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Glad (OpenGL (ES))
add_subdirectory("glad")
target_link_libraries(main glad)

# GLFW (Windowing & Input)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # Don't build the GLFW example programs
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE) # Don't build the GLFW test programs
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE) # Don't build the GLFW documentation
add_subdirectory("glfw")
target_link_libraries(main glfw)

# GLM (Math library)
add_subdirectory("glm")
target_link_libraries(main glm)
