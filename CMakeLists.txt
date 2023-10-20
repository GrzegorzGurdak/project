cmake_minimum_required(VERSION 3.11)
include(FetchContent)

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
# set(GLUT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/glut-3.7.6-bin")
# set(GLUT_glut_LIBRARY "${CMAKE_SOURCE_DIR}/glut-3.7.6-bin/glut32.lib")

# ExternalProject_Add(GLUT
#     DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}
#     URL https://github.com/markkilgard/glut.git
#     UPDATE_COMMAND ""
#     SOURCE_DIR ${make_lib_SOURCE_DIR}
#     BUILD_IN_SOURCE 1
#     CONFIGURE_COMMAND ""
#     BUILD_COMMAND "make -j8"
#     INSTALL_COMMAND "${make_lib_install_commands}"
# )

# add_library(make_lib_libA STATIC IMPORTED GLOBAL)
# set_property(TARGET make_lib_libA 
#     PROPERTY IMPORTED_LOCATION 
#     ${make_lib_SOURCE_DIR}/path/to/make_lib_libA.a
# )

# add_dependencies(myOtherLib make_lib)

FetchContent_Declare(
  GLUT
  GIT_REPOSITORY
  https://github.com/FreeGLUTProject/freeglut
)

FetchContent_GetProperties(GLUT)

if(NOT GLUT_POPULATED)
  FetchContent_Populate(GLUT)
  add_subdirectory(${glut_SOURCE_DIR} ${glut_BINARY_DIR})
endif()

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

# include_directories(${CMAKE_SOURCE_DIR}/glut-3.7.6-bin/include)
include_directories(${glut_SOURCE_DIR}/include)

file(GLOB sources "src/*.cpp") # "glut-3.7.6-bin/*.h")

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(OpenMP)
find_package(OpenGL)
# find_package(GLUT REQUIRED)
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} sfml-graphics OpenMP::OpenMP_CXX OpenGL::GLU ${glut_BINARY_DIR}/lib/Release/freeglut.lib)# ${CMAKE_SOURCE_DIR}/glut-3.7.6-bin/lib/glut32.lib)

# target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/glut-3.7.6-bin/glut32.lib)

if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC )
    message("found OpenMP")
else()
    message(SEND_ERROR "OpenMP not found")
endif()

set_target_properties(PROPERTIES LINKER_LANGUAGE CXX)

#Uncomment for font support
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${PROJECT_SOURCE_DIR}/fonts"
  $<TARGET_FILE_DIR:${PROJECT_NAME}>/fonts)

# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy_if_different
#   ${CMAKE_SOURCE_DIR}/glut-3.7.6-bin/mylib.dll
#   $<TARGET_FILE_DIR:${PROJECT_NAME}>)