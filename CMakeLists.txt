cmake_minimum_required(VERSION 3.3)
project(glutil)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

configure_file (glutil-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/glutil-config.cmake @ONLY)

add_subdirectory (util)
add_subdirectory (glutil)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/glutil-config.cmake DESTINATION lib/cmake/glutil)
install (DIRECTORY glutil DESTINATION include FILES_MATCHING PATTERN "*.h")
install (EXPORT glutil DESTINATION lib/cmake/glutil)
