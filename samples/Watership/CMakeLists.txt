add_executable(Watership gateboss.cpp)
target_link_libraries(Watership PRIVATE Hakkero)

if(WIN32)
  add_custom_command(TARGET Watership POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:Watership>
            $<TARGET_FILE_DIR:Watership>
    )
endif()
