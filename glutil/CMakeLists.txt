find_package (OGLP REQUIRED)

set (GLUTIL_SOURCES CircularBuffer.cpp FullscreenQuad.cpp LoadProgram.cpp LoadTexture.cpp
        SimpleAllocator.cpp StaticBufferManager.cpp)

file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)
add_custom_command (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shaders/fsquad.cpp
                    COMMAND glsl2cpp -S source
                                     -H "<glutil/shader.h>"
                                     -P "namespace shader {"
                                     -P "extern const struct source fsquad;"
                                     -A "} /* namespace shader */"
                                    fsquad ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fsquad.glsl
                                    ${CMAKE_CURRENT_BINARY_DIR}/shaders/fsquad.cpp
                    DEPENDS glsl2cpp ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fsquad.glsl
                                     ${CMAKE_CURRENT_SOURCE_DIR}/shader.h
                    VERBATIM)

add_library (glutil SHARED ${GLUTIL_SOURCES})

target_include_directories (glutil PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)
target_include_directories (glutil SYSTEM PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include> ${OGLP_INCLUDE_DIRS})

set_property (TARGET glutil PROPERTY COMPILE_FLAGS -std=c++14)

install (TARGETS glutil EXPORT glutil LIBRARY DESTINATION lib)
