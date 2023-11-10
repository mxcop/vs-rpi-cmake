# Glob all source files
file(GLOB_RECURSE SOURCES
    "*.cpp"
)

# Add source files into target main
target_sources( main PRIVATE
    ${SOURCES}
)
