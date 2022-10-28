cmake_minimum_required(VERSION 3.10)
project(MyGLProject)
set(CMAKE_CXX_STANDARD 11)

# --------------------- INCLUDE --------------------- #

include_directories(src)
include_directories(src/vendor)

include_directories(src/vendor/glew/include)
include_directories(src/vendor/glfw/include)

include_directories(src/vendor/glm)
include_directories(src/vendor/assimp)
include_directories(src/vendor/imgui)
include_directories(src/vendor/stb_image)


aux_source_directory(. MyGLSrc)
aux_source_directory(./src MyGLSrc)
aux_source_directory(./src/collision MyGLSrc)
aux_source_directory(./src/game MyGLSrc)
aux_source_directory(./src/ogCat MyGLSrc)
aux_source_directory(./src/vendor/imgui MyGLSrc)

message(STATUS "all src: ${MyGLSrc}")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}")

# --------------------- LINK --------------------- #
add_executable(MyGL ${MyGLSrc})
target_link_libraries(MyGL opengl32)
target_link_libraries(MyGL "${CMAKE_CURRENT_SOURCE_DIR}/src/vendor/glew/lib/glew32.lib")
target_link_libraries(MyGL "${CMAKE_CURRENT_SOURCE_DIR}/src/vendor/glfw/lib/libglfw3dll.a")
