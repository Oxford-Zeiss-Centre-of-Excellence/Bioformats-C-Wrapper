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

set(JAVA_INCLUDES
    ${JAVA_OUTPUT_DIR}/bioformats_package.jar
    ${JAVA_OUTPUT_DIR}/bio-formats-tools.jar 
    ${JAVA_OUTPUT_DIR}/formats-gpl.jar 
    ${JAVA_OUTPUT_DIR}/formats-api.jar
)

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