
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/emscripten)

# all source files
set(SRC ${SRCROOT}/Emscripten.cpp)

set(CMAKE_EXECUTABLE_SUFFIX ".html")

# define the emscripten target
add_executable(emscripten ${SRC})

set_target_properties(emscripten PROPERTIES DEBUG_POSTFIX -d)
set_target_properties(emscripten PROPERTIES FOLDER "Examples")

set(LINK_FLAGS "")

set(LINK_FLAGS "${LINK_FLAGS} --preload-file resources/ball.wav")
set(LINK_FLAGS "${LINK_FLAGS} --preload-file resources/sansation.ttf")

set_target_properties(emscripten PROPERTIES LINK_FLAGS ${LINK_FLAGS})

target_link_libraries(emscripten sfml-graphics sfml-audio sfml-window sfml-network sfml-system)

# add the install rule
install(TARGETS emscripten
        RUNTIME DESTINATION ${INSTALL_MISC_DIR}/examples/emscripten COMPONENT examples
        BUNDLE DESTINATION ${INSTALL_MISC_DIR}/examples/emscripten COMPONENT examples)

# install the example's source code
install(FILES ${SRC}
        DESTINATION ${INSTALL_MISC_DIR}/examples/emscripten
        COMPONENT examples)

# install the example's resources as well
set(EXAMPLE_RESOURCES "${CMAKE_SOURCE_DIR}/examples/emscripten/resources")
if(EXISTS ${EXAMPLE_RESOURCES})
    install(DIRECTORY ${EXAMPLE_RESOURCES}
            DESTINATION ${INSTALL_MISC_DIR}/examples/emscripten
            COMPONENT examples)
endif()
