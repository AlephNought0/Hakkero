add_executable(Meow main.cpp)
target_link_libraries(Meow PRIVATE Hakkero)

if(WIN32)
  add_custom_command(TARGET Meow POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:Hakkero>
            $<TARGET_FILE_DIR:Meow>
    )
endif()
