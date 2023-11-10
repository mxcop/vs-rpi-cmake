set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Glad (OpenGL (ES))
add_subdirectory("glad")
target_link_libraries(main glad)

# GLFW (Windowing & Input)
add_subdirectory("glfw")
target_link_libraries(main glfw)
