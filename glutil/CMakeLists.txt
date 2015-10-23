find_package (OGLP REQUIRED)
find_package (LZ4 REQUIRED)

file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)
add_custom_command (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shaders/fsquad.cpp
                    COMMAND glsl2cpp -S "glutil::shadersource"
                                     -H "<glutil/shader.h>"
                                     -P "namespace shader {"
                                     -P "extern const struct glutil::shadersource fsquad;"
                                     -A "} /* namespace shader */"
                                    fsquad ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fsquad.glsl
                                    ${CMAKE_CURRENT_BINARY_DIR}/shaders/fsquad.cpp
                    DEPENDS glsl2cpp ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fsquad.glsl
                                     ${CMAKE_CURRENT_SOURCE_DIR}/shader.h
                    VERBATIM)

set (GLUTIL_SOURCES CircularBuffer.cpp FullscreenQuad.cpp LoadProgram.cpp LoadTexture.cpp
        SimpleAllocator.cpp StaticBufferManager.cpp ${CMAKE_CURRENT_BINARY_DIR}/shaders/fsquad.cpp
        AttribPacker.cpp AttribPacker.h)

add_library (glutil SHARED ${GLUTIL_SOURCES})

target_include_directories (glutil PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>)
target_include_directories (glutil SYSTEM PUBLIC ${OGLP_INCLUDE_DIRS})
target_include_directories (glutil SYSTEM PRIVATE ${LZ4_INCLUDE_DIR})

target_link_libraries (glutil ${LZ4_LIBRARY})

set_property (TARGET glutil PROPERTY COMPILE_FLAGS -std=c++14)

install (TARGETS glutil EXPORT glutil LIBRARY DESTINATION lib)
