if (EXISTS ${CMAKE_SOURCE_DIR}/.version.cmake)
    include(.version)
    message(STATUS ">>> version decl file found: ${CMAKE_SOURCE_DIR}/.version.cmake (V${VERSION})")
    #elseif (EXIST)
else ()
    message(STATUS ">>> version decl file ignored")
    set(VERSION 0.1.0.1)
endif ()