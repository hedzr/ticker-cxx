cmake_minimum_required(VERSION 3.9..3.21)

#### PREPARATIONS ##################################################
set(CMAKE_SCRIPTS "cmake")
set(CMAKE_MODULE_PATH
        ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/modules
        ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}
        ${CMAKE_SOURCE_DIR}  # for .version.cmake & .options.cmake
        ${CMAKE_MODULE_PATH})
#### INCLUDES ##################################################
include(prerequisites)
include(dummy-project)


set(PROJECT_MACRO_NAME ticker_cxx)
set(PROJECT_MACRO_SHORT_NAME tk)
set(PROJECT_MACRO_MID_NAME ticker-cxx)
set(PROJECT_MACRO_PREFIX "TICKER_CXX")
#gen_versions(${PROJECT_MACRO_NAME} ${PROJECT_MACRO_PREFIX}
#        ${PROJECT_MACRO_SHORT_NAME}-version.hh
#        ${PROJECT_MACRO_SHORT_NAME}-config.hh
#        ${PROJECT_MACRO_SHORT_NAME}-${PROJECT_VERSION}
#        ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/version.h.in
#        ${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/config-base.h.in
#        )


#### OPTIONS ##################################################

option(OK "PL-OK" ON)

#option(TICKER_CXX_BUILD_TESTS_EXAMPLES "Build Test Examples OK" OFF)
#option(TICKER_CXX_BUILD_DOCS "Build Docs-OK" OFF)

# the following options are just used for executables
option(TICKER_CXX_ENABLE_ASSERTIONS "Enable Assertions-OK" ON)
option(TICKER_CXX_ENABLE_PRECONDITION_CHECKS "Enable Precondition Checks-OK" ON)
option(TICKER_CXX_ENABLE_THREAD_POOL_READY_SIGNAL "Enable ready signal actions in thread_pool template class" ON)
option(TICKER_CXX_ENABLE_VERBOSE_LOG "Enable `dbg_verbose_debug` macro definition (TRACE MODE)" OFF)
option(TICKER_CXX_TEST_THREAD_POOL_DBGOUT "Enable `pool_debug` macro definition" OFF)
option(TICKER_CXX_UNIT_TEST "Enable the extra unit-tests" OFF)


#### PROJECTS ##################################################


project(ticker_cxx
        VERSION ${VERSION}
        DESCRIPTION "ticker-cxx - a timer/ticker/cron/scheduler library."
        LANGUAGES CXX)
set(PROJECT_MACRO_NAME ticker_cxx)
set(PROJECT_MACRO_PREFIX TICKER_CXX)
set(TICKER_CXX_detail_header_files
        #${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx/detail/ticker-if.hh
        )
set(TICKER_CXX_header_files
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ticker_cxx.hh
        ${CMAKE_GENERATED_DIR}/${PROJECT_NAME}-version.hh
        ${CMAKE_GENERATED_DIR}/${PROJECT_NAME}-config.hh
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
define_installable_cxx_library_project(${PROJECT_NAME} ${PROJECT_MACRO_PREFIX})


#project(undo_cxx)
#set(PROJECT_MACRO_PREFIX UNDO_CXX)
#set(UNDO_CXX_detail_header_files
#        ${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/detail/undo-if.hh
#        )
#set(UNDO_CXX_header_files
#        ${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx.hh
#        ${CMAKE_GENERATED_DIR}/${PROJECT_NAME}-version.hh
#        ${CMAKE_GENERATED_DIR}/${PROJECT_NAME}-config.hh
#        ${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo.hh
#        ${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo-common.hh
#        ${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo-def.hh
#        ${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo-log.hh
#        ${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo-unmgr.hh
#        )
#define_installable_cxx_library_project(${PROJECT_NAME} ${PROJECT_MACRO_PREFIX})


##### TESTS ##################################################
#
#if (${TICKER_CXX_BUILD_TESTS_EXAMPLES} OR (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
#    enable_testing()
#    add_subdirectory(examples/)
#    add_subdirectory(tests/)
#endif ()
## TPL ## TAIL ##########

message(STATUS "tail of top level : ")
#apply_all_unit_tests(all_tests)     # invoke CTest unittests automatically.
#dump_feature_summary()              # tail print to cmake-debug/features.log
# env var CTEST_OUTPUT_ON_FAILURE=1 to display ctest error verbosely.
# ctest cmdline options: --verbose or --extra-verbose or --verbose-on-failure
#debug_print_value(DEBUG)
#debug_print_value(BUILD_DOCUMENTATION)
