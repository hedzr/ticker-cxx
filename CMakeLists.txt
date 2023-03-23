cmake_minimum_required(VERSION 3.9..3.23)

#### PREPARATIONS ######################################################
set(CMAKE_SCRIPTS "cmake")
set(CMAKE_MODULE_PATH
        ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/Modules
        ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}
        ${CMAKE_SOURCE_DIR}  # for .version.cmake & .options.cmake
        ${CMAKE_MODULE_PATH})
#### INCLUDES ##########################################################
include(prerequisites)
#use_vcpkg()


set(PROJECT_MACRO_NAME ticker_cxx)
set(PROJECT_MACRO_SHORT_NAME tk)
set(PROJECT_MACRO_MID_NAME ticker-cxx)
set(PROJECT_MACRO_PREFIX "TICKER_CXX")


# ### OPTIONS ##################################################

# option(OK "PLL-OK" ON)

# option(${PROJECT_MACRO_PREFIX}_BUILD_TESTS_EXAMPLES "Build Test Examples OK" OFF)
# option(${PROJECT_MACRO_PREFIX}_BUILD_DOCS "Build Docs-OK" OFF)

# the following options are just used for executables
option(${PROJECT_MACRO_PREFIX}_ENABLE_ASSERTIONS "Enable Assertions-OK" ON)
option(${PROJECT_MACRO_PREFIX}_ENABLE_PRECONDITION_CHECKS "Enable Precondition Checks-OK" ON)
option(${PROJECT_MACRO_PREFIX}_ENABLE_THREAD_POOL_READY_SIGNAL "Enable ready signal actions in thread_pool template class" ON)
option(${PROJECT_MACRO_PREFIX}_ENABLE_VERBOSE_LOG "Enable `dbg_verbose_debug` macro definition (TRACE MODE)" OFF)
option(${PROJECT_MACRO_PREFIX}_TEST_THREAD_POOL_DBGOUT "Enable `pool_debug` macro definition" OFF)
option(${PROJECT_MACRO_PREFIX}_UNIT_TEST "Enable the extra unit-tests" OFF)



#### PROJECTS PREPARE ##################################################

include(CTest)

find_package(Threads REQUIRED)


#### PROJECTS ##########################################################


project(${PROJECT_MACRO_NAME}
	VERSION ${VERSION}
	DESCRIPTION "${PROJECT_MACRO_MID_NAME} - a timer/ticker/cron/scheduler library."
	LANGUAGES CXX)

message(STATUS "---- define ${PROJECT_NAME} / ${PROJECT_MACRO_NAME} ------------")
define_cxx_library_project(${PROJECT_NAME} 
        INTERFACE
        PREFIX ${PROJECT_MACRO_PREFIX}
        INSTALL GENERATE_CONFIG

        #        SOURCES
        #        core.cc
        #        driver.cc

        HEADERS
        #${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx.hh
        #${CMAKE_GENERATED_DIR}/${PROJECT_NAME}-version.hh
        #${CMAKE_GENERATED_DIR}/${PROJECT_NAME}-config.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_MACRO_NAME}.hh
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-version.hh
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_MACRO_NAME}/ticker.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-core.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-anchors.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-assert.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-chrono.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-common.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-config.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-core.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-dbg.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-def.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-if.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-jobs.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-log.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-periodical-job.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-pool.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-timer-job.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-x-class.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/ticker-x-test.hh
        )
message(STATUS "---- defined ${PROJECT_NAME} / ${PROJECT_MACRO_NAME} ------------")


# # TPL ## TAIL ##########
message(STATUS "tail of top level : ")

# apply_all_unit_tests(all_tests)     # invoke CTest unittests automatically.
# dump_feature_summary()              # tail print to cmake-debug/features.log
# env var CTEST_OUTPUT_ON_FAILURE=1 to display ctest error verbosely.
# ctest cmdline options: --verbose or --extra-verbose or --verbose-on-failure
# debug_print_value(DEBUG)
# debug_print_value(BUILD_DOCUMENTATION)
