cmake_minimum_required(VERSION 3.14)
project(tello_msgs)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Default to C++14
if (NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif ()

# Debugging: set _dump_all_variables to true
set(_dump_all_variables false)
if (_dump_all_variables)
  get_cmake_property(_variable_names VARIABLES)
  list(SORT _variable_names)
  foreach (_variable_name ${_variable_names})
    message(STATUS "${_variable_name}=${${_variable_name}}")
  endforeach ()
endif ()

# Find packages
find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)

# Generate ROS interfaces
rosidl_generate_interfaces(
  ${PROJECT_NAME}
  "msg/FlightData.msg"
  "msg/TelloResponse.msg"
  "srv/TelloAction.srv"
  DEPENDENCIES std_msgs
  LIBRARY_NAME ${PROJECT_NAME}
)

ament_export_dependencies(rosidl_default_runtime)
ament_package()
