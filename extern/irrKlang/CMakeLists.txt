cmake_minimum_required(VERSION 3.5)
project(irrKlang)

# Set the output directory for libraries and executables
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set the include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add the binary library directory
set(IRRKLANG_LIBRARY_DIR bin/linux-gcc-64/)  # Update the path to the binary library directory

# Get all .so files in the library directory
file(GLOB IRRKLANG_LIBRARY_FILES "${IRRKLANG_LIBRARY_DIR}/*.so")

# Create a library target
add_library(${PROJECT_NAME} SHARED IMPORTED)

# Set the library properties for each .so file
foreach(LIBRARY_FILE ${IRRKLANG_LIBRARY_FILES})
    get_filename_component(LIBRARY_NAME ${LIBRARY_FILE} NAME_WE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        IMPORTED_LOCATION_${LIBRARY_NAME} ${LIBRARY_FILE}
    )
endforeach()

# Set the required C++ standard for your library
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
)

# Specify the installation directories for your library
# install(TARGETS ${PROJECT_NAME}
#     ARCHIVE DESTINATION lib
#     LIBRARY DESTINATION lib
#     RUNTIME DESTINATION bin
# )
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include)

# Optionally, you can add additional installation steps or custom configurations here

