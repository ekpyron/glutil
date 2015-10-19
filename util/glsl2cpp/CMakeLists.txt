if (NOT CMAKE_CROSSCOMPILING)
   file (GLOB GLSL2CPP_SOURCES main.cpp)

   add_executable (glsl2cpp ${GLSL2CPP_SOURCES})
   target_link_libraries (glsl2cpp lz4)

   install (TARGETS glsl2cpp EXPORT glutil RUNTIME DESTINATION bin)
endif (NOT CMAKE_CROSSCOMPILING)
