 # hakkero/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(Hakkero LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Hakkero SHARED
  # Core
  Core/Engine.cpp
  Core/Application.cpp

  # Graphics
  Graphics/Vulkan/Context.cpp
  Graphics/Shader.cpp

  # Audio
  Audio/AudioEngine.cpp
)

target_include_directories(Hakkero PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(Hakkero PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Graphics/Vulkan
)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(OpenAL REQUIRED)

target_link_libraries(Hakkero PRIVATE
  glfw
  Vulkan::Vulkan
  OpenAL::OpenAL
)

target_compile_options(Hakkero PRIVATE -Wall -Wextra -Werror)

install(TARGETS Hakkero
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ 
  DESTINATION Hakkero
  FILES_MATCHING 
  PATTERN "*.hpp" 
  PATTERN "*.cpp"
)
