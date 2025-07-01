add_executable(Nesquik main.cpp)
target_link_libraries(Nesquik PRIVATE Hakkero)

if(WIN32)
  add_custom_command(TARGET Nesquik POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:Hakkero>
            $<TARGET_FILE_DIR:Nesquik>
    )
endif()
