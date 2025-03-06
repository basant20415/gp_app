cmake_minimum_required(VERSION 3.0)

project(PRINT)  # Project name

set(CMAKE_CXX_STANDARD 17)

# Include the directory where math.hpp is located
include_directories( includes / utils)

# Source files
set(SOURCE
    src/aws.cpp
    
)
find_package(PahoMqttCpp REQUIRED)
# find_package(AWSSDK REQUIRED COMPONENTS iot)
# Add executable
add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(PRINT PahoMqttCpp::paho-mqttpp3)
# target_link_libraries(my_app AWS::aws-cpp-sdk-iot)
