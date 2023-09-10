cmake_minimum_required(VERSION 3.1)

project(project)

# prevent for in-source building
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -O3")

#set(SFML_USE_DRM TRUE)

message("------------------------")

set(SFML_STATIC_LIBRARIES TRUE)

# if(EXISTS "${CMAKE_SOURCE_DIR}/SFML_path.cmake")
#     include("${CMAKE_SOURCE_DIR}/SFML_path.cmake")
#     message("used file SFML_path.cmake: ${SFML_DIR}")
# endif()

set(SFML_DIR "C:/SFML-2.5.1/lib/cmake/SFML")

if (WIN32)
  if(NOT SFML_DIR)
    set(SFML_DIR "${CMAKE_SOURCE_DIR}/SFML/lib/cmake/SFML")
    message("used default SFML_DIR: ${SFML_DIR}")
  endif()
  if(NOT DEFINED SFML_STATIC_LIBRARIES)
    set(SFML_STATIC_LIBRARIES TRUE)
    message("used default SFML_STATIC_LIBRARIES: ${SFML_STATIC_LIBRARIES}")
  endif()
endif (WIN32)

message("sfml_dir: ${SFML_DIR}")
message("------------------------")

file(GLOB sources "src/*.cpp")

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(OpenMP)
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} sfml-graphics OpenMP::OpenMP_CXX)


if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC )
else()
    message(SEND_ERROR "OpenMP not found")
endif()

set_target_properties(PROPERTIES LINKER_LANGUAGE CXX)

#Uncomment for font support
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_directory
       "${PROJECT_SOURCE_DIR}/fonts"
       $<TARGET_FILE_DIR:${PROJECT_NAME}>/fonts)