cmake_minimum_required(VERSION 3.20)
set(Hakkero_VERSION 0.1.0)
project(Hakkero VERSION ${Hakkero_VERSION} LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Hakkero SHARED 
    # NOTE: There has to be a better way to do this. Check on it later.
    core/logger.cpp
    core/time_utils.cpp
    core/vulkan/vulkan_instance.cpp
    core/vulkan/vulkan_utils.cpp
    core/vulkan/vulkan_device.cpp
    core/vulkan/vulkan_types.cpp
    core/vulkan/vulkan_surface.cpp
    core/vulkan/vulkan_swapchain.cpp
    core/vulkan/vulkan_image.cpp
    core/vulkan/vulkan_init.cpp
    core/vulkan/vulkan_pipeline.cpp
    core/vulkan/vulkan_command_buffer.cpp
    core/vulkan/vulkan_render.cpp
)

include(GenerateExportHeader)
generate_export_header(Hakkero BASE_NAME Hakkero)

target_include_directories(Hakkero PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>  
  $<INSTALL_INTERFACE:include>                  
  ${CMAKE_CURRENT_BINARY_DIR}     
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(Hakkero PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/platform/window/glfw
  ${CMAKE_CURRENT_SOURCE_DIR}/core
  ${CMAKE_CURRENT_SOURCE_DIR}/core/vulkan
)

find_package(glfw3 3.4 REQUIRED)
find_package(Vulkan REQUIRED COMPONENTS glslc)
find_package(OpenAL REQUIRED)

target_link_libraries(Hakkero
  glfw
  Vulkan::Vulkan
  OpenAL::OpenAL
)

target_compile_options(Hakkero PRIVATE -Wall -Wextra -Werror)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hakkero.pc
  DESTINATION lib/pkgconfig
)

option(BUILD_SAMPLES "Build samples" ON)

install(TARGETS Hakkero
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ 
  DESTINATION Hakkero
  FILES_MATCHING 
  PATTERN "*.hpp" 
  PATTERN "*.cpp"
  PATTERN "samples" EXCLUDE
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hakkero.pc
  DESTINATION lib/pkgconfig
)

if(BUILD_SAMPLES)
    if(TARGET Hakkero AND glfw3_FOUND AND Vulkan_FOUND)
        # Enable shader compilation
        find_program(GLSLC_EXECUTABLE NAMES glslc HINTS Vulkan::glslc)
        
        # Find all sample directories
        file(GLOB sample_dirs LIST_DIRECTORIES true RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/samples" "${CMAKE_CURRENT_SOURCE_DIR}/samples/*")
        
        foreach(sample_dir ${sample_dirs})
            if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/samples/${sample_dir}")
                # Check if the directory has a CMakeLists.txt
                if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/samples/${sample_dir}/CMakeLists.txt")
                    message(STATUS "Adding sample: ${sample_dir}")
                    add_subdirectory(samples/${sample_dir})
                endif()
            endif()
        endforeach()
    else()
        message(WARNING "Samples skipped - missing dependencies")
        message(STATUS "Hakkero target exists: ${TARGET Hakkero}")
        message(STATUS "GLFW found: ${glfw3_FOUND}")
        message(STATUS "Vulkan found: ${Vulkan_FOUND}")
    endif()
endif()
