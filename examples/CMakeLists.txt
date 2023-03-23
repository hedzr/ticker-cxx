project(examples
        VERSION ${VERSION}
        DESCRIPTION "examples - all examples apps for ticker-cxx library"
        LANGUAGES CXX)

set(PROJECT_ARCHIVE_NAME ${PROJECT_NAME}s-${PROJECT_VERSION})

define_example_program(simple-app simple-app.cc LIBRARIES libs::ticker_cxx)

message(STATUS "END of examples")