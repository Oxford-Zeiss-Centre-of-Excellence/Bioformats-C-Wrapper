cmake_minimum_required(VERSION 3.10)
project(BioFormatsWrapper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the paths to Java JDK and JNI headers
find_package(Java REQUIRED)
find_package(JNI REQUIRED)

include(UseJava)

# Java source files and output paths
set(JAVA_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(JAVA_OUTPUT_DIR ${CMAKE_BINARY_DIR}/java)

# Create directory for compiled Java classes and JAR files
file(MAKE_DIRECTORY ${JAVA_OUTPUT_DIR})

# Add Java files to compile
set(JAVA_SOURCES 
    ${JAVA_SOURCE_DIR}/BioFormatsWrapper.java 
    )

# Download necessary BioFormats dependencies
set(JAVA_DEPS
    https://github.com/ome/bioformats/releases/download/v7.3.1/bioformats_package.jar
    https://github.com/ome/bioformats/releases/download/v7.3.1/bio-formats-tools.jar
    https://github.com/ome/bioformats/releases/download/v7.3.1/formats-gpl.jar
    https://github.com/ome/bioformats/releases/download/v7.3.1/formats-api.jar
)

# List to store paths of downloaded files (similar to JAVA_INCLUDES)
set(DOWNLOADED_FILES)

# Iterate through each file URL and download it
foreach(FILE_URL IN LISTS JAVA_DEPS)
    # Get the file name from the URL
    get_filename_component(FILE_NAME ${FILE_URL} NAME)

    # Set the full output path for the file
    set(OUTPUT_FILE "${JAVA_OUTPUT_DIR}/${FILE_NAME}")

    # Download the file
    file(DOWNLOAD ${FILE_URL} ${OUTPUT_FILE}
         SHOW_PROGRESS
         STATUS DOWNLOAD_STATUS
         )

    # Check the status of the download operation
    list(GET DOWNLOAD_STATUS 0 DOWNLOAD_RESULT)
    list(GET DOWNLOAD_STATUS 1 DOWNLOAD_MESSAGE)

    if(NOT ${DOWNLOAD_RESULT} EQUAL 0)
        message(FATAL_ERROR "Failed to download ${FILE_NAME}: ${DOWNLOAD_MESSAGE}")
    else()
        message(STATUS "Successfully downloaded ${FILE_NAME} to ${OUTPUT_FILE}")

        # Add the downloaded file path to the list of downloaded files
        list(APPEND DOWNLOADED_FILES ${OUTPUT_FILE})
    endif()
endforeach()

# Set JAVA_INCLUDES variable to include the downloaded Bioformats packages for Java side wrapper
set(JAVA_INCLUDES ${DOWNLOADED_FILES})

# Compile Java files and create a JAR file
add_jar(BioFormatsWrapper
    SOURCES ${JAVA_SOURCES}
    OUTPUT_DIR ${JAVA_OUTPUT_DIR}
    INCLUDE_JARS  ${JAVA_INCLUDES}
    ENTRY_POINT BioFormatsWrapper
)

# Include JNI and the compiled JAR paths
include_directories(${JNI_INCLUDE_DIRS})
include_directories(${JNI_INCLUDE_DIRS}/win32) # Use if on Windows

# Specify the main C++ source file
add_executable(BioFormatsWrapper_Example src/main.cpp)

# Link the Java JAR as a library to the C++ executable
add_dependencies(BioFormatsWrapper_Example BioFormatsWrapper)

# Link JNI library
target_link_libraries(BioFormatsWrapper_Example ${JNI_LIBRARIES})